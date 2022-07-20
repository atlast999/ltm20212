// Server.cpp : Defines the entry point for the console application.
//
#include "stdafx.h"
#include "constant.h"
#include <iostream>
#include <string>



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

	UpdateRequest uRequest(4, 1, 2);
	string res10 = controller.handleRequest(uRequest.serialize());
	UpdateResponse uResponse;
	uResponse.deserialize(res10);
	cout << res10 << endl;
	cout << (uResponse.code == CODE_SUCCESS) << endl;

	system("pause");
	return 0;
}
