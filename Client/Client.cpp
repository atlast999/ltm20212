// clientApp.cpp : Defines the entry point for the console application.
//

#include <iostream>
#include <sstream>
#include <string>

#include <winsock2.h>
#include <ws2tcpip.h> //inet_pton
#include <list>

#include "util.h"

#include "model.h";
#include "constant.h";

#define BUFFER_SIZE 2048
#define gets_s(x, len) fgets((x), (len) + 1, stdin)

#pragma comment(lib, "Ws2_32.lib")

using namespace std;

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
* Create TCP socket
* param: instance[OUT] - pointer to the socket created
* return true if success, false otherwise
*/
bool initTCPSocket(SOCKET* instance)
{
	*instance = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (*instance == INVALID_SOCKET)
	{
		cout << endl
			<< "Error in initializing socket server: " << WSAGetLastError();
		return 0;
	}
	return 1;
}

int showFeaturesMenu(SOCKET& client);

int main(int argc, char* argv[])
{

	// Validate arguments
	if (argc != 3)
	{
		cout << "This app expects 2 argument! Actual: " << argc - 1;
		return 1;
	}
	istringstream ss(argv[2]);
	int server_port;
	if (!(ss >> server_port) || !ss.eof())
	{
		cout << "Invalid argument: " << argv[2] << '\n';
		return 1;
	}
	in_addr address;
	if (inet_pton(AF_INET, argv[1], &address) == 0)
	{
		cout << "Invalid argument: " << argv[1];
		return 1;
	}

	// Initialize winsock
	if (!initWinsock())
	{
		return 1;
	}

	// Initialize socket
	SOCKET client;
	if (!initTCPSocket(&client))
	{
		return 1;
	}

	// Initialize server info
	sockaddr_in serverAddr;
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(server_port);
	serverAddr.sin_addr.S_un.S_addr = address.S_un.S_addr;

	// send connection request to server (doing the handsakes)
	if (connect(client, (sockaddr*)&serverAddr, sizeof(serverAddr)))
	{
		printf("Error! Cannot connect server.");
		cout << WSAGetLastError();
		return 1;
	}
	cout << endl
		<< "\t\tWellcome to simple posting application. " << endl;

	// When the connection is established show user interface
	while (showFeaturesMenu(client) != 4);

	// shutdown(client, SD_SEND);
	// Close socket server
	// closesocket(client);
	// Clean winsock
	WSACleanup();

	return 0;
}
/*
* Send request to server and receive response
* param:
*  client[IN] - TCP socket used to comunicate with server
*  buffer - request content to send to server
* return response code
*/
string startComunicatingWithServer(SOCKET& client, const char* buffer)
{
	int ret, messageLen;
	ret = send(client, buffer, strlen(buffer), 0);
	if (ret == SOCKET_ERROR)
	{
		printf("Cannot send to server: %d\n", WSAGetLastError());
		return "-1";
	}

	// Receive response from server
	char* response = new char[2];
	ret = recv(client, response, 2, 0);
	if (ret == SOCKET_ERROR)
	{
		printf("Cannot recieve from server: %d", WSAGetLastError());
		return "-1";
	}
	response[ret] = 0;
	return response;
}

/*
* Get input from user
* param:
*  message[IN] - UI hint message
* return input content
*/
string getUserInput(const char* message)
{
	cout << message;
	string input;
	getline(cin, input);
	return input;
}

/*
* Show register UI
* param:
*  client[IN] - socket param to pass to startComunicatingWithServer function
* return feature code
*/
string showRegisterFeature(SOCKET& client)
{
	string username = getUserInput("Enter your username: ");
	string password = getUserInput("Enter your password: ");

	SignUpRequest request(username, password);
	string rawRequest = request.serialize();
	string response = startComunicatingWithServer(client, rawRequest.c_str());
	SignUpResponse signUpResponse;
	signUpResponse.deserialize(response);
	if(signUpResponse.message == MESSAGE_NAME_INVALID){
		return MESSAGE_NAME_INVALID;
	} else if (signUpResponse.message == MESSAGE_SUCCESS)
	{
		return MESSAGE_SUCCESS;
	}
}

/*
* Show login UI
* param:
*  client[IN] - socket param to pass to startComunicatingWithServer function
* return feature code
*/
string showLoginFeature(SOCKET& client)
{
	string username = getUserInput("Enter your username: ");
	string password = getUserInput("Enter your password: ");
	
	LogInRequest request(username, password);
	string rawRequest = request.serialize();
	string response = startComunicatingWithServer(client, rawRequest.c_str());
	LogInResponse logInResponse;
	logInResponse.deserialize(response);
	if(logInResponse.message == MESSAGE_NAME_INVALID){
		return MESSAGE_NAME_INVALID;
	} else if (logInResponse.message == MESSAGE_CREDENTIAL_INVALID)
	{
		return MESSAGE_CREDENTIAL_INVALID;
	} else if (logInResponse.message == MESSAGE_SUCCESS)
	{
		return MESSAGE_SUCCESS;
	}
	
}



/*
* Show list event UI
* param:
*  client[IN] - socket param to pass to startComunicatingWithServer function
* return feature code
*/
int showListEvent(SOCKET& client)
{
	cout << "All events" << endl;
	ListEventRequest listEventRequest;
	string rawRequest = listEventRequest.serialize();
	string response = startComunicatingWithServer(client, rawRequest.c_str());
	ListEventResponse listEventResponse;
	listEventResponse.deserialize(response);
	list<Event*> events = listEventResponse.events;
	for (Event* event : events)
		cout << event->id << endl;
	return 3;
}

void showlistEvent(list<Event*> events)
{
	for (Event* event: events)
        cout << event->id << endl;
}

/*
* Log out in this session
* param:
*  client[IN] - socket param to pass to startComunicatingWithServer function
* return feature code
*/
//int justLogout(SOCKET& client)
//{
//	vector<string> requestParts = { CMD_OUT, "\r\n" };
//	string request = joinToString(requestParts);
//	int responseCode = startComunicatingWithServer(client, request.c_str());
//	handleResponseCode(responseCode);
//	return 3;
//}
/*
* Show feature menu UI along with feature code
* param:
*  client[IN] - socket param to pass to feature function
* return feature code
*/
int showFeaturesMenu(SOCKET& client)
{
	cout << "Select a feature below:" << endl;
	cout << "1. Register" << endl;
	cout << "2. Login" << endl;
	cout << "3. List Event" << endl;
	cout << "4. List Request" << endl;
	cout << "5. Logout";
	string option;
	getline(cin, option);
	switch (atoi(option.c_str()))
	{
	case 1:
		if(showRegisterFeature(client) == MESSAGE_SUCCESS){
			cout << "---------  You have successfully registered ----------" << endl;
		} else if (showRegisterFeature(client) ==  MESSAGE_NAME_INVALID)
		{
			cout << "----- Your account already exists -----" << endl;
		}
		return 1;
		break;
	case 2:
		if(showLoginFeature(client) == MESSAGE_SUCCESS){
			cout << "--------- Successful login ----------" << endl;
		} else if ((showLoginFeature(client) ==  MESSAGE_NAME_INVALID) || (showLoginFeature(client) ==  MESSAGE_CREDENTIAL_INVALID))
		{
			cout << "--------- Account or password is incorrect ----------" << endl;
		}
		return 2;
		break;
	case 3:
		return showListEvent(client);
		break;
	case 4:
		return 4;
		break;
	default:
		cout << "\nThis option is not available." << endl;
		return -1;
	}
}