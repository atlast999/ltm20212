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

// declare "token" variable as global
int token = -1;

// list functions
void showMyEvents(SOCKET& client);
void showOtherEvents(SOCKET& client);
void showDetailEventById(SOCKET& client, int eventId, int status);
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
* @param: instance[OUT] - pointer to the socket created
* @return true if success, false otherwise
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
		cout << WSAGetLastError() << endl;
		return 1;
	}
	cout << endl
		<< "\t\tWellcome to Event management application. " << endl;

	// When the connection is established show user interface
	while (showFeaturesMenu(client) != 6);
	WSACleanup();

	return 0;
}

/*
* @function: Send request to server and receive response
* @param client[IN] - TCP socket used to comunicate with server
* @param buffer - request content to send to server
* @return char* response recv to server
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
* @function getUserInput: Get input from user
* 
* @param message[IN] - UI hint message
* 
* @return input content
*/
string getUserInput(const char* message)
{
	cout << message;
	string input;
	getline(cin, input);
	return input;
}

/*
* @function showRegisterFeature: Show register UI
* 
* @param client[IN] - socket param to pass to startComunicatingWithServer function
* 
* @return No return value
*/
void showRegisterFeature(SOCKET& client)
{
	if (token > 0) {
		cout << "You logged in" << endl;
		cout << "-------------------------------" << endl;
	}
	else
	{
		string username = getUserInput("Enter your username: ");
		string password = getUserInput("Enter your password: ");

		SignUpRequest request(username, password);
		string rawRequest = request.serialize();
		string response = startComunicatingWithServer(client, rawRequest.c_str());
		SignUpResponse signUpResponse;
		signUpResponse.deserialize(response);
		if (signUpResponse.code == CODE_ERROR) {
			cout << signUpResponse.message << endl;
			cout << "-------------------------------" << endl;
 		}
		else if (signUpResponse.code == CODE_SUCCESS)
		{
			cout << "You registered sucessfully" << endl;
			cout << "------------------------------" << endl;
		}
	}
}

/*
* @function showLoginFeature: Show login UI
* 
* @param client[IN] - socket param to pass to startComunicatingWithServer function
* 
* @return feature code
*/
void showLoginFeature(SOCKET& client)
{
	if (token > 0)
	{
		cout << "You are logged in" << endl;
		cout << "-------------------------------" << endl;
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
			cout << "Incorrect account or password" << endl;
			cout << "-------------------------------" << endl;
		}
		else if (logInResponse.code == CODE_SUCCESS)
		{
			cout << "Successful login" << endl;
			cout << "-------------------------------" << endl;
		}
	}
}

/*
* @function showListEvent: Check login status 
*	If the user is logged in, 
*	a menu will be displayed for the user to choose the type of event they want to display
*	else back original menu
*	
* @param client[IN] - socket param to pass to startComunicatingWithServer function
* 
* @return 3
*/
int showListEvent(SOCKET& client)
{
	if (token < 0) {
		cout << "You are not logged in" << endl;
	}
	else
	{
		cout << "Show events" << endl;
		showListEventMenu(client);
	}
	return 3;
}

/**
* @function showListEventMenu: Show UI to user select the type of event you want to display
*
* @param: client[IN] - socket param to pass to startComunicatingWithServer function
*
* @return: type of event 
**/
int showListEventMenu(SOCKET& client) {
	cout << "Select a option to show events:" << endl;
	cout << "1. My events" << endl;
	cout << "2. Joined events" << endl;
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

/**
* @function showMemberEvent: show list joined event 
*
* @param client[IN] - socket param to pass to startComunicatingWithServer function
*
* @return: No return
**/
void showMemberEvent(SOCKET& client) {
	cout << "List joined events" << endl;
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
		if ((index == 1) || (choose >= index)) {
			cout << "Your index is incorrect, please re-enter" << endl;
			showMemberEvent(client);
		}
		list<Event*>::iterator it = events.begin();
		advance(it, choose-1);
		showDetailEventById(client, (*it)->id, JOINED_EVENTS);
	}
}

/**
* @function showMyEvents: show list my events
*
* @param client[IN] - socket param to pass to startComunicatingWithServer function
*
* @return: No return
**/
void showMyEvents(SOCKET& client) {
	cout << "List my events" << endl;
	ListEventRequest listEventRequest(MY_EVENTS, token);
	string rawRequest = listEventRequest.serialize();
	string response = startComunicatingWithServer(client, rawRequest.c_str());
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
		if ((index == 1) || (choose >= index)) {
			cout << "Your index is incorrect, please re-enter" << endl;
			showMyEvents(client);
		}
		int eventId;
		list<Event*>::iterator it = events.begin();
		advance(it, choose-1);
		eventId = (*it)->id;
		showDetailEventById(client, eventId, MY_EVENTS);
	}
}

/**
* @function showListUserInvite: show list user have not joined event by "eventId" to 
*     select
*
* @param client[IN] - socket param to pass to startComunicatingWithServer function
* @param eventId - id of event
* @param status - status of user in events ( OTHER_EVENTS 0, JOINED_EVENTS 1, MY_EVENTS 2 )
*
* @return: No return
**/
void showListUserInvite(SOCKET& client, int eventId, int status) {
	cout << "List user not joined yet" << endl;
	FreeUsersRequest freeUsersRequest(eventId, token);
	string rawRequest = freeUsersRequest.serialize();
	string response = startComunicatingWithServer(client, rawRequest.c_str());
	FreeUsersResponse freeUsersResponse;
	freeUsersResponse.deserialize(response);
	if (freeUsersResponse.code == CODE_ERROR) {
		cout << freeUsersResponse.message << endl;
		showDetailEventById(client, eventId, MY_EVENTS);
	}
	else if (freeUsersResponse.code == CODE_SUCCESS)
	{
		// cout << freeUsersResponse.message << endl;
		cout << "choose list index of the user you want to invite separated by commas" << endl;
		cout << "0. Back" << endl;
		list<User*> users = freeUsersResponse.users;
		int index = 1;
		for (User* user : users) {
			cout << to_string(index) + "-" + user->name << endl;
			index++;
		}
		list<User*>::iterator it = users.begin();
		// get to user
		string op;
		getline(cin, op);
		if (op == "0") {
			showDetailEventById(client, eventId, status);
		}
		else if (atoi(op.c_str()) == 0) {
			cout << "you entered wrong. request re-entry" << endl;
			showListUserInvite(client, eventId, status);
		} else {
			list<string> ids = tokenize(op, ",");

			// create list of user to save
			list<User*> usersRequest;
			for (string id : ids) {
				advance(it, atoi(id.c_str()) - 1);
				usersRequest.emplace_back(*it);
				it = users.begin();
			}
			CreateInviteRequest createInviteRequest(eventId, usersRequest, token);
			string rawRequest = createInviteRequest.serialize();
			string response = startComunicatingWithServer(client, rawRequest.c_str());
			CreateInviteResponse createInviteResponse;
			createInviteResponse.deserialize(response);
			if (createInviteResponse.code == CODE_ERROR) {
				cout << freeUsersResponse.message << endl;
				showListUserInvite(client, eventId, status);
			}
			else if (freeUsersResponse.code == CODE_SUCCESS) {
				cout << freeUsersResponse.message << endl;
				showListEventMenu(client);
			}
		}
	}
}

/**
* @function showOtherEvents: show list your unparticipated events ( other evnet )
*
* @param client[IN] - socket param to pass to startComunicatingWithServer function
* 
* @return: No return
**/
void showOtherEvents(SOCKET& client) {
	cout << "List other events" << endl;
	ListEventRequest listEventRequest(OTHER_EVENTS, token);
	string rawRequest = listEventRequest.serialize();
	string response = startComunicatingWithServer(client, rawRequest.c_str());
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
		if ((index == 1) || (choose >= index)) {
			cout << "Your index is incorrect, please re-enter" << endl;
			showOtherEvents(client);
		}
		int eventId;
		list<Event*>::iterator it = events.begin();
		advance(it, choose-1);
		eventId = (*it)->id;
		showDetailEventById(client, eventId, OTHER_EVENTS);
	}
}

/**
* @function showDetailEventById: show detail event by eventId
*
* @param client[IN] - socket param to pass to startComunicatingWithServer function
* @param eventId[IN] - id of event
* @param status[IN] - type of event ( OTHER_EVENTS 0, JOINED_EVENTS 1,MY_EVENTS 2 )
*
* @return: No return
**/
void showDetailEventById(SOCKET& client, int eventId, int status) {
	cout << "Event " + eventId << endl;
	DetailEventRequest detailEventRequest(eventId, token);
	string rawRequest = detailEventRequest.serialize();
	string response = startComunicatingWithServer(client, rawRequest.c_str());
	DetailEventResponse detailEventResponse;
	detailEventResponse.deserialize(response);
	if (detailEventResponse.code == CODE_ERROR) {
		cout << detailEventResponse.message << endl;
		showListEventMenu(client);
	}
	else if (detailEventResponse.code == CODE_SUCCESS)
	{
		//cout << detailEventResponse.message << endl;
		cout << "---- Event Detail --- " << endl;
		//cout << "id: " << detailEventResponse.event->id << endl;
		cout << "name: " + detailEventResponse.event->name << endl;
		cout << "description: " + detailEventResponse.event->description << endl;
		cout << "time: " + detailEventResponse.event->time << endl;
		cout << "location: " + detailEventResponse.event->location << endl;
		cout << "owner: " << detailEventResponse.event->owner << endl;

		if (status == OTHER_EVENTS) {
			cout << "choose r to request join group" << endl;
			cout << "0. Back" << endl;
			string opt;
			getline(cin, opt);
			if (opt == "0") {
				showOtherEvents(client);
			}
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
			return;
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
			cout << "0. Back" << endl;
			string opt;
			getline(cin, opt);
			if (opt == "s") {
				showListUserInvite(client, detailEventResponse.event->id, status);
			}
			else if(opt == "0") {
				showMyEvents(client);
			}
		}
	}
}

/**
* @function showListMyRequest: show list request by id of user 
*
* @param client[IN] - socket param to pass to startComunicatingWithServer function
* @param userId[IN] - id of user 
*
* @return: No return
**/
void showListMyRequest(SOCKET& client, int userId) {
	cout << "List Request in User " << endl;
	ListRequestRequest listRequestRequest(userId);
	string rawRequest = listRequestRequest.serialize();
	string response = startComunicatingWithServer(client, rawRequest.c_str());
	ListRequestResponse listRequestResponse;
	listRequestResponse.deserialize(response);
	if (listRequestResponse.code == CODE_ERROR) {
		cout << listRequestResponse.message << endl;
		showFeaturesMenu(client);
	}
	else if (listRequestResponse.code == CODE_SUCCESS)
	{
		cout << listRequestResponse.message << endl;
		int index = 1;
		for (AppRequest* appRequest : listRequestResponse.requests) {
			cout << to_string(index) + "-" << appRequest->name << endl;
			index++;
		}
		cout << "select the request to handle by index" << endl;
		cout << "0. Back" << endl;
		string option;
		getline(cin, option);
		if (atoi(option.c_str()) == 0) {
			showFeaturesMenu(client);
			return;
		}

		if ((index == 1) || (atoi(option.c_str()) >= index)) {
			cout << "Your index is incorrect, please re-enter" << endl;
			showListMyRequest(client, userId);
		}

		list<AppRequest*>::iterator it = listRequestResponse.requests.begin();
		advance(it, atoi(option.c_str())-1);
		int requestId = (*it)->id;
		requestProcessingMenu(client, requestId, userId);
	}
}

/**
* @function requestProcessingMenu: display menu for user to confirm request ( yes, no )
*
* @param client[IN] - socket param to pass to startComunicatingWithServer function
* @param requestId[IN] - id of request
* @param userId[IN] - id of user
*
* @return: No return
**/
void requestProcessingMenu(SOCKET& client, int requestId, int userId) {
	cout << "Menu confirm request for requestId: " << requestId << endl;
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
		updateStatusRequest(client, requestId, REQUEST_STATUS_ACCEPT, token);
	}
	else if (functionId == 2) {
		updateStatusRequest(client, requestId, REQUEST_STATUS_REJECT, token);
	}
	else {
		cout << "input is incorrect. Requires re-entry" << endl;
		requestProcessingMenu(client, requestId, userId);
	}
}

/**
* @function updateStatusRequest: update status of request into the database
*
* @param client[IN] - socket param to pass to startComunicatingWithServer function
* @param requestId[IN] - id of request
* @param status[IN] - status of request ( REQUEST_STATUS_ACCEPT 1, REQUEST_STATUS_REJECT 2)
* @param userId[IN] - id of user
*
* @return: No return
**/
void updateStatusRequest(SOCKET& client,int requestId, int status, int userId) {
	UpdateRequest updateRequest(requestId, status, token);
	string rawRequest = updateRequest.serialize();
	string response = startComunicatingWithServer(client, rawRequest.c_str());
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

/**
* @function createEventFeature: create 1 event into database
* 
* @param client[IN] - socket param to pass to startComunicatingWithServer function
*
* @return: No return
**/
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
		string response = startComunicatingWithServer(client, rawRequest.c_str());
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

/*
* @function: Show feature menu UI along with feature code
* 
* @param: client[IN] - socket param to pass to feature function
* 
* return feature code
*/
int showFeaturesMenu(SOCKET& client)
{
	cout << "Select a feature below:" << endl;
	cout << "1. Register" << endl;
	cout << "2. Login" << endl;
	cout << "3. List Events" << endl;
	cout << "4. My Requests " << endl;
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