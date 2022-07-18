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

	ListEventRequest eventRequest(0, 2);
	string res3 = controller.handleRequest(eventRequest.serialize());
	ListEventResponse eventResponse;
	eventResponse.deserialize(res3);
	cout << res3 << endl;
	cout << (eventResponse.code == CODE_SUCCESS) << endl;

	ListRequestRequest appRequest(2);
	string res4 = controller.handleRequest(appRequest.serialize());
	ListRequestResponse appRequestResponse;
	appRequestResponse.deserialize(res4);
	cout << res4 << endl;
	cout << (appRequestResponse.code == CODE_SUCCESS) << endl;

	system("pause");
	return 0;
}
