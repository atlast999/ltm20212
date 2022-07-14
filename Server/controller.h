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

    string getUsersNotJoiningEvent(string rawRequest) {
        UsersRequest request;
        request.deserialize(rawRequest);
        list<User*> users = service.getUsersNotJoiningEvent(request.eventId);
        UsersResponse response(CODE_SUCCESS, MESSAGE_SUCCESS, users);
        return response.serialize();
    }
public:
    string handleRequest(string rawRequest) {
        BaseRequest request;
        request.deserialize(rawRequest);
        if (!request.isAuthenticated()) {
            return BaseResponse(CODE_ERROR, MESSAGE_UNAUTHENTICATED).serialize();
        }
        switch (request.requestType)
        {
        case REQUEST_TYPE_USERS_NOT_JOINING:
            return getUsersNotJoiningEvent(rawRequest);
            break;
        default:
            return BaseResponse(CODE_ERROR, MESSAGE_INVALID_OPERATION).serialize();
            break;
        }
    }
};


#endif