// Server.cpp : Defines the entry point for the console application.
//

#include <winsock2.h>
#include <ws2tcpip.h>
#include <process.h>

#include <iostream>
#include <string>
#include <list>

#include "helper.h"

#pragma comment(lib, "Ws2_32.lib")

int main()
{
	ServerSocket server;
	server.start();
	return 0;
}
