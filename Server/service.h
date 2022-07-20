#ifndef SERVICE_H_
#define SERVICE_H_
#define FMT_HEADER_ONLY

#include "model.h"
#include "constant.h"
#include <sstream>
#include <list>
#include <string>
#include "fmt\core.h"
#include "mysql\mysql.h"

using namespace std;
// Defining Constant Variables
#define SERVER "mysql.hoanandroid.app"
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

    inline bool createMembership(int eventId, int userId) {
        string query = fmt::format(QUERY_CREATE_MEMBERSHIP, eventId, userId);
        executeQuery(query);
        return TRUE;
    }

    string joinToString(list<string> tokens, string del = " ")
    {
        ostringstream stream;
        bool applyDel = false;
        for (string token : tokens)
        {
            stream << (applyDel ? del : "") << token;
            applyDel = true;
        }
        return stream.str();
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
            result.emplace_back(new Event(id, name, description, time, location, owner));
        }
        mysql_free_result(res_set);
        return result;
    }

    Event* getEventDetail(DetailEventRequest &request) {
        string query = fmt::format(QUERY_GET_EVENT_BY_ID, request.eventId);
        MYSQL_RES* res_set = executeQuery(query);
        MYSQL_ROW row = mysql_fetch_row(res_set);
        if (row == NULL) return NULL;
        int id = atoi(row[0]);
        string name = row[1];
        string description = row[2];
        string time = row[3];
        string location = row[4];
        int owner = atoi(row[5]);
        mysql_free_result(res_set);
        return new Event(id, name, description, time, location, owner);
    }

    string createEvent(CreateEventRequest& request) {
        Event* event = request.event;
        string addEventQuery = fmt::format(QUERY_CREATE_EVENT, event->name, event->description, event->time, event->location, event->owner);
        executeQuery(addEventQuery);
        int eventId = mysql_insert_id(connect);
        createMembership(eventId, event->owner);
        return MESSAGE_SUCCESS;
    }

    list<User*> getUsersNotJoinEvent(FreeUsersRequest &request) {
         string query = fmt::format(QUERY_USERS_NOT_JOIN_EVENT, request.eventId);
         MYSQL_RES* res_set = executeQuery(query);
         MYSQL_ROW row;

         list<User*> result;
         while (row = mysql_fetch_row(res_set)) {
             int id = atoi(row[0]);
             string name = row[1];
             string credential = row[2];
             result.emplace_back(new User(id, name, credential));
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
            result.emplace_back(new AppRequest(id, type, event, targetUser));
        }
        mysql_free_result(res_set);
        return result;
    }
    
    string createAskRequest(CreateAskRequest& request) {
        string query = fmt::format(QUERY_CREATE_ASK_REQUEST, request.eventId, request.eventOwner, request.token);
        executeQuery(query);
        return MESSAGE_SUCCESS;
    }
    
    string createInviteRequest(CreateInviteRequest& request) {
        list<string> insertRows;
        for (User* user : request.users) {
            string row = fmt::format(INSERT_INVITE_REQUEST_PATTERN, request.eventId, user->id, request.token);
            insertRows.emplace_back(row);
        }
        string query = fmt::format(QUERY_CREATE_INVITE_REQUEST, joinToString(insertRows, ",\n"));
        executeQuery(query);
        return MESSAGE_SUCCESS;
    }

    string updateRequest(UpdateRequest& request) {
        string updateQuery = fmt::format(QUERY_UPDATE_REQUEST, request.status, request.requestId);
        MYSQL_RES* res_set = executeQuery(updateQuery);
        if (request.status == REQUEST_STATUS_ACCEPT) {
            string findQuery = fmt::format(QUERY_GET_REQUEST_BY_ID, request.requestId);
            MYSQL_RES* res_set = executeQuery(findQuery);
            MYSQL_ROW row = mysql_fetch_row(res_set);
            int eventId = atoi(row[1]);
            int requestOwner = atoi(row[2]);
            int target = atoi(row[3]);
            int requestType = atoi(row[4]);
            mysql_free_result(res_set);
            if (requestType == REQUEST_TYPE_ASK) {
                createMembership(eventId, target);
            }
            else {
                createMembership(eventId, requestOwner);
            }
        }
        return MESSAGE_SUCCESS;
    }
};

#endif