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

#define BUFFER_SIZE 20240

#pragma comment(lib, "Ws2_32.lib")

using namespace std;

int token = -1;

void showMyEvents(SOCKET& client);
void showOtherEvents(SOCKET& client);
void showDetailEventById(SOCKET& client, int choose, int status);
int showListEventMenu(SOCKET& client);
int showFeaturesMenu(SOCKET& client);
void showMemberEvent(SOCKET& client);
void requestProcessingMenu(SOCKET& client, int requestId, int userId);
void updateStatusRequest(SOCKET& client, int requestId, int status, int userId);

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
		cout << "Error! Cannot connect server." << endl;
		cout << WSAGetLastError();
		return 1;
	}
	cout << endl
		<< "\t\tWellcome to simple posting application. " << endl;

	// When the connection is established show user interface
	while (showFeaturesMenu(client) != 6);

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
	char* response = new char[BUFFER_SIZE];
	ret = recv(client, response, BUFFER_SIZE, 0);
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
void showRegisterFeature(SOCKET& client)
{
	if (token > 0) {
		cout << "--- You are logged in" << endl;
	}
	else
	{
		string username = getUserInput("Enter your username: ");
		string password = getUserInput("Enter your password: ");

		SignUpRequest request(username, password);
		string rawRequest = request.serialize();
		cout << rawRequest << endl;
		string response = startComunicatingWithServer(client, rawRequest.c_str());
		cout << response << endl;
		SignUpResponse signUpResponse;
		signUpResponse.deserialize(response);
		if (signUpResponse.code == CODE_ERROR) {
			cout << "--- " + signUpResponse.message << endl;
		}
		else if (signUpResponse.code == CODE_SUCCESS)
		{
			cout << "--- You are register sucess" << endl;
		}
	}
}

/*
* Show login UI
* param:
*  client[IN] - socket param to pass to startComunicatingWithServer function
* return feature code
*/
void showLoginFeature(SOCKET& client)
{
	if (token > 0)
	{
		cout << "--- You are logged in" << endl;
	}
	else {
		string username = getUserInput("Enter your username: ");
		string password = getUserInput("Enter your password: ");

		LogInRequest request(username, password);
		string rawRequest = request.serialize();
		string response = startComunicatingWithServer(client, rawRequest.c_str());
		LogInResponse logInResponse;
		logInResponse.deserialize(response);
		token = logInResponse.token;
		if (logInResponse.code == CODE_ERROR) {
			cout << "--- Incorrect account or password" << endl;
		}
		else if (logInResponse.code == CODE_SUCCESS)
		{
			cout << "--- Successful login" << endl;
		}
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
	if (token < 0) {
		cout << "--- You are not logged in" << endl;
	}
	else
	{
		cout << "Show events" << endl;
		/*ListEventRequest listEventRequest(0, token);
		string rawRequest = listEventRequest.serialize();
		cout << rawRequest << endl;
		string response = startComunicatingWithServer(client, rawRequest.c_str());
		cout << response << endl;
		ListEventResponse listEventResponse;
		listEventResponse.deserialize(response);
		list<Event*> events = listEventResponse.events;
		for (Event* event : events)
			cout << to_string(event->id) +"-"+ event->name << endl;*/
		showListEventMenu(client);
	}
	return 3;
}

int showListEventMenu(SOCKET& client) {
	cout << "Select a option to show events:" << endl;
	cout << "1. My events" << endl;
	cout << "2. Member events" << endl;
	cout << "3. Other events" << endl;
	cout << "4. Back" << endl;
	string option;
	getline(cin, option);
	switch (atoi(option.c_str()))
	{
	case 1:
		showMyEvents(client);
		return 1;
		break;
	case 2:
		showMemberEvent(client);
		return 2;
		break;
	case 3:
		showOtherEvents(client);
		return 3;
		break;
	case 4:
		showFeaturesMenu(client);
		break;
	default:
		cout << "\nThis option is not available." << endl;
		return -1;
	}
}

void showMemberEvent(SOCKET& client) {
	cout << "List my events" << endl;
	ListEventRequest listEventRequest(JOINED_EVENTS, token);
	string rawRequest = listEventRequest.serialize();
	cout << rawRequest << endl;
	string response = startComunicatingWithServer(client, rawRequest.c_str());
	cout << response << endl;
	ListEventResponse listEventResponse;
	listEventResponse.deserialize(response);
	list<Event*> events = listEventResponse.events;
	int index = 1;
	for (Event* event : events) {
		cout << to_string(index) + "-" + event->name << endl;
		index++;
	}
	cout << "select event index to view event details" << endl;
	cout << "0. Back" << endl;
	string option;
	getline(cin, option);
	int choose = atoi(option.c_str());
	if (choose == 0) {
		showListEventMenu(client);
	}
	else
	{
		list<Event*>::iterator it = events.begin();
		advance(it, choose);
		showDetailEventById(client, (*it)->id, JOINED_EVENTS);
	}
}

void showMyEvents(SOCKET& client) {
	cout << "List my events" << endl;
	ListEventRequest listEventRequest(MY_EVENTS, token);
	string rawRequest = listEventRequest.serialize();
	cout << rawRequest << endl;
	string response = startComunicatingWithServer(client, rawRequest.c_str());
	cout << response << endl;
	ListEventResponse listEventResponse;
	listEventResponse.deserialize(response);
	list<Event*> events = listEventResponse.events;
	
	int index = 1;
	for (Event* event : events) {
		cout << to_string(index) + "-" + event->name << endl;
		index++;
	}
	cout << "select event index to view event details" << endl;
	cout << "0. Back" << endl;
	string option;
	getline(cin, option);
	int choose = atoi(option.c_str());
	if (choose == 0) {
		showListEventMenu(client);
	}
	else
	{
		int eventId;
		list<Event*>::iterator it = events.begin();
		advance(it, choose-1);
		eventId = (*it)->id;
		showDetailEventById(client, eventId, MY_EVENTS);
	}
}

void showListUserInvite(SOCKET& client, int choose) {
	cout << "List user not joined yet" << endl;
	FreeUsersRequest freeUsersRequest(choose, token);
	string rawRequest = freeUsersRequest.serialize();
	string response = startComunicatingWithServer(client, rawRequest.c_str());
	FreeUsersResponse freeUsersResponse;
	freeUsersResponse.deserialize(response);
	if (freeUsersResponse.code == CODE_ERROR) {
		cout << freeUsersResponse.message << endl;
		showDetailEventById(client, choose, MY_EVENTS);
	}
	else if (freeUsersResponse.code == CODE_SUCCESS)
	{
		cout << freeUsersResponse.message << endl;
		cout << "choose list index of the user you want to invite separated by commas" << endl;
		list<User*> users = freeUsersResponse.users;
		int index = 1;
		for (User* user : users){
			cout << to_string(index) + "-" + user->name << endl;
			index++;
		}
		list<User*>::iterator it = users.begin();

		// get to user
		string op;
		getline(cin, op);
		list<string> ids = tokenize(op, "," );

		// create list of user to save
		list<User*> usersRequest;
		for (string id : ids) {
			advance(it, atoi(id.c_str()) - 1);
			usersRequest.emplace_back(*it);
			it = users.begin();
		}
		CreateInviteRequest createInviteRequest(choose, usersRequest ,token);
		string rawRequest = createInviteRequest.serialize();
		string response = startComunicatingWithServer(client, rawRequest.c_str());
		CreateInviteResponse createInviteResponse;
		createInviteResponse.deserialize(response);
		if (createInviteResponse.code == CODE_ERROR) {
			cout << freeUsersResponse.message << endl;
			showListUserInvite(client, choose);
		}
		else if (freeUsersResponse.code == CODE_SUCCESS) {
			cout << freeUsersResponse.message << endl;
			showListEventMenu(client);
		}
	}
}

void showOtherEvents(SOCKET& client) {
	cout << "List my events" << endl;
	ListEventRequest listEventRequest(OTHER_EVENTS, token);
	string rawRequest = listEventRequest.serialize();
	cout << rawRequest << endl;
	string response = startComunicatingWithServer(client, rawRequest.c_str());
	cout << response << endl;
	ListEventResponse listEventResponse;
	listEventResponse.deserialize(response);
	list<Event*> events = listEventResponse.events;
	int index = 1;
	for (Event* event : events) {
		cout << to_string(index) + "-" + event->name << endl;
		index++;
	}
	cout << "select event index to view event details" << endl;
	cout << "0. Back" << endl;
	string option;
	getline(cin, option);
	int choose = atoi(option.c_str());
	if (choose == 0) {
		showListEventMenu(client);
	}
	else
	{
		int eventId;
		list<Event*>::iterator it = events.begin();
		advance(it, choose-1);
		eventId = (*it)->id;
		showDetailEventById(client, eventId, OTHER_EVENTS);
	}
}

void showDetailEventById(SOCKET& client, int choose, int status) {
	cout << "Event " + choose << endl;
	DetailEventRequest detailEventRequest(choose, token);
	string rawRequest = detailEventRequest.serialize();
	cout << rawRequest << endl;
	string response = startComunicatingWithServer(client, rawRequest.c_str());
	cout << response << endl;
	DetailEventResponse detailEventResponse;
	detailEventResponse.deserialize(response);
	if (detailEventResponse.code == CODE_ERROR) {
		cout << detailEventResponse.message << endl;
		showListEventMenu(client);
	}
	else if (detailEventResponse.code == CODE_SUCCESS)
	{
		cout << detailEventResponse.message << endl;
		cout << "id: " << detailEventResponse.event->id << endl;
		cout << "name: " + detailEventResponse.event->name << endl;
		cout << "description: " + detailEventResponse.event->description << endl;
		cout << "time: " + detailEventResponse.event->time << endl;
		cout << "location: " + detailEventResponse.event->location << endl;
		cout << "owner: " << detailEventResponse.event->owner << endl;

		if (status == OTHER_EVENTS) {
			cout << "choose r to request join group" << endl;
			string opt;
			getline(cin, opt);
			if (opt == "r") {
				CreateAskRequest createAskRequest(detailEventResponse.event->id, detailEventResponse.event->owner, token);
				string rawRequest = createAskRequest.serialize();
				string response = startComunicatingWithServer(client, rawRequest.c_str());
				CreateAskResponse createAskResponse;
				createAskResponse.deserialize(response);
				if (createAskResponse.code == CODE_ERROR) {
					cout << createAskResponse.message << endl;
					showDetailEventById(client, detailEventResponse.event->id, 0);
				}
				else if (createAskResponse.code == CODE_SUCCESS)
				{
					cout << createAskResponse.message << endl;
					showListEventMenu(client);
				}
			}
		}
		else if (status == JOINED_EVENTS)
		{
			cout << "0. Back" << endl;
			string opt;
			getline(cin, opt);
			if (atoi(opt.c_str()) == 0) {
				showMemberEvent(client);
			}
		}
		else if (status == MY_EVENTS)
		{
			cout << "choose s to invite people join group" << endl;
			string opt;
			getline(cin, opt);
			if (opt == "s") {
				showListUserInvite(client, detailEventResponse.event->id);
			}
		}
	}
}

//void showListMyEvents(SOCKET& client) {
//	cout << "List my events" << endl;
//	ListEventRequest listEventRequest(MY_EVENTS, token);
//	string rawRequest = listEventRequest.serialize();
//	cout << rawRequest << endl;
//	string response = startComunicatingWithServer(client, rawRequest.c_str());
//	cout << response << endl;
//	ListEventResponse listEventResponse;
//	listEventResponse.deserialize(response);
//	list<Event*> events = listEventResponse.events;
//	int index = 1;
//	for (Event* event : events) {
//		cout << to_string(index) + "-" + event->name << endl;
//		index++;
//	}
//	cout << "select event index to show request to other user" << endl;
//	cout << "0. Back" << endl;
//	string option;
//	getline(cin, option);
//	int choose = atoi(option.c_str());
//	if (choose == 0) {
//		showListEventMenu(client);
//	}
//	else
//	{
//		list<Event*>::iterator it = events.begin();
//		advance(it, choose);
//		showListRequest(client, (*it)->id);
//	}
//}

void showListMyRequest(SOCKET& client, int userId) {
	cout << "List Request in User " << userId << endl;
	ListRequestRequest listRequestRequest(userId);
	string rawRequest = listRequestRequest.serialize();
	cout << rawRequest << endl;
	string response = startComunicatingWithServer(client, rawRequest.c_str());
	cout << response << endl;
	ListRequestResponse listRequestResponse;
	listRequestResponse.deserialize(response);
	if (listRequestResponse.code == CODE_ERROR) {
		cout << listRequestResponse.message << endl;
		showFeaturesMenu(client);
	}
	else if (listRequestResponse.code == CODE_SUCCESS)
	{
		cout << listRequestResponse.message << endl;
		listRequestResponse.requests;
		list<AppRequest*> appRequests;
		int index = 1;
		for (AppRequest* appRequest : appRequests) {
			cout << to_string(index) + "-" << appRequest->name << endl;
			index++;
		}
		cout << "select the request to handle by index" << endl;
		cout << "0. Back";
		string option;
		getline(cin, option);
		int requestId = atoi(option.c_str());
		if (requestId == 0) {
			showFeaturesMenu(client);
		}
		else {
			requestProcessingMenu(client, requestId, userId);
		}
	}
}

void requestProcessingMenu(SOCKET& client, int requestId, int userId) {
	cout << "Menu process requet for requestId: " << requestId << endl;
	cout << "select function" << endl;
	cout << "1. Yes" << endl;
	cout << "2. No" << endl;
	cout << "3. Back" << endl;
	string option;
	getline(cin, option);
	int functionId = atoi(option.c_str());
	if (functionId == 3) {
		showListMyRequest(client, userId);
	}
	else if (functionId == 1) {
		updateStatusRequest(client, requestId, 1, token);
	}
	else if (functionId == 2) {
		updateStatusRequest(client, requestId, 0, token);
	}
	else {
		cout << "input is incorrect. Requires re-entry" << endl;
		requestProcessingMenu(client, requestId, userId);
	}
}

void updateStatusRequest(SOCKET& client,int requestId, int status, int userId) {
	UpdateRequest updateRequest(requestId, status, token);
	string rawRequest = updateRequest.serialize();
	cout << rawRequest << endl;
	string response = startComunicatingWithServer(client, rawRequest.c_str());
	cout << response << endl;
	UpdateResponse updateResponse;
	updateResponse.deserialize(response);
	if (updateResponse.code == CODE_ERROR) {
		cout << updateResponse.message << endl;
		requestProcessingMenu(client, requestId, userId);
	}
	else if (updateResponse.code == CODE_SUCCESS) {
		cout << updateResponse.message << endl;
		showListMyRequest(client, userId);
	}
}

void createEventFeature(SOCKET& client) {
	if (token < 0) {
		cout << "You are not logged in" << endl;
	}
	else
	{
		string name = getUserInput("Enter your name of event: ");
		string description = getUserInput("Enter your description of event: ");
		string time = getUserInput("Enter your time of event: ");
		string location = getUserInput("Enter your location of event: ");
		Event event(name, description, time, location);
		CreateEventRequest createEventRequest(&event, token);
		string rawRequest = createEventRequest.serialize();
		cout << rawRequest << endl;
		string response = startComunicatingWithServer(client, rawRequest.c_str());
		cout << response << endl;
		CreateEventResponse createEventResponse;
		createEventResponse.deserialize(response);

		if (createEventResponse.code == CODE_ERROR) {
			cout << createEventResponse.message << endl;
			cout << "Create failure event" << endl;
			createEventFeature(client);
		}
		else if (createEventResponse.code == CODE_SUCCESS)
		{
			cout << createEventResponse.message << endl;
			cout << "You create event sucess" << endl;
			showFeaturesMenu(client);
		}
	}
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
	cout << "4. List Request Join Event" << endl;
	cout << "5. Create Event" << endl;
	cout << "6. Logout" << endl;
	string option;
	getline(cin, option);
	switch (atoi(option.c_str()))
	{
	case 1:
		showRegisterFeature(client);
		return 1;
		break;
	case 2:
		showLoginFeature(client);
		return 2;
		break;
	case 3:
		return showListEvent(client);
		break;
	case 4:
		showListMyRequest(client, token);
		return 4;
		break;
	case 5:
		createEventFeature(client);
		return 5;
		break;
	case 6:
		return 6;
		break;
	default:
		cout << "\nThis option is not available." << endl;
		return -1;
	}
}