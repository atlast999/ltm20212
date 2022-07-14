#ifndef SERVICE_H_
#define SERVICE_H_

#include "model.h"
#include "constant.h"
#include<vector>
#include<string>

using namespace std;

class AppService {
private:
    //db connection instance
public:

    string createNewUser(CreateUserRequest &request) {
        //check and insert into db
        if (request.name == "hoan") {
            return MESSAGE_NAME_INVALID;
        }
        return MESSAGE_SUCCESS;
    }

    list<User*> getUsersNotJoiningEvent(int eventId) {
        list<User*> result;

        //query from db to build list, below is just faking

        for (int i = 1; i <= 3; i++) {
            User *user = new User(i, "User " + to_string(i));
            result.push_back(user);
        }
        return result;
    }
};

#endif