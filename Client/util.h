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
* @function: Split a string into list of strings separated by param del
* 
* @param: s[IN] - the original string
* @param: del[IN] - the delimiter
* 
* @return: a list of separated strings
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