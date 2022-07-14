// Server.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include <iostream>
#include <string>

using namespace rapidjson;
using namespace std;

#define KEY_NAME "name"
#define KEY_CREDENTIAL "credential"

class Serializable {
public:
	/**
	* Convert model to json
	*
	*/
	void deserialize(string raw) {
		this->document.Parse(raw.c_str());
	}
	string serialize() {
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
	string stringify() {
		StringBuffer buffer;
		Writer<StringBuffer> writer(buffer);
		document.Accept(writer);
		return buffer.GetString();
	}
private:
	Document document;
};

class User : public Serializable {
private:
	string name;
	string credential;
public:
	User() {}
	User(string name, string credential) {
		this->name = name;
		this->credential = credential;
	}
	void deserialize(string raw) {
		Serializable::deserialize(raw);
		this->name = getByKey(KEY_NAME).GetString();
		this->credential = getByKey(KEY_CREDENTIAL).GetString();
	}
	string serialize() {
		Serializable::serialize();
		addString(KEY_NAME, this->name);
		addString(KEY_CREDENTIAL, this->credential);
		return stringify();
	}
};

int main(int argc, char const *argv[])
{
	const char *json = "{\"name\":\"rapidjson\",\"credential\":\"mypass\"}";
	User user;
	user.deserialize(string(json));
	string res = user.serialize();
	cout << res << endl;
	return 0;
}
