#pragma once
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

using namespace std;

class Auth {
private:
    string file;
    string errorMsg;
    bool error;
public:
    Auth(const std::string& filePath);
    string generateNewId();
    bool handleAuth(string currentState, string username, string password);
    bool createUser(const string username, const string password);
    bool checkUsernameExists(const string username);
    bool authenticateUser(const string username, const string password);
    bool createPlayerFile(const string& username);

    string getError() { return errorMsg; };
    bool isError() { return error; };
};