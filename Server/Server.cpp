// Server.cpp : Defines the entry point for the console application.
//

//#include "stdafx.h"
#include "controller.h"
#include "model.h"
#include<iostream>
using namespace std;


int main(int argc, char const *argv[])
{
	AppController controller;
	string raw = "{\"eventId\":1,\"requestType\":1,\"token\":1}";
	string res = controller.handleRequest(raw);
	cout << res << endl;
	return 0;
}
