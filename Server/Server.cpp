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


int main(int argc, char* argv[])
{

	if (argc != 2)
	{
		cout << "This app expects 1 argument! Actual: " << argc - 1;
		return 1;
	}
	istringstream ss(argv[1]);
	int server_port;
	if (!(ss >> server_port) || !ss.eof())
	{
		cout << "Invalid argument: " << argv[1] << '\n';
		return 1;
	}

	ServerSocket server(server_port);
	server.start();
	return 0;
}
