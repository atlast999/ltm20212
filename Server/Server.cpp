// Server.cpp : Defines the entry point for the console application.
//
#include <iostream>
#include <string>

#include <winsock2.h>
#include <ws2tcpip.h>

#include "constant.h"
#include "model.h"
#include "controller.h"
#include "helper.h"

#include <list>

#include <process.h>


#pragma comment(lib, "Ws2_32.lib")

HANDLE ioCompletionPort;
class ServerSocket {
private:
	SOCKET socket;
	int port;
	string ip;
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
		cout << "Init winsock successfully" << endl;
		return 1;
	}
	/*
* Create TCP socket
* param: instance[OUT] - pointer to the socket created
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
* params: instance[IN/OUT] - the socket to be bond
*		  port[IN] - a host's port to be bond
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
			_beginthreadex(0, 0, ServerSocket::serveIOThread, (void*)&i, 0, 0);
		}
		return 1;
	}
public:
	
	ServerSocket() {
		this->port = 8080;
		this->ip = "127.0.0.1";
		ioCompletionPort = NULL;
	}
	bool start() {
		InitializeCriticalSection(&criticalSection);
		if (!initWinsock()) return 0;
		if (!initIOCP()) return 0;
		if (!initTCPSocket()) return 0;
		if (!bindAddToSocket()) return 0;
		// Keep trying to listen until success
		while (listen(socket, 10))
		{
			cout << "Error in listenning! Attempting to try again..." << WSAGetLastError() << endl;;
		}

		cout << endl
			<< "Server started at port: " << port << endl;
		_beginthreadex(0, 0, ServerSocket::acceptThread, (void*)this->socket, 0, 0);
		while (true);
		return 1;

	}

public:
	static unsigned __stdcall acceptThread(LPVOID socket) {
		SOCKET server = (SOCKET)socket;
		sockaddr_in clientAddr;
		while (true) {
			SOCKET client = WSAAccept(server, (sockaddr*)&clientAddr, NULL, NULL, 0);
			if (client == INVALID_SOCKET) {
				continue;
			}
			AppSession* session = new AppSession(client);
			session->opCode = OP_READ;
			addNewSession(session);
			HANDLE associate = CreateIoCompletionPort((HANDLE)session->client, ioCompletionPort, (DWORD)session, 0);
			if (associate == NULL) {
				removeSession(session);
				continue;
			}
			session->opCode = OP_WRITE;
			DWORD pBytes = 0, pFlag = 0;
			WSARecv(session->client, session->wsaBuf, 1,
				&pBytes, &pFlag, session->overlap, NULL);
		}
	}

	static unsigned __stdcall serveIOThread(LPVOID threadIndex) {
		DWORD bytesTransferred = 0, byteReceived = 0, byteSent = 0;
		OVERLAPPED* overlapped = NULL;
		void* pSession = NULL;
		AppSession* session = NULL;
		DWORD pBytes = 0, pFlag = 0;
		//AppController  controller;
		while (true) {
			int ret = GetQueuedCompletionStatus(
				ioCompletionPort,
				&bytesTransferred,
				(PULONG_PTR)session,
				&overlapped,
				INFINITE
			);
			if (ret == 0 || bytesTransferred == 0) {
				//lost connection
				removeSession(session);
				continue;
			}
			switch (session->opCode) {
			case OP_READ:
				session->opCode = OP_WRITE;
				session->clearBuffer();
				byteReceived = WSARecv(
					session->client,
					session->wsaBuf,
					1,
					&pBytes,
					&pFlag,
					session->overlap,
					NULL
				);
				if (byteReceived == SOCKET_ERROR) {
					removeSession(session);
				}
				break;
			case OP_WRITE:
				session->opCode = OP_READ;
				//string response = controller.handleRequest(string(session->buffer));

				session->sentBytes = 0;
				session->wsaBuf->buf;

				byteSent = WSASend(
					session->client,
					session->wsaBuf,
					1,
					&pBytes,
					pFlag,
					session->overlap,
					NULL
				);
				if (byteSent == SOCKET_ERROR) {
					removeSession(session);
				}
				break;
			}
		}
	}
};


int main()
{

	AppController controller;

	//SignUpRequest signUpRequest("admin2", "admin");
	//string res1 = controller.handleRequest(signUpRequest.serialize());
	//SignUpResponse signUpResponse;
	//signUpResponse.deserialize(res1);
	//cout << res1 << endl;
	//cout << (signUpResponse.code == CODE_ERROR) << endl;

	//LogInRequest loginRequest("admin1", "admin");
	//string res2 = controller.handleRequest(loginRequest.serialize());
	//LogInResponse loginResponse;
	//loginResponse.deserialize(res2);
	//cout << res2 << endl;
	//cout << (loginResponse.code == CODE_SUCCESS) << endl;

	//ListEventRequest eventRequest(0, 2);
	//string res3 = controller.handleRequest(eventRequest.serialize());
	//ListEventResponse eventResponse;
	//eventResponse.deserialize(res3);
	//cout << res3 << endl;
	//cout << (eventResponse.code == CODE_SUCCESS) << endl;

	//ListRequestRequest appRequest(1);
	//string res4 = controller.handleRequest(appRequest.serialize());
	//ListRequestResponse appRequestResponse;
	//appRequestResponse.deserialize(res4);
	//cout << res4 << endl;
	//cout << (appRequestResponse.code == CODE_SUCCESS) << endl;

	//DetailEventRequest edRequest(1, 1);
	//string res5 = controller.handleRequest(edRequest.serialize());
	//DetailEventResponse edResponse;
	//edResponse.deserialize(res5);
	//cout << res5 << endl;
	//cout << (edResponse.code == CODE_SUCCESS) << endl;

	//Event* event = new Event("created event 2", "test create 2", "a time", "a place");
	//CreateEventRequest ceRequest(event, 2);
	//string res6 = controller.handleRequest(ceRequest.serialize());
	//CreateEventResponse edResponse;
	//edResponse.deserialize(res6);
	//cout << res6 << endl;
	//cout << (edResponse.code == CODE_SUCCESS) << endl;

	//FreeUsersRequest fusersRequest(4, 2);
	//string res7 = controller.handleRequest(fusersRequest.serialize());
	//FreeUsersResponse fusersResponse;
	//fusersResponse.deserialize(res7);
	//cout << res7 << endl;
	//cout << (fusersResponse.code == CODE_SUCCESS) << endl;

	//CreateAskRequest askRequest(4, 2, 3);
	//string res8 = controller.handleRequest(askRequest.serialize());
	//CreateAskResponse askResponse;
	//askResponse.deserialize(res8);
	//cout << res8 << endl;
	//cout << (askResponse.code == CODE_SUCCESS) << endl;

	//CreateInviteRequest inviteRequest;
	//string res9 = controller.handleRequest(inviteRequest.serialize());
	//CreateInviteResponse inviteResponse;
	//inviteResponse.deserialize(res9);
	//cout << res9 << endl;
	//cout << (inviteResponse.code == CODE_SUCCESS) << endl;

	//UpdateRequest uRequest(4, 1, 2);
	//string res10 = controller.handleRequest(uRequest.serialize());
	//UpdateResponse uResponse;
	//uResponse.deserialize(res10);
	//cout << res10 << endl;
	//cout << (uResponse.code == CODE_SUCCESS) << endl;

	ServerSocket server;
	server.start();
	system("pause");
	return 0;
}
