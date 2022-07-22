#pragma once
#ifndef CONTROLLER_H_
#define CONTROLLER_H_

#include "constant.h"
#include "model.h"
#include "service.h"
#include<vector>
#include<string>

class AppController {
private:
	AppService service;

	string createNewUser(string rawRequest) {
		SignUpRequest request;
		request.deserialize(rawRequest);
		string message = service.createNewUser(request);
		SignUpResponse response(CODE_SUCCESS, message.c_str());
		if (message == MESSAGE_SUCCESS) {
			return response.serialize();
		}
		response.code = CODE_ERROR;
		return response.serialize();
	}

	string verifyUser(string rawRequest) {
		LogInRequest request;
		request.deserialize(rawRequest);
		int token = -1;
		string message = service.verifyUser(request, token);
		LogInResponse response(CODE_SUCCESS, message.c_str(), token);
		if (message == MESSAGE_SUCCESS) {
			return response.serialize();
		}
		response.code = CODE_ERROR;
		return response.serialize();
	}

	string getEvents(string rawRequest) {
		ListEventRequest request;
		request.deserialize(rawRequest);
		list<Event*> events = service.getEvents(request);
		return ListEventResponse(CODE_SUCCESS, MESSAGE_SUCCESS, events).serialize();
	}

	string getEventDetail(string rawRequest) {
		DetailEventRequest request;
		request.deserialize(rawRequest);
		Event* event = service.getEventDetail(request);
		if (event == NULL) {
			return BaseResponse(CODE_ERROR, MESSAGE_EVENT_NOT_EXIST).serialize();
		}
		return DetailEventResponse(CODE_SUCCESS, MESSAGE_SUCCESS, event).serialize();
	}

	string createEvent(string rawRequest) {
		CreateEventRequest request;
		request.deserialize(rawRequest);
		string message = service.createEvent(request);
		CreateEventResponse response(CODE_SUCCESS, message.c_str());
		if (message == MESSAGE_SUCCESS) {
			return response.serialize();
		}
		response.code = CODE_ERROR;
		return response.serialize();
	}

	string getUsersNotJoinEvent(string rawRequest) {
		FreeUsersRequest request;
		request.deserialize(rawRequest);
		list<User*> users = service.getUsersNotJoinEvent(request);
		return FreeUsersResponse(CODE_SUCCESS, MESSAGE_SUCCESS, users).serialize();
	}

	string getRequests(string rawRequest) {
		ListRequestRequest request;
		request.deserialize(rawRequest);
		list<AppRequest*> requests = service.getRequests(request);
		return ListRequestResponse(CODE_SUCCESS, MESSAGE_SUCCESS, requests).serialize();
	}

	string createAskRequest(string rawRequest) {
		CreateAskRequest request;
		request.deserialize(rawRequest);
		string message = service.createAskRequest(request);
		CreateAskResponse response(CODE_SUCCESS, message.c_str());
		if (message == MESSAGE_SUCCESS) {
			return response.serialize();
		}
		response.code = CODE_ERROR;
		return response.serialize();
	}

	string createInviteRequest(string rawRequest) {
		CreateInviteRequest request;
		request.deserialize(rawRequest);
		string message = service.createInviteRequest(request);
		CreateInviteResponse response(CODE_SUCCESS, message.c_str());
		if (message == MESSAGE_SUCCESS) {
			return response.serialize();
		}
		response.code = CODE_ERROR;
		return response.serialize();
	}

	string updateRequest(string rawRequest) {
		UpdateRequest request;
		request.deserialize(rawRequest);
		string message = service.updateRequest(request);
		UpdateResponse response(CODE_SUCCESS, message.c_str());
		if (message == MESSAGE_SUCCESS) {
			return response.serialize();
		}
		response.code = CODE_ERROR;
		return response.serialize();
	}

public:
	string handleRequest(string rawRequest) {
		BaseRequest request;
		request.deserialize(rawRequest);
		if (!request.isAuthenticated()) {
			return BaseResponse(CODE_ERROR, MESSAGE_UNAUTHENTICATED).serialize();
		}
		switch (request.operation)
		{
		case OP_SIGN_UP:
			return createNewUser(rawRequest);
			break;
		case OP_LOG_IN:
			return verifyUser(rawRequest);
			break;
		case OP_LIST_EVENT:
			return getEvents(rawRequest);
			break;
		case OP_DETAIL_EVENT:
			return getEventDetail(rawRequest);
			break;
		case OP_CREATE_EVENT:
			return createEvent(rawRequest);
			break;
		case OP_LIST_REQUEST:
			return getRequests(rawRequest);
			break;
		case OP_USERS_NOT_JOINING_EVENT:
			return getUsersNotJoinEvent(rawRequest);
			break;
		case OP_CREATE_ASK_REQUEST:
			return createAskRequest(rawRequest);
			break;
		case OP_CREATE_INVITE_REQUEST:
			return createInviteRequest(rawRequest);
			break;
		case OP_UPDATE_REQUEST:
			return updateRequest(rawRequest);
			break;
		default:
			return BaseResponse(CODE_ERROR, MESSAGE_INVALID_OPERATION).serialize();
			break;
		}
	}
};


#endif