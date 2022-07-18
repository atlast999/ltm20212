#ifndef MODEL_H_
#define MODEL_H_
#define FMT_HEADER_ONLY

#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "fmt\core.h"
#include "constant.h"
#include <string>
#include <list>

#include<iostream>

using namespace rapidjson;
using namespace std;

/**
 * Interface allows to convert model to json string and vice versa 
 *
 */
class Serializable
{
private:
	Document document;
public:
    /**
    * To convert json string to model
    * SubType needs to call this before converting
    */
	virtual void deserialize(string raw) {
		this->document.Parse(raw.c_str());
	}
    /**
    * To convert model to json string
    * SubType needs to call this before converting
    */
	virtual string serialize() {
		this->document.SetObject();
		return "";
	}
    /**
    * SubType needs to override this in order to be a array response
    */
    virtual Value& toObject() {
        Value object(kObjectType);
        return object;
    }
protected:
    Document::AllocatorType& getAllocator() {
        return document.GetAllocator();
    }
	Value& getByKey(const char * key) {
		return this->document[key];
	}
	void addString(const char * key, string value) {
		Value val(kObjectType);
		val.SetString(value.c_str(), value.length(), document.GetAllocator());
		document.AddMember(StringRef(key), val, document.GetAllocator());
	}
    void addInt(const char * key, int value) {
        document.AddMember(StringRef(key), value, document.GetAllocator());
    }
    template<typename T>
    void addArray(const char * key, list<T> &values) {
        Value array(kArrayType);
        array.Reserve(values.size(), document.GetAllocator());
        for (Serializable *value : values) {
            string raw = value->serialize();
            Document itemDoc;
            itemDoc.Parse(raw.c_str());
            Value itemObj(itemDoc, itemDoc.GetAllocator());
            array.PushBack(itemObj, document.GetAllocator());
        }
        document.AddMember(StringRef(key), array, document.GetAllocator());
    }
    void addObject(const char * key, Serializable &value) {
        string raw = value.serialize();
        Document object;
        object.Parse(raw.c_str());
        document.AddMember(StringRef(key), object, document.GetAllocator());
    }
	string stringify() {
		StringBuffer buffer;
		Writer<StringBuffer> writer(buffer);
		document.Accept(writer);
		return buffer.GetString();
	}
};

class BaseRequest : public Serializable {
public:
    int operation = -1;
    int token = 0;
    void deserialize(string raw) {
        Serializable::deserialize(raw);
        this->token = getByKey(KEY_TOKEN).GetInt();
        this->operation = getByKey(KEY_OPERATION).GetInt();
    }
	string serialize() {
		Serializable::serialize();
		addInt(KEY_OPERATION, this->operation);
		addInt(KEY_TOKEN, this->token);
		return stringify();
	}
    bool isAuthenticated() {
        return (this->operation == OP_SIGN_UP || this->operation == OP_LOG_IN || this->token > 0);
    }
};

class BaseResponse : public Serializable {
public:
    int code;
    string message;
	BaseResponse() {}
    BaseResponse(int code, const char * message){
        this->code = code;
        this->message = string(message);
    }
	void deserialize(string raw) {
		Serializable::deserialize(raw);
		this->code = getByKey(KEY_CODE).GetInt();
		this->message = getByKey(KEY_MESSAGE).GetString();
	}
    string serialize() {
		Serializable::serialize();
        addInt(KEY_CODE, this->code);
		addString(KEY_MESSAGE, this->message);
		return stringify();
	}
};

class User : public Serializable {
public:
    int id;
	string name;
    string credential;
	User() : User(0, "") {}
	User(int id, string name) : User(id, name, "") {}
    User(int id, string name, string credential) {
        this->id = id;
		this->name = name;
		this->credential = credential;
	}
	void deserialize(string raw) {
		Serializable::deserialize(raw);
        this->id = getByKey(KEY_ID).GetInt();
		this->name = getByKey(KEY_NAME).GetString();
	}
    string serialize() {
		Serializable::serialize();
        addInt(KEY_ID, this->id);
		addString(KEY_NAME, this->name);
		return stringify();
	}
};

class Event : public Serializable {
public:
    int id;
	string name;
    string description;
    string time;
    string location;
    int owner;
    Event(int id, string name, string description, string time, string location, int owner) {
        this->id = id;
		this->name = name;
        this->description = description;
        this->time = time;
        this->location = location;
        this->owner = owner;
	}
	void deserialize(string raw) {
		Serializable::deserialize(raw);
        this->id = getByKey(KEY_ID).GetInt();
		this->name = getByKey(KEY_NAME).GetString();
        this->description = getByKey(KEY_DESCRIPTION).GetString();
        this->time = getByKey(KEY_TIME).GetString();
        this->location = getByKey(KEY_LOCATION).GetString();
        this->owner = getByKey(KEY_OWNER).GetInt();
	}
	string serialize() {
		Serializable::serialize();
        addInt(KEY_ID, this->id);
		addString(KEY_NAME, this->name);
        addString(KEY_DESCRIPTION, this->description);
        addString(KEY_TIME, this->time);
        addString(KEY_LOCATION, this->location);
        addInt(KEY_OWNER, this->owner);
		return stringify();
	}
   /* Value& toObject() {
        Value &object = Serializable::toObject();
        object.AddMember()
    }*/
};

#define REQUEST_TYPE_INVITE 1
#define REQUEST_TYPE_ASK 2
#define INVITE_REQUEST_NAME "{} invited you to join event {}"
#define ASK_REQUEST_NAME "{} asked to join your event {}"

class AppRequest : public Serializable {
public: 
    int id;
    string name;
    AppRequest(int id, int type, string event, string targetUser) {
        this->id = id;
        if (type == REQUEST_TYPE_INVITE) {
            this->name = fmt::format(INVITE_REQUEST_NAME, targetUser, event);
        }
        else {
            this->name = fmt::format(ASK_REQUEST_NAME, targetUser, event);
        }
    }
    void deserialize(string raw) {
        Serializable::deserialize(raw);
        this->id = getByKey(KEY_ID).GetInt();
        this->name = getByKey(KEY_NAME).GetString();
    }
    string serialize() {
        Serializable::serialize();
        addInt(KEY_ID, this->id);
        addString(KEY_NAME, this->name);
        return stringify();
    }

};

//OP_SIGN_UP
class SignUpRequest : public BaseRequest {
public:
    string name;
    string credential;
	SignUpRequest() {}
	SignUpRequest(string name, string credential) {
        this->operation = OP_SIGN_UP;
		this->name = name;
		this->credential = credential;
	}
    void deserialize(string raw) {
        BaseRequest::deserialize(raw);
        this->name = getByKey(KEY_NAME).GetString();
		this->credential = getByKey(KEY_CREDENTIAL).GetString();
    }
	string serialize() {
		BaseRequest::serialize();
		addString(KEY_NAME, this->name);
		addString(KEY_CREDENTIAL, this->credential);
		return stringify();
	}
};
class SignUpResponse : public BaseResponse {
public:
    SignUpResponse() {}
	SignUpResponse(int code, const char * message) : BaseResponse(code, message) {}
};

//OP_LOG_IN
class LogInRequest : public SignUpRequest {
public:
	LogInRequest() {}
	LogInRequest(string name, string credential) : SignUpRequest(name, credential) {
        this->operation = OP_LOG_IN;
	}
};
class LogInResponse : public BaseResponse {
public:
    int token = -1;
    LogInResponse() {}
    LogInResponse(int code, const char * message, int token) : BaseResponse(code, message) {
        this->token = token;
    }
    void deserialize(string raw) {
        BaseResponse::deserialize(raw);
        this->token = getByKey(KEY_TOKEN).GetInt();
    }
	string serialize() {
		BaseResponse::serialize();
		addInt(KEY_TOKEN, this->token);
		return stringify();
	}
};

//OP_LIST_EVENT
class ListEventRequest : public BaseRequest {
public:
    int isMine = 1;
    ListEventRequest() {}
    ListEventRequest(int isMine, int token) {
        this->operation = OP_LIST_EVENT;
        this->token = token;
        this->isMine = isMine;
    }
    void deserialize(string raw) {
        BaseRequest::deserialize(raw);
        this->isMine = getByKey(KEY_MINE).GetInt();
    }
    string serialize() {
        BaseRequest::serialize();
        addInt(KEY_MINE, this->isMine);
        return stringify();
    }
};
class ListEventResponse : public BaseResponse {
public:
    list<Event*> events;
    ListEventResponse() {}
    ListEventResponse(int code, const char* message, list<Event*>& events) : BaseResponse(code, message) {
        this->events = events;
    }
    string serialize() {
        BaseResponse::serialize();
        addArray(KEY_DATA, this->events);
        return stringify();
    }
};

//OP_LIST_REQUEST
class ListRequestRequest : public BaseRequest {
public:
    ListRequestRequest() { }
    ListRequestRequest(int token) {
        this->operation = OP_LIST_REQUEST;
        this->token = token;
    }
};
class ListRequestResponse : public BaseResponse {
public:
    list<AppRequest*> requests;
    ListRequestResponse() {}
    ListRequestResponse(int code, const char * message, list<AppRequest*>& requests) : BaseResponse(code, message) {
        this->requests = requests;
    }
    string serialize() {
        BaseResponse::serialize();
        addArray(KEY_DATA, this->requests);
        return stringify();
    }
};

//class InviteUsersRequest : public BaseRequest {
//public:
//    int eventId;
//    void deserialize(string raw) {
//        BaseRequest::deserialize(raw);
//        this->eventId = getByKey(KEY_EVENT_ID).GetInt();
//    }
//};
//
//class InviteUsersResponse : public BaseResponse {
//private:
//    list<User*> users;
//public:
//    InviteUsersResponse(int code, const char * message, list<User*> &users) : BaseResponse(code, message) {
//        this->users = users;
//    }
//    string serialize() {
//        BaseResponse::serialize();
//        addArray(KEY_DATA, this->users);
//        // addObject("object", *this->data.front()); //this helps add object to json
//        return stringify();
//    }
//};

#endif