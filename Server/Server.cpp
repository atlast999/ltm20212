// Server.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "controller.h"
#include "model.h"
#include<iostream>
using namespace std;


int main(int argc, char const *argv[])
{
	//client
	CreateUserRequest request;
	request.operation = OP_SIGN_UP;
	request.name = "name";
	request.credential = "pass";
	string rawRequest = request.serialize();

	AppController controller;
	string res = controller.handleRequest(rawRequest);
	BaseResponse response;
	response.deserialize(res);
	
	cout << response.code << endl;
	return 0;
}
