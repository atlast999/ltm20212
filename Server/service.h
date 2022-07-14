#ifndef SERVICE_H_
#define SERVICE_H_

#include "model.h"
#include<vector>
#include<string>

using namespace std;

class AppService {
private:
    //db connection instance
public:
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