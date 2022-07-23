#pragma once
#ifndef SERVICE_H_
#define SERVICE_H_
#define FMT_HEADER_ONLY

#include <sstream>
#include <list>
#include <string>
#include "fmt\core.h"
#include "mysql\mysql.h"
#include "model.h"
#include "constant.h"

using namespace std;

#define SERVER "mysql.hoanandroid.app"
#define USER "ltm20212" 
#define PASSWORD "ltm20212"
#define DATABASE "ltm20212" 
#define PORT 3306

class AppService {
private:
    MYSQL* connect;
    /**
     * Send the query to database server
     * Params:
     *  query [IN]: the said query
     * return the pointer to result of query
     * Remember to call mysql_free_result(res_set) after being done with res_set
     */
    MYSQL_RES* executeQuery(string query) {
        mysql_query(connect, query.c_str());
        return mysql_use_result(connect);
    }

    /**
     * Find a user by their name
     * Params:
     *  username [IN]: the said name
     * return the pointer to found user
     */
    User* findUserByName(string username) {
        string query = fmt::format(QUERY_FIND_USER_BY_NAME, username);
        MYSQL_RES* res_set = executeQuery(query);
        MYSQL_ROW row = mysql_fetch_row(res_set);
        if (row == NULL) return NULL;
        int id = atoi(row[0]);
        string name = row[1];
        string credential = row[2];
        mysql_free_result(res_set);
        return new User(id, name, credential);
    }

    /**
     * Find a user by their id
     * Params:
     *  userId [IN]: the said id
     * return the pointer to found user
     */
    User* findUserById(int userId) {
        string query = fmt::format(QUERY_FIND_USER_BY_ID, userId);
        MYSQL_RES* res_set = executeQuery(query);
        mysql_free_result(res_set);
        MYSQL_ROW row = mysql_fetch_row(res_set);
        if (row == NULL) return NULL;
        int id = atoi(row[0]);
        string name = row[1];
        string credential = row[2];
        mysql_free_result(res_set);
        return new User(id, name, credential);
    }

    /**
     * Create membership between event and user
     * Params:
     *  userId [IN]: the said event
     *  userId [IN]: the said user
     * return true if success, false otherwise
     */
    bool createMembership(int eventId, int userId) {
        string query = fmt::format(QUERY_CREATE_MEMBERSHIP, eventId, userId);
        executeQuery(query);
        return TRUE;
    }

    /**
     * Concatenate list of strings into one, separated a delimeter
     * Params:
     *  tokens [IN]: the said list
     *  del [IN]: the said delimeter
     * return the concatenated string
     */
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
    /**
     * Establish a database connection when instanciate a service
     */
    AppService() {
        connect = mysql_init(NULL);
        connect = mysql_real_connect(connect, SERVER, USER, PASSWORD, DATABASE, PORT, NULL, 0);
    }
    ~AppService() {
        mysql_close(connect);
        delete connect;
    }

    /**
     * Create a new user
     * Params:
     *  request [IN]: see detail at model/SignUpRequest
     * return the result message
     */
    string createNewUser(SignUpRequest& request) {
        User* user = findUserByName(request.name);
        if (user != NULL) {
            return MESSAGE_NAME_INVALID;
        }
        string addQuery = fmt::format(QUERY_CREATE_USER, request.name, request.credential);
        executeQuery(addQuery);
        return MESSAGE_SUCCESS;
    }

    /**
     * Verify a user trying to log in
     * Params:
     *  request [IN]: see detail at model/LogInRequest
     * return the result message
     */
    string verifyUser(LogInRequest& request, int& token) {
        User* user = findUserByName(request.name);
        if (user == NULL) {
            return MESSAGE_NAME_INVALID;
        }
        if (user->credential != request.credential) {
            return MESSAGE_CREDENTIAL_INVALID;
        }
        token = user->id;
        return MESSAGE_SUCCESS;
    }

    /**
     * Get a list of events
     * Params:
     *  request [IN]: see detail at model/ListEventRequest
     * return the said list
     */
    list<Event*> getEvents(ListEventRequest& request) {
        string query;
        if (request.isMine) {
            query = fmt::format(QUERY_LIST_EVENT_MINE, request.token);
        }
        else {
            query = fmt::format(QUERY_LIST_EVENT);
        }
        MYSQL_RES* res_set = executeQuery(query);
        MYSQL_ROW row;

        list<Event*> result;
        while (row = mysql_fetch_row(res_set)) {
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

    /**
     * Get the detail of a event
     * Params:
     *  request [IN]: see detail at model/DetailEventRequest
     * return the said event
     */
    Event* getEventDetail(DetailEventRequest& request) {
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

    /**
     * Create a new event
     * Params:
     *  request [IN]: see detail at model/CreateEventRequest
     * return the result message
     */
    string createEvent(CreateEventRequest& request) {
        Event* event = request.event;
        string addEventQuery = fmt::format(QUERY_CREATE_EVENT, event->name, event->description, event->time, event->location, event->owner);
        executeQuery(addEventQuery);
        int eventId = mysql_insert_id(connect);
        createMembership(eventId, event->owner);
        return MESSAGE_SUCCESS;
    }

    /**
     * Get a list of users who were not members of a specific event
     * Params:
     *  request [IN]: see detail at model/FreeUsersRequest
     * return the said list
     */
    list<User*> getUsersNotJoinEvent(FreeUsersRequest& request) {
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

    /**
     * Get a list of requests belonging to current user
     * Params:
     *  request [IN]: see detail at model/ListRequestRequest
     * return the said list
     */
    list<AppRequest*> getRequests(ListRequestRequest& request) {
        string query = fmt::format(QUERY_LIST_REQUEST, request.token);
        MYSQL_RES* res_set = executeQuery(query);
        MYSQL_ROW row;

        list<AppRequest*> result;
        while (row = mysql_fetch_row(res_set)) {
            int id = atoi(row[0]);
            int type = atoi(row[1]);
            string event = row[2];
            string targetUser = row[3];
            result.emplace_back(new AppRequest(id, type, event, targetUser));
        }
        mysql_free_result(res_set);
        return result;
    }

    /**
     * Create a ask-to-join request
     * Params:
     *  request [IN]: see detail at model/CreateAskRequest
     * return the result message
     */
    string createAskRequest(CreateAskRequest& request) {
        string query = fmt::format(QUERY_CREATE_ASK_REQUEST, request.eventId, request.eventOwner, request.token);
        executeQuery(query);
        return MESSAGE_SUCCESS;
    }

    /**
     * Create a invite-to-join request
     * Params:
     *  request [IN]: see detail at model/CreateInviteRequest
     * return the result message
     */
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

    /**
     * Accept or reject a request
     * Params:
     *  request [IN]: see detail at model/UpdateRequest
     * return the result message
     */
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