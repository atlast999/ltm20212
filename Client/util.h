#pragma once
#include <iostream>
#include <stdio.h>
#include <string>
#include <vector>
#include <sstream>

#include <winsock2.h>
#include <ws2tcpip.h>

// account status
#define ACC_ACTIVE "1"
#define ACC_BLOCKED "0"

// client request type
#define CMD_IN "USER"
#define CMD_OUT "BYE"
#define CMD_POST "POST"

// session status
#define AUTH_GUESS 0
#define AUTH_MEMBER 1

// response code
#define AUTH_IN_SUCCESS 10
#define AUTH_BLOCLED 11
#define AUTH_NOT_EXIST 12
#define AUTH_IN_OTHER_SESSION 13
#define AUTH_ALREADY_IN 14
#define POST_SUCCESS 20
#define AUTH_NOT_LOGIN 21
#define AUTH_OUT_SUCCESS 30
#define INVALID_CMD 99

using namespace std;

/*
* Split a string into list of strings separated by param del
* param:
*  s[IN] - the original string
*  del[IN] - the delimiter
* return a vector of separated strings
*/
list<string> tokenize(string s, string del = " ")
{
	list<string> tokens;
	int start = 0;
	int end = s.find(del);
	while (end != -1)
	{
		tokens.push_back(s.substr(start, end - start));
		start = end + del.size();
		end = s.find(del, start);
	}
	tokens.push_back(s.substr(start, end - start));
	return tokens;
}

/*
* Join a list of strings into a single string connected by param del
* param:
*  tokens[IN] - the original list of strings
*  del[IN] - the delimiter
* return a single connected string
*/
string joinToString(vector<string> tokens, string del = " ")
{
	ostringstream stream;
	for (int i = 0; i < tokens.size(); i++)
	{
		if (i)
			stream << del;
		stream << tokens.at(i);
	}
	return stream.str();
}

/*
* Print the message according to response code
* param:
*  code[IN] - the response code
*/
void handleResponseCode(int code) {
	cout << endl;
	switch (code)
	{
	case AUTH_IN_SUCCESS:
		cout << "Login successfully" << endl;
		break;
	case AUTH_BLOCLED:
		cout << "Your account has been blocked" << endl;
		break;
	case AUTH_NOT_EXIST:
		cout << "Your account is not exist" << endl;
		break;
	case AUTH_IN_OTHER_SESSION:
		cout << "You've loged in other session" << endl;
		break;
	case AUTH_ALREADY_IN:
		cout << "You've already loged in. Try log out instead to use other account" << endl;
		break;
	case POST_SUCCESS:
		cout << "Post article successfully" << endl;
		break;
	case AUTH_NOT_LOGIN:
		cout << "You have not been loged in yet" << endl;
		break;
	case AUTH_OUT_SUCCESS:
		cout << "Logout successfully" << endl;
		break;
	case INVALID_CMD:
		cout << "Invalid request" << endl;
		break;
	default:
		cout << "Something went wrong, please try again" << endl;
		break;
	}
}
