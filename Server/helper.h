#pragma once
#ifndef SOCKHELPER_H_
#define SOCKHELPER_H_

#include <winsock2.h>
#include <ws2tcpip.h>

#include <list>


using namespace std;

#define OP_READ 0
#define OP_WRITE 1
#define BUFFER_SIZE 20240

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

CRITICAL_SECTION criticalSection;
list<AppSession*> appSessions;
void addNewSession(AppSession* session) {
	EnterCriticalSection(&criticalSection);
	appSessions.emplace_back(session);
	LeaveCriticalSection(&criticalSection);
}

void removeSession(AppSession* session) {
	EnterCriticalSection(&criticalSection);
	appSessions.remove(session);
	delete session;
	LeaveCriticalSection(&criticalSection);
}


#endif