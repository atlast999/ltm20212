#pragma once
#ifndef SOCKHELPER_H_
#define SOCKHELPER_H_

#include <winsock2.h>
#include <ws2tcpip.h>

#include <list>

#include "controller.h"

using namespace std;

#define OP_READY_TO_RECEIVE 0
#define OP_READY_TO_SEND 1
#define BUFFER_SIZE 20240
/**
 * The class represents attributes associated with client socket
 * Used as completionKey in IOCP related functions
 */
class AppSession {
public:
	SOCKET client;
	OVERLAPPED* overlap;
	WSABUF* wsaBuf;
	char buffer[BUFFER_SIZE];
	int totalBytes;
	int sentBytes;
	int opCode;

	AppSession() {
		this->client = SOCKET_ERROR;
		this->overlap = new OVERLAPPED;
		ZeroMemory(overlap, sizeof(OVERLAPPED));
		ZeroMemory(buffer, BUFFER_SIZE);
		this->wsaBuf = new WSABUF;
		this->wsaBuf->buf = buffer;
		this->wsaBuf->len = BUFFER_SIZE;
		this->totalBytes = 0;
		this->sentBytes = 0;
		this->opCode = 0;
	}
	AppSession(SOCKET socket) : AppSession() {
		this->client = socket;
	}
	~AppSession() {
		closesocket(this->client);
		delete this->overlap;
		delete this->wsaBuf;
	}
	void clearBuffer() {
		ZeroMemory(buffer, BUFFER_SIZE);
		this->wsaBuf->buf = buffer;
		this->wsaBuf->len = BUFFER_SIZE;
	}
};
/**
 * Global variables and functions
 * Used for managing client connections synchronously
 */
CRITICAL_SECTION criticalSection;
list<AppSession*> appSessions;
void addNewSession(AppSession* session) {
	EnterCriticalSection(&criticalSection);
	appSessions.emplace_back(session);
	cout << "A client was added to list: " << session->client << endl;
	LeaveCriticalSection(&criticalSection);
}
void removeSession(AppSession* session) {
	EnterCriticalSection(&criticalSection);
	appSessions.remove(session);
	cout << "A client was deleted from list: " << session->client << endl;
	delete session;
	LeaveCriticalSection(&criticalSection);
}
HANDLE ioCompletionPort;

/**
 * A class represents attributes of server
 */
class ServerSocket {
private:
	SOCKET socket;
	string ip;
	int port;
	AppController* controller;

	/*
	* Initialize for using Winsock
	* return true if success, else otherwise
	*/
	bool initWinsock()
	{
		WORD wsVersion = MAKEWORD(2, 2);
		WSADATA wsaData;
		if (WSAStartup(wsVersion, &wsaData))
		{
			cout << endl
				<< "Winsock 2.2 is not supported" << endl;
			return 0;
		}
		return 1;
	}
	/*
	* Create TCP socket with overlapped option
	* return true if success, false otherwise
	*/
	bool initTCPSocket()
	{
		this->socket = WSASocket(AF_INET, SOCK_STREAM,
			0, NULL,
			0, WSA_FLAG_OVERLAPPED);
		if (this->socket == INVALID_SOCKET)
		{
			cout << endl
				<< "Error in initializing socket server: " << WSAGetLastError();
			return 0;
		}
		return 1;
	}
	/*
	* Bind a socket to a host's specific address
	* return true if success, false otherwise
	*/
	bool bindAddToSocket()
	{
		sockaddr_in serverAddr;
		serverAddr.sin_family = AF_INET;
		serverAddr.sin_port = htons(port);
		inet_pton(AF_INET, ip.c_str(), &serverAddr.sin_addr);
		if (bind(socket, (sockaddr*)&serverAddr, sizeof(serverAddr)))
		{
			cout << endl
				<< "Error in binding adress: " << WSAGetLastError();
			return 0;
		}
		return 1;
	}

	/*
	* Initialize global ioCompletionPort variable
	* Create worker threads for handling IOCP events
	* return true if success, false otherwise
	*/
	bool initIOCP() {
		ioCompletionPort = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
		if (ioCompletionPort == NULL) {
			cout << "Error in create IOCP: " << WSAGetLastError() << endl;
			return 0;
		}
		SYSTEM_INFO info;
		GetSystemInfo(&info);
		int nCpu = info.dwNumberOfProcessors;
		for (int i = 0; i < nCpu * 2; i++) {
			_beginthreadex(0, 0, ServerSocket::serveIOThread, (void*)this->controller, 0, 0);
		}
		return 1;
	}

public:
	ServerSocket() : ServerSocket("127.0.0.1", 8080) { }
	ServerSocket(string ip, int port) {
		this->ip = ip;
		this->port = port;
		this->controller = new AppController();
		ioCompletionPort = NULL;
	}
	~ServerSocket() {
		delete this->controller;
	}
	/*
	* Start the server
	* Create a worker threads for accepting new client connections
	*/
	bool start() {
		InitializeCriticalSection(&criticalSection);
		if (!initWinsock()) return 0;
		if (!initIOCP()) return 0;
		if (!initTCPSocket()) return 0;
		if (!bindAddToSocket()) return 0;
		// Keep trying to listen until success
		while (listen(socket, 10))
		{
			cout << "Error in listenning! Attempting to try again... " << WSAGetLastError() << endl;;
		}
		cout << "Server started at port: " << port << endl;

		_beginthreadex(0, 0, ServerSocket::acceptThread, (void*)this->socket, 0, 0);
		//Keep the server alive forever
		while (true);
		return 1;
	}

public:
	/*
	* A thread used for accepting new connections
	*/
	static unsigned __stdcall acceptThread(LPVOID socket) {
		SOCKET server = (SOCKET)socket;
		sockaddr_in clientAddr;
		while (true) {
			SOCKET client = WSAAccept(server, (sockaddr*)&clientAddr, NULL, NULL, 0);
			if (client == INVALID_SOCKET) {
				continue;
			}
			AppSession* session = new AppSession(client);
			session->opCode = OP_READY_TO_RECEIVE;
			addNewSession(session);
			//associate client's socket to IOCP with client's session as the key
			//so that we know which session operating when IOCP event notified
			HANDLE associate = CreateIoCompletionPort((HANDLE)session->client, ioCompletionPort, (ULONG_PTR)session, 0);
			if (associate == NULL) {
				removeSession(session);
				continue;
			}
			//Execute the first receive operation, following operations will be processed in io threads
			session->opCode = OP_READY_TO_SEND;
			DWORD pBytes = 0, pFlag = 0;
			WSARecv(session->client, session->wsaBuf, 1,
				&pBytes, &pFlag, session->overlap, NULL);
		}
	}
	/*
	* Threads used for processing IOCP events
	*/
	static unsigned __stdcall serveIOThread(LPVOID appController) {
		AppController* controller = (AppController*)appController;
		DWORD bytesTransferred = 0;
		WSAOVERLAPPED* overlapped;
		AppSession* session = NULL;
		DWORD pBytes = 0, pFlag = 0;
		while (true) {
			//overlapped parameter needs to be NONNULL so that session parameter could be assigned correctly
			int ret = GetQueuedCompletionStatus(
				ioCompletionPort,
				&bytesTransferred,
				(PULONG_PTR)&session,
				&overlapped,
				INFINITE
			);
			if (ret == 0 || bytesTransferred == 0) {
				//client disconnected
				removeSession(session);
				continue;
			}
			switch (session->opCode) {
			case OP_READY_TO_RECEIVE:
				session->sentBytes += bytesTransferred;
				//In case WSASend did not send all the data
				if (session->sentBytes < session->totalBytes) {
					session->opCode = OP_READY_TO_RECEIVE;
					//update wsabuf to the data that was not sent
					session->wsaBuf->buf += bytesTransferred;
					session->wsaBuf->len = session->totalBytes - session->sentBytes;
					int ret = WSASend(
						session->client,
						session->wsaBuf,
						1,
						&pBytes,
						pFlag,
						session->overlap,
						NULL
					);
					if (ret == SOCKET_ERROR && WSAGetLastError() != WSA_IO_PENDING) {
						removeSession(session);
					}
				}
				else {
					session->opCode = OP_READY_TO_SEND;
					session->clearBuffer();
					int ret = WSARecv(
						session->client,
						session->wsaBuf,
						1,
						&pBytes,
						&pFlag,
						session->overlap,
						NULL
					);
					if (ret == SOCKET_ERROR && WSAGetLastError() != WSA_IO_PENDING) {
						removeSession(session);
					}
				}
				break;
			case OP_READY_TO_SEND:
				session->opCode = OP_READY_TO_RECEIVE;
				//process received data
				string response = controller->handleRequest(string(session->buffer));
				//prepare to send back response
				strcpy_s(session->buffer, response.c_str());
				session->sentBytes = 0;
				session->totalBytes = response.size();
				session->wsaBuf->len = response.size();
				int ret = WSASend(
					session->client,
					session->wsaBuf,
					1,
					&pBytes,
					pFlag,
					session->overlap,
					NULL
				);
				if (ret == SOCKET_ERROR && WSAGetLastError() != WSA_IO_PENDING) {
					removeSession(session);
				}
				break;
			}
		}
	}
};


#endif