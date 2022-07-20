#ifndef CONSTANT_H_
#define CONSTANT_H_

#define KEY_ID "id"
#define KEY_NAME "name"
#define KEY_CREDENTIAL "credential"
#define KEY_TOKEN "token"
#define KEY_OPERATION "operation"
#define KEY_EVENT_ID "eventId"
#define KEY_CODE "code"
#define KEY_MESSAGE "message"
#define KEY_DATA "data"
#define KEY_DESCRIPTION "description"
#define KEY_TIME "time"
#define KEY_LOCATION "location"
#define KEY_OWNER "owner"
#define KEY_MINE "isMine"
#define KEY_STATUS "status"

#define CODE_SUCCESS 200
#define CODE_ERROR 400

#define MESSAGE_SUCCESS "OK"
#define MESSAGE_UNAUTHENTICATED "Unauthenticated"
#define MESSAGE_INVALID_OPERATION "Invalid Operation"
#define MESSAGE_NAME_INVALID "Invalid Name"
#define MESSAGE_CREDENTIAL_INVALID "Password Invalide"
#define MESSAGE_EVENT_NOT_EXIST "Event is not exist"

#define OP_SIGN_UP 0
#define OP_LOG_IN 1
#define OP_LIST_EVENT 2
#define OP_DETAIL_EVENT 3
#define OP_CREATE_EVENT 4
#define OP_USERS_NOT_JOINING_EVENT 5
#define OP_LIST_REQUEST 6
#define OP_CREATE_ASK_REQUEST 7
#define OP_CREATE_INVITE_REQUEST 8
#define OP_UPDATE_REQUEST 9


#define REQUEST_TYPE_INVITE 1
#define REQUEST_TYPE_ASK 2
#define INVITE_REQUEST_NAME "{} invited you to join event {}"
#define ASK_REQUEST_NAME "{} asked to join your event {}"
#define REQUEST_STATUS_ACCEPT 1
#define REQUEST_STATUS_REJECT 2

#define QUERY_FIND_USER_BY_NAME R"(
    SELECT * FROM users
    WHERE 
        users.name = '{}'
    LIMIT 1
)"

#define QUERY_FIND_USER_BY_ID R"(
    SELECT * FROM users
    WHERE 
        users.id = {}
    LIMIT 1
)"

#define QUERY_CREATE_USER R"(
    INSERT INTO users(name, credential)
    VALUES
    ('{}', '{}')
)"

#define QUERY_LIST_EVENT R"(
    SELECT * FROM events
)"

#define QUERY_LIST_EVENT_MINE R"(
    SELECT * FROM events
    WHERE 
        events.owner = {}
)"

#define QUERY_GET_EVENT_BY_ID R"(
    SELECT * FROM events
    WHERE
        events.id = {}
)"

#define QUERY_CREATE_EVENT R"(
    INSERT INTO events(name, description, time, location, owner)
    VALUES
    ('{}', '{}', '{}', '{}', {})
)"

#define QUERY_CREATE_MEMBERSHIP R"(
    INSERT INTO memberships(eventId, userId)
    VALUES
    ({}, {})
)"

#define QUERY_USERS_NOT_JOIN_EVENT R"(
    SELECT * FROM users
    WHERE users.id NOT IN (
        SELECT memberships.userId
        FROM memberships
        WHERE
            memberships.eventId = {}
    )
)"

#define QUERY_LIST_REQUEST R"(
    SELECT requests.id, requests.type, events.name AS eventName, targetUser.name AS targetUser
    FROM requests, events, users AS targetUser
    WHERE 
        requests.status = 0 and
        requests.owner = {} and
        events.id = requests.eventId and 
        targetUser.id = requests.target
)"

#define QUERY_CREATE_ASK_REQUEST R"(
    INSERT INTO requests(eventId, owner, target, type, status)
    VALUES
    ({}, {}, {}, 2, 0)
)"

#define QUERY_CREATE_INVITE_REQUEST R"(
    INSERT INTO requests(eventId, owner, target, type, status) 
    VALUES
    {}
)"

#define INSERT_INVITE_REQUEST_PATTERN "({}, {}, {}, 1, 0)"

#define QUERY_UPDATE_REQUEST R"(
    UPDATE requests
    SET status = {}
    WHERE id = {}
)"

#define QUERY_GET_REQUEST_BY_ID R"(
    SELECT * FROM requests
    WHERE requests.id = {}
)"

#endif