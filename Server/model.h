#ifndef MODEL_H_
#define MODEL_H_

#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
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
protected:
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
        array.SetArray();
        for (Serializable *value : values) {
            string raw = value->serialize();
            Document item;
            item.Parse(raw.c_str());
            array.PushBack(item, document.GetAllocator());
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
        return (this->operation == OP_SIGN_UP || this->token > 0);
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
    string serialize() {
		Serializable::serialize();
        addInt(KEY_CODE, this->code);
		addString(KEY_MESSAGE, this->message);
		return stringify();
	}
	void deserialize(string raw) {
		Serializable::deserialize(raw);
		this->code = getByKey(KEY_CODE).GetInt();
		this->message = getByKey(KEY_MESSAGE).GetString();
	}
};

class User : public Serializable {
private:
    int id;
	string name;
public:
	User() : User(0, "") {}
	User(int id, string name) {
        this->id = id;
		this->name = name;
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

class InviteUsersRequest : public BaseRequest {
public:
    int eventId;
    void deserialize(string raw) {
        BaseRequest::deserialize(raw);
        this->eventId = getByKey(KEY_EVENT_ID).GetInt();
    }
};

class InviteUsersResponse : public BaseResponse {
private:
    list<User*> users;
public:
    InviteUsersResponse(int code, const char * message, list<User*> &users) : BaseResponse(code, message) {
        this->users = users;
    }
    string serialize() {
        BaseResponse::serialize();
        addArray(KEY_DATA, this->users);
        // addObject("object", *this->data.front()); //this helps add object to json
        return stringify();
    }
};

class CreateUserRequest : public BaseRequest {
public:
    string name;
    string credential;
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

#endif