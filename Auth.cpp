#include "Auth.h"
#include <iostream>
#include <iomanip>
#include <sstream>

using namespace std;

Auth::Auth(const std::string& filePath) : file(filePath) {
    error = false;
    errorMsg = "";
}

string Auth::generateNewId() {
    ifstream inFile(file);
    string line;
    int maxId = 0;

    while (getline(inFile, line)) {
        stringstream ss(line);
        string idStr;
        getline(ss, idStr, ':');
        try {
            int id = stoi(idStr);
            if (id > maxId) maxId = id;
        }
        catch (...) {
            continue;
        }
    }

    ostringstream oss;
    oss << setw(3) << setfill('0') << (maxId + 1);
    return oss.str();
}

// Function to create a new user (username, password)
bool Auth::createUser(const string username, const string password) {
    // Input validation
    if (username.empty() || password.empty()) {
        error = true;
        errorMsg = "Username and password cannot be empty!";
        return false;
    }

    if (checkUsernameExists(username)) {
        error = true;
        errorMsg = "Username already exists!";
        return false;
    }

    if (password.length() < 6) {
        error = true;
        errorMsg = "Password must be at least 6 characters long!";
        return false;
    }

    // Generate user data
    string id = generateNewId();

    ofstream outFile(file, ios::app);
    if (!outFile.is_open()) {
        error = true;
        errorMsg = "Failed to open file for writing.";
        return false;
    }

    outFile << id << ":" << username << ":" << password << "\n";
    outFile.close();

    // Create player stats file
    if (!createPlayerFile(username)) {
        error = true;
        errorMsg = "Account created but couldn't initialize stats";
    }

    return true;
}


// check if the username exists in the file
bool Auth::checkUsernameExists(const string username) {
    ifstream inFile(file);
    string line;
    while (getline(inFile, line)) {
        stringstream ss(line);
        string id, storedUsername;
        getline(ss, id, ':');
        getline(ss, storedUsername, ':');
        if (storedUsername == username) {
            return true;
        }
    }
    return false;
}

bool Auth::createPlayerFile(const std::string& username) {

    std::ofstream playerFile("players/" + username + ".txt");
    if (!playerFile.is_open()) {
        return false;
    }

    // Write default stats
    playerFile << "Total Games Played: 0\n";
    playerFile << "Games Won: 0\n";
    playerFile << "High Score: 0\n";
    playerFile << "Last Score: 0\n";
    playerFile << "Average Score: 0\n";
    playerFile << "--- Recent Games ---\n";
    playerFile << "No games played yet\n";

    playerFile.close();
    return true;
}

//authenticate a user 
bool Auth::authenticateUser(const string username, const string password) {
    ifstream inFile(file);
    string line;
    while (getline(inFile, line)) {
        stringstream ss(line);
        string id, storedUsername, storedPassword;
        getline(ss, id, ':');
        getline(ss, storedUsername, ':');
        getline(ss, storedPassword, ':');

        if (storedUsername == username && storedPassword == password) {
            return true;
        }
    }
    return false;
}

bool Auth::handleAuth(string currentState, string username, string password) {
    error = false;
    errorMsg = "";

    // check empty
    if (username.empty() || password.empty()) {
        error = true;
        errorMsg = "Fill all the fields!";
        return false;
    }

    if (currentState == "login") {
        // check for username existence/password correct
        if (checkUsernameExists(username)) {
            if (authenticateUser(username, password))
                return true;
            else {
                error = true;
                errorMsg = "Incorrect password!";
                return false;
            }
        }
        else {
            error = true;
            errorMsg = "User does not exist!";
            return false;
        }
    }
    else if (currentState == "register") {
        return createUser(username, password);
    }

    return false;
}

