#pragma once
#pragma once
#include <string>
#include <fstream>
#include "Auth.h"

const int INITIAL_ARRAY_SIZE = 10;
const int HASH_TABLE_SIZE = 100;

class LinkedList {
public:
    struct Node {
        std::string data;
        Node* next;
        Node(const std::string& d) : data(d), next(nullptr) {}
    };
    Node* head;
public:
    LinkedList() : head(nullptr) {}
    ~LinkedList();
    void append(const std::string& data);
    bool remove(const std::string& data);
    bool contains(const std::string& data) const;
    std::string toString() const;
    void clear();
};

class DynamicArray {
private:
    struct Player {
        std::string username;
        LinkedList friends;
        LinkedList pendingRequests;
        // Game statistics
        int totalGames;
        int gamesWon;
        int highScore;
        int lastScore;
        float averageScore;
        LinkedList recentGames; // Stores last 5 game results as "Win: X" or "Loss: X"
    };
public:
    Player* array;
    int size;
    int capacity;
    void resize();
public:
    DynamicArray();
    ~DynamicArray();
    int addPlayer(const std::string& username);
    int findPlayer(const std::string& username) const;
    bool addFriend(int playerIdx, const std::string& friendName, bool pending);
    bool acceptFriend(int playerIdx, const std::string& friendName);
    bool rejectFriend(int playerIdx, const std::string& friendName);
    std::string getFriends(int playerIdx) const;
    std::string getPendingRequests(int playerIdx) const;
    int getSize() const { return size; }
    // New methods for game stats
    void updateGameStats(int playerIdx, int score, bool won);
    std::string getGameStats(int playerIdx) const;
};

class HashTable {
private:
    struct HashNode {
        std::string key;
        int value;
        HashNode* next;
        HashNode(const std::string& k, int v) : key(k), value(v), next(nullptr) {}
    };
    HashNode* table[HASH_TABLE_SIZE];
    int hashFunction(const std::string& key) const;
public:
    HashTable();
    ~HashTable();
    void insert(const std::string& key, int value);
    int find(const std::string& key) const;
    void remove(const std::string& key);
};

class FriendSystem {
private:
    DynamicArray players;
    HashTable usernameToIndex;
    Auth auth;
    void savePlayerData(const std::string& username);
    void loadPlayerData(const std::string& username);
public:
    FriendSystem();
    ~FriendSystem();
    bool addPlayer(const std::string& username);
    bool sendFriendRequest(const std::string& sender, const std::string& receiver);
    bool acceptFriendRequest(const std::string& receiver, const std::string& sender);
    bool rejectFriendRequest(const std::string& receiver, const std::string& sender);
    std::string getFriends(const std::string& username) const;
    std::string getPendingRequests(const std::string& username) const;
    // New method to update game stats
    void updateGameStats(const std::string& username, int score, bool won);
    std::string getGameStats(const std::string& username) const;
};