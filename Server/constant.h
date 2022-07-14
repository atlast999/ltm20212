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

#define CODE_SUCCESS 200
#define CODE_ERROR 400

#define MESSAGE_SUCCESS "OK"
#define MESSAGE_UNAUTHENTICATED "Unauthenticated"
#define MESSAGE_INVALID_OPERATION "Invalid Operation"
#define MESSAGE_NAME_INVALID "Invalid Name"

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


#endif