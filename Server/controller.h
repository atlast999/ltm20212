#pragma once
#ifndef CONTROLLER_H_
#define CONTROLLER_H_

#include<vector>
#include<string>

#include "constant.h"
#include "model.h"
#include "service.h"

class AppController {
private:
	AppService service;

    /**
     * Create a new user
     * Params:
     *  rawRequest [IN]: the string represents SignUpRequest
     * return the string represents response data
     */
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

    /**
     * Verify a user trying to log in
     * Params:
     *  rawRequest [IN]: the string represents LogInRequest
     * return the string represents response data
     */
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

    /**
     * Get a list of events
     * Params:
     *  rawRequest [IN]: the string represents ListEventRequest
     * return the string represents response data
     */
	string getEvents(string rawRequest) {
		ListEventRequest request;
		request.deserialize(rawRequest);
		list<Event*> events = service.getEvents(request);
		return ListEventResponse(CODE_SUCCESS, MESSAGE_SUCCESS, events).serialize();
	}

    /**
     * Get the detail of a event
     * Params:
     *  rawRequest [IN]: the string represents DetailEventRequest
     * return the string represents response data
     */
	string getEventDetail(string rawRequest) {
		DetailEventRequest request;
		request.deserialize(rawRequest);
		Event* event = service.getEventDetail(request);
		if (event == NULL) {
			return BaseResponse(CODE_ERROR, MESSAGE_EVENT_NOT_EXIST).serialize();
		}
		return DetailEventResponse(CODE_SUCCESS, MESSAGE_SUCCESS, event).serialize();
	}

    /**
     * Create a new event
     * Params:
     *  rawRequest [IN]: the string represents CreateEventRequest
     * return the string represents response data
     */
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

    /**
     * Get a list of users who were not members of a specific event
     * Params:
     *  rawRequest [IN]: the string represents FreeUsersRequest
     * return the string represents response data
     */
	string getUsersNotJoinEvent(string rawRequest) {
		FreeUsersRequest request;
		request.deserialize(rawRequest);
		list<User*> users = service.getUsersNotJoinEvent(request);
		return FreeUsersResponse(CODE_SUCCESS, MESSAGE_SUCCESS, users).serialize();
	}

    /**
     * Get a list of requests belonging to current user
     * Params:
     *  rawRequest [IN]: the string represents ListRequestRequest
     * return the string represents response data
     */
	string getRequests(string rawRequest) {
		ListRequestRequest request;
		request.deserialize(rawRequest);
		list<AppRequest*> requests = service.getRequests(request);
		return ListRequestResponse(CODE_SUCCESS, MESSAGE_SUCCESS, requests).serialize();
	}

    /**
     * Create a ask-to-join request
     * Params:
     *  rawRequest [IN]: the string represents CreateAskRequest
     * return the string represents response data
     */
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

    /**
     * Create a invite-to-join request
     * Params:
     *  rawRequest [IN]: the string represents CreateInviteRequest
     * return the string represents response data
     */
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

    /**
     * Accept or reject a request
     * Params:
     *  rawRequest [IN]: the string represents UpdateRequest
     * return the string represents response data
     */
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
    /**
     * Identify the request' operation then call the coresponding function
     * Params:
     *  rawRequest [IN]: the string represents client request
     * return the string represents response data
     */
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