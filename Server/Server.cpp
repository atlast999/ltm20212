// Server.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "controller.h"
#include "model.h"
#include<iostream>
using namespace std;


int main(int argc, char const *argv[])
{
	AppController controller;
	string raw = "{\"name\":\"hoang\",\"credential\":\"password\",\"token\":1,\"operation\":0}";
	string res = controller.handleRequest(raw);
	cout << res << endl;
	return 0;
}
