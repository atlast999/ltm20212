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
        CreateUserRequest request;
        request.deserialize(rawRequest);
        string message = service.createNewUser(request);
        BaseResponse response(CODE_SUCCESS, message.c_str());
        if (message == MESSAGE_SUCCESS) {
            return response.serialize();
        }
        response.code = CODE_ERROR;
        return response.serialize();
    }

    string getUsersNotJoiningEvent(string rawRequest) {
        InviteUsersRequest request;
        request.deserialize(rawRequest);
        list<User*> users = service.getUsersNotJoiningEvent(request.eventId);
        InviteUsersResponse response(CODE_SUCCESS, MESSAGE_SUCCESS, users);
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
        case OP_USERS_NOT_JOINING_EVENT:
            return getUsersNotJoiningEvent(rawRequest);
            break;
        default:
            return BaseResponse(CODE_ERROR, MESSAGE_INVALID_OPERATION).serialize();
            break;
        }
    }
};


#endif