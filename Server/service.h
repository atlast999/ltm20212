#ifndef SERVICE_H_
#define SERVICE_H_
#define FMT_HEADER_ONLY

#include "model.h"
#include "constant.h"
#include<vector>
#include<string>
#include "fmt\core.h"
#include "mysql\mysql.h"

using namespace std;
// Defining Constant Variables
#define SERVER "35.77.223.104"
#define USER "ltm20212" 
#define PASSWORD "ltm20212"
#define DATABASE "ltm20212" 
#define PORT 3306

class AppService {
private:
    MYSQL *connect;
	//always call mysql_free_result(res_set) after being done with res_set
    inline MYSQL_RES * executeQuery(string query) {
        mysql_query(connect, query.c_str());
        return mysql_use_result(connect);
    }

    inline User * findUserByName(string username) {
        string query = fmt::format(QUERY_FIND_USER_BY_NAME, username);
		MYSQL_RES *res_set = executeQuery(query);
        MYSQL_ROW row = mysql_fetch_row(res_set);
        if (row == NULL) return NULL;
		int id = atoi(row[0]);
        string name = row[1];
        string credential = row[2];
        mysql_free_result(res_set);
		return new User(id, name, credential);
    }

    inline User * findUserById(int userId) {
        string query = fmt::format(QUERY_FIND_USER_BY_ID, userId);
		MYSQL_RES *res_set = executeQuery(query);
		mysql_free_result(res_set);
        MYSQL_ROW row = mysql_fetch_row(res_set);
        if (row == NULL) return NULL;
		int id = atoi(row[0]);
        string name = row[1];
        string credential = row[2];
        mysql_free_result(res_set);
		return new User(id, name, credential);
    }
public:
    AppService() {
        connect = mysql_init(NULL);
        connect = mysql_real_connect(connect, SERVER, USER, PASSWORD, DATABASE, PORT, NULL, 0);
        // if (connect) // check if the connection was successful,
        // {
        //     cout << "Connection Succeeded\n";
        // }
        // else
        // {
        //     cout << "Connection Failed\n";
        //     return 0;
        // }
    }
    ~AppService() {
	    mysql_close(connect);
    }
    string createNewUser(SignUpRequest &request) {
		User * user = findUserByName(request.name);
        if (user != NULL) {
            return MESSAGE_NAME_INVALID;
        }
        string addQuery = fmt::format(QUERY_CREATE_USER, request.name, request.credential);
        executeQuery(addQuery);
        return MESSAGE_SUCCESS;
    }

    string verifyUser(LogInRequest &request, int& token) {
        User * user = findUserByName(request.name);
        if (user == NULL) {
            return MESSAGE_NAME_INVALID;
        }
        if (user->credential != request.credential) {
            return MESSAGE_CREDENTIAL_INVALID;
        }
        token = user->id;
        return MESSAGE_SUCCESS;
    }

    list<Event*> getEvents(ListEventRequest &request) {
        string query;
        if (request.isMine) {
            query = fmt::format(QUERY_LIST_EVENT_MINE, request.token);
        } else {
            query = fmt::format(QUERY_LIST_EVENT);
        }
        MYSQL_RES *res_set = executeQuery(query);
        MYSQL_ROW row;

        list<Event*> result;
        while(row = mysql_fetch_row(res_set)) {
            int id = atoi(row[0]);
            string name = row[1];
            string description = row[2];
            string time = row[3];
            string location = row[4];
            int owner = atoi(row[5]);
            result.push_back(new Event(id, name, description, time, location, owner));
        }
        mysql_free_result(res_set);
        return result;
    }

    list<AppRequest*> getRequests(ListRequestRequest& request) {
        string query = fmt::format(QUERY_LIST_REQUEST, request.token);
        MYSQL_RES *res_set = executeQuery(query);
        MYSQL_ROW row;

        list<AppRequest*> result;
        while(row = mysql_fetch_row(res_set)) {
            int id = atoi(row[0]);
            int type = atoi(row[1]);
            string event = row[2];
            string targetUser = row[3];
            result.push_back(new AppRequest(id, type, event, targetUser));
        }
        mysql_free_result(res_set);
        return result;
    }
    

    // list<User*> getUsersNotJoiningEvent(int eventId) {
    //     list<User*> result;

    //     //query from db to build list, below is just faking

    //     for (int i = 1; i <= 3; i++) {
    //         User *user = new User(i, "User " + to_string(i));
    //         result.push_back(user);
    //     }
    //     return result;
    // }
};

#endif