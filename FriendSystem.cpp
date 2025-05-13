#include "FriendSystem.h"
#include <cstring>
#include <sstream>
#include <iostream>

// Utility function to trim whitespace manually
static std::string trim(const std::string& str) {
    std::string result = str;
    size_t start = 0;
    while (start < result.length() && (result[start] == ' ' || result[start] == '\t' || result[start] == '\n' || result[start] == '\r')) {
        start++;
    }
    size_t end = result.length();
    while (end > start && (result[end - 1] == ' ' || result[end - 1] == '\t' || result[end - 1] == '\n' || result[end - 1] == '\r')) {
        end--;
    }
    if (start >= result.length()) return "";
    return result.substr(start, end - start);
}

// LinkedList Implementation
LinkedList::~LinkedList() {
    clear();
}

void LinkedList::append(const std::string& data) {
    std::string trimmedData = trim(data);
    if (trimmedData.empty()) return;
    Node* newNode = new Node(trimmedData);
    if (!head) {
        head = newNode;
        return;
    }
    Node* curr = head;
    while (curr->next) curr = curr->next;
    curr->next = newNode;
}

bool LinkedList::remove(const std::string& data) {
    if (!head) return false;
    std::string trimmedData = trim(data);
    if (trimmedData.empty()) return false;
    if (head->data == trimmedData) {
        Node* temp = head;
        head = head->next;
        delete temp;
        return true;
    }
    Node* curr = head;
    while (curr->next && curr->next->data != trimmedData) curr = curr->next;
    if (curr->next) {
        Node* temp = curr->next;
        curr->next = temp->next;
        delete temp;
        return true;
    }
    return false;
}

bool LinkedList::contains(const std::string& data) const {
    std::string trimmedData = trim(data);
    if (trimmedData.empty()) return false;
    Node* curr = head;
    while (curr) {
        if (curr->data == trimmedData) return true;
        curr = curr->next;
    }
    return false;
}

std::string LinkedList::toString() const {
    std::string result;
    Node* curr = head;
    while (curr) {
        result += curr->data;
        if (curr->next) result += ", ";
        curr = curr->next;
    }
    return result.empty() ? "" : result;
}

void LinkedList::clear() {
    Node* curr = head;
    while (curr) {
        Node* temp = curr;
        curr = curr->next;
        delete temp;
    }
    head = nullptr;
}

// DynamicArray Implementation
DynamicArray::DynamicArray() : size(0), capacity(INITIAL_ARRAY_SIZE) {
    array = new Player[capacity];
}

DynamicArray::~DynamicArray() {
    delete[] array;
}

void DynamicArray::resize() {
    capacity *= 2;
    Player* newArray = new Player[capacity];
    for (int i = 0; i < size; ++i) {
        newArray[i] = array[i];
    }
    delete[] array;
    array = newArray;
}

int DynamicArray::addPlayer(const std::string& username) {
    int existingIdx = findPlayer(username);
    if (existingIdx != -1) return existingIdx;
    if (size == capacity) resize();
    array[size].username = username;
    array[size].friends.clear();
    array[size].pendingRequests.clear();
    // Initialize game stats
    array[size].totalGames = 0;
    array[size].gamesWon = 0;
    array[size].highScore = 0;
    array[size].lastScore = 0;
    array[size].averageScore = 0.0f;
    array[size].recentGames.clear();
    return size++;
}

int DynamicArray::findPlayer(const std::string& username) const {
    for (int i = 0; i < size; ++i) {
        if (array[i].username == username) return i;
    }
    return -1;
}

bool DynamicArray::addFriend(int playerIdx, const std::string& friendName, bool pending) {
    if (playerIdx < 0 || playerIdx >= size) return false;
    if (pending) {
        if (array[playerIdx].friends.contains(friendName)) return false;
        if (array[playerIdx].pendingRequests.contains(friendName)) return false;
        array[playerIdx].pendingRequests.append(friendName);
    }
    else {
        if (!array[playerIdx].friends.contains(friendName)) {
            array[playerIdx].friends.append(friendName);
        }
    }
    return true;
}

bool DynamicArray::acceptFriend(int playerIdx, const std::string& friendName) {
    if (playerIdx < 0 || playerIdx >= size) return false;
    if (array[playerIdx].pendingRequests.remove(friendName)) {
        array[playerIdx].friends.append(friendName);
        return true;
    }
    return false;
}

bool DynamicArray::rejectFriend(int playerIdx, const std::string& friendName) {
    if (playerIdx < 0 || playerIdx >= size) return false;
    return array[playerIdx].pendingRequests.remove(friendName);
}

std::string DynamicArray::getFriends(int playerIdx) const {
    if (playerIdx < 0 || playerIdx >= size) return "None";
    return array[playerIdx].friends.toString();
}

std::string DynamicArray::getPendingRequests(int playerIdx) const {
    if (playerIdx < 0 || playerIdx >= size) return "None";
    return array[playerIdx].pendingRequests.toString();
}

void DynamicArray::updateGameStats(int playerIdx, int score, bool won) {
    if (playerIdx < 0 || playerIdx >= size) return;

    Player& player = array[playerIdx];
    player.totalGames++;
    if (won) player.gamesWon++;
    player.lastScore = score;
    if (score > player.highScore) player.highScore = score;

    // Update average score
    player.averageScore = ((player.averageScore * (player.totalGames - 1)) + score) / player.totalGames;

    // Update recent games (limit to 5)
    std::string gameResult = (won ? "Win: " : "Loss: ") + std::to_string(score);
    player.recentGames.append(gameResult);
    int count = 0;
    LinkedList::Node* curr = player.recentGames.head;
    while (curr) {
        count++;
        curr = curr->next;
    }
    while (count > 5) {
        player.recentGames.remove(player.recentGames.head->data);
        count--;
    }
}

std::string DynamicArray::getGameStats(int playerIdx) const {
    if (playerIdx < 0 || playerIdx >= size) return "No stats available";

    const Player& player = array[playerIdx];
    std::stringstream ss;
    ss << "Total Games Played: " << player.totalGames << "\n";
    ss << "Games Won: " << player.gamesWon << "\n";
    ss << "High Score: " << player.highScore << "\n";
    ss << "Last Score: " << player.lastScore << "\n";
    ss << "Average Score: " << static_cast<int>(player.averageScore) << "\n";
    ss << "Recent Games: " << player.recentGames.toString();
    return ss.str();
}

// HashTable Implementation
HashTable::HashTable() {
    for (int i = 0; i < HASH_TABLE_SIZE; ++i) {
        table[i] = nullptr;
    }
}

HashTable::~HashTable() {
    for (int i = 0; i < HASH_TABLE_SIZE; ++i) {
        HashNode* curr = table[i];
        while (curr) {
            HashNode* temp = curr;
            curr = curr->next;
            delete temp;
        }
    }
}

int HashTable::hashFunction(const std::string& key) const {
    unsigned long hash = 0;
    for (char c : key) {
        hash = (hash * 31) + c;
    }
    return hash % HASH_TABLE_SIZE;
}

void HashTable::insert(const std::string& key, int value) {
    int index = hashFunction(key);
    HashNode* newNode = new HashNode(key, value);
    if (!table[index]) {
        table[index] = newNode;
    }
    else {
        HashNode* curr = table[index];
        while (curr->next) curr = curr->next;
        curr->next = newNode;
    }
}

int HashTable::find(const std::string& key) const {
    int index = hashFunction(key);
    HashNode* curr = table[index];
    while (curr) {
        if (curr->key == key) return curr->value;
        curr = curr->next;
    }
    return -1;
}

void HashTable::remove(const std::string& key) {
    int index = hashFunction(key);
    HashNode* curr = table[index];
    HashNode* prev = nullptr;
    while (curr && curr->key != key) {
        prev = curr;
        curr = curr->next;
    }
    if (!curr) return;
    if (!prev) {
        table[index] = curr->next;
    }
    else {
        prev->next = curr->next;
    }
    delete curr;
}

// FriendSystem Implementation
FriendSystem::FriendSystem() : auth("users.txt") {}

FriendSystem::~FriendSystem() {
    for (int i = 0; i < players.getSize(); ++i) {
        savePlayerData(players.array[i].username);
    }
}

void FriendSystem::savePlayerData(const std::string& username) {
    int idx = usernameToIndex.find(username);
    if (idx == -1) return;

    std::string filepath = "players/" + username + ".txt";
    std::ofstream out(filepath);
    if (!out.is_open()) return;

    // Write game statistics
    out << "[GAME STATISTICS]\n";
    out << "Total Games Played: " << players.array[idx].totalGames << "\n";
    out << "Games Won: " << players.array[idx].gamesWon << "\n";
    out << "High Score: " << players.array[idx].highScore << "\n";
    out << "Last Score: " << players.array[idx].lastScore << "\n";
    out << "Average Score: " << static_cast<int>(players.array[idx].averageScore) << "\n";
    out << "Recent Games: " << players.array[idx].recentGames.toString() << "\n";
    // Write friends section
    out << "[FRIENDS]\n";
    out << "accepted: " << players.getFriends(idx) << "\n";
    out << "pending: " << players.getPendingRequests(idx) << "\n";

    out.close();
}

void FriendSystem::loadPlayerData(const std::string& username) {
    int idx = usernameToIndex.find(username);
    if (idx == -1) {
        std::cout << "Load failed: No index for " << username << std::endl;
        return;
    }

    std::string filepath = "players/" + username + ".txt";
    std::ifstream in(filepath);
    if (!in.is_open()) {
        std::cout << "File not found: " << filepath << std::endl;
        // Initialize stats if file doesn't exist
        players.array[idx].totalGames = 0;
        players.array[idx].gamesWon = 0;
        players.array[idx].highScore = 0;
        players.array[idx].lastScore = 0;
        players.array[idx].averageScore = 0.0f;
        players.array[idx].friends.clear();
        players.array[idx].pendingRequests.clear();
        players.array[idx].recentGames.clear();
        savePlayerData(username); // Create the file with default stats
        return;
    }

    players.array[idx].friends.clear();
    players.array[idx].pendingRequests.clear();
    players.array[idx].recentGames.clear();

    std::string line;
    bool inGameStatsSection = false;
    bool inFriendsSection = false;
    while (std::getline(in, line)) {
        std::string trimmedLine = trim(line);
        if (trimmedLine == "[GAME STATISTICS]") {
            inGameStatsSection = true;
            inFriendsSection = false;
            continue;
        }
        if (trimmedLine == "[FRIENDS]") {
            inGameStatsSection = false;
            inFriendsSection = true;
            continue;
        }

        if (inGameStatsSection && !trimmedLine.empty()) {
            size_t colon = trimmedLine.find(":");
            if (colon != std::string::npos) {
                std::string key = trimmedLine.substr(0, colon);
                std::string value = trim(trimmedLine.substr(colon + 1));
                if (key == "Total Games Played") {
                    players.array[idx].totalGames = std::stoi(value);
                }
                else if (key == "Games Won") {
                    players.array[idx].gamesWon = std::stoi(value);
                }
                else if (key == "High Score") {
                    players.array[idx].highScore = std::stoi(value);
                }
                else if (key == "Last Score") {
                    players.array[idx].lastScore = std::stoi(value);
                }
                else if (key == "Average Score") {
                    players.array[idx].averageScore = std::stof(value);
                }
                else if (key == "Recent Games" && value != "") {
                    std::string curr = value;
                    while (!curr.empty()) {
                        size_t comma = curr.find(",");
                        std::string gameResult = comma == std::string::npos ? curr : curr.substr(0, comma);
                        gameResult = trim(gameResult);
                        if (!gameResult.empty()) {
                            players.array[idx].recentGames.append(gameResult);
                        }
                        curr = comma == std::string::npos ? "" : curr.substr(comma + 1);
                    }
                }
            }
        }

        if (inFriendsSection && !trimmedLine.empty()) {
            size_t colon = trimmedLine.find(":");
            if (colon != std::string::npos) {
                std::string key = trimmedLine.substr(0, colon);
                std::string value = trimmedLine.substr(colon + 1);
                if (key == "accepted") {
                    std::string curr = value;
                    while (!curr.empty() && curr != "None") {
                        size_t comma = curr.find(",");
                        std::string friendName = comma == std::string::npos ? curr : curr.substr(0, comma);
                        friendName = trim(friendName);
                        if (!friendName.empty()) {
                            players.addFriend(idx, friendName, false);
                        }
                        curr = comma == std::string::npos ? "" : curr.substr(comma + 1);
                    }
                }
                else if (key == "pending") {
                    std::string curr = value;
                    while (!curr.empty() && curr != "None") {
                        size_t comma = curr.find(",");
                        std::string requester = comma == std::string::npos ? curr : curr.substr(0, comma);
                        requester = trim(requester);
                        if (!requester.empty()) {
                            players.addFriend(idx, requester, true);
                        }
                        curr = comma == std::string::npos ? "" : curr.substr(comma + 1);
                    }
                }
            }
        }
    }
    in.close();
}

bool FriendSystem::addPlayer(const std::string& username) {
    if (!auth.checkUsernameExists(username)) return false;

    int idx = players.findPlayer(username);
    if (idx == -1) {
        idx = players.addPlayer(username);
        if (idx == -1) return false;
        usernameToIndex.insert(username, idx);
    }
    loadPlayerData(username);
    return true;
}

bool FriendSystem::sendFriendRequest(const std::string& sender, const std::string& receiver) {
    if (!auth.checkUsernameExists(sender) || !auth.checkUsernameExists(receiver)) return false;

    addPlayer(sender);
    addPlayer(receiver);

    int senderIdx = usernameToIndex.find(sender);
    int receiverIdx = usernameToIndex.find(receiver);

    if (senderIdx == -1 || receiverIdx == -1 || sender == receiver) return false;

    if (players.array[receiverIdx].friends.contains(sender) ||
        players.array[receiverIdx].pendingRequests.contains(sender)) return false;

    bool success = players.addFriend(receiverIdx, sender, true);
    if (success) {
        savePlayerData(receiver);
        loadPlayerData(receiver);

    }
    return success;
}

bool FriendSystem::acceptFriendRequest(const std::string& receiver, const std::string& sender) {
    int receiverIdx = usernameToIndex.find(receiver);
    int senderIdx = usernameToIndex.find(sender);

    if (senderIdx == -1)
        addPlayer(sender);

    senderIdx = usernameToIndex.find(sender);

    loadPlayerData(receiver);
    loadPlayerData(sender);

    if (!players.array[receiverIdx].pendingRequests.contains(sender)) return false;

    if (!players.acceptFriend(receiverIdx, sender)) return false;

    if (!players.addFriend(senderIdx, receiver, false)) {
        players.array[receiverIdx].pendingRequests.append(sender);
        players.array[receiverIdx].friends.remove(sender);
        savePlayerData(receiver);
        savePlayerData(sender);
        loadPlayerData(receiver);
        loadPlayerData(sender);
        return false;
    }

    savePlayerData(receiver);
    savePlayerData(sender);
    loadPlayerData(receiver);
    loadPlayerData(sender);
    return true;
}

bool FriendSystem::rejectFriendRequest(const std::string& receiver, const std::string& sender) {
    int receiverIdx = usernameToIndex.find(receiver);
    if (receiverIdx == -1) return false;

    loadPlayerData(receiver);
    if (!players.array[receiverIdx].pendingRequests.contains(sender)) return false;

    bool success = players.rejectFriend(receiverIdx, sender);
    if (success) {
        savePlayerData(receiver);
        loadPlayerData(receiver);
    }
    return success;
}

std::string FriendSystem::getFriends(const std::string& username) const {
    int idx = usernameToIndex.find(username);
    return players.getFriends(idx);
}

std::string FriendSystem::getPendingRequests(const std::string& username) const {
    int idx = usernameToIndex.find(username);
    return players.getPendingRequests(idx);
}

void FriendSystem::updateGameStats(const std::string& username, int score, bool won) {
    int idx = usernameToIndex.find(username);
    if (idx == -1) return;

    players.updateGameStats(idx, score, won);
    savePlayerData(username);
}

std::string FriendSystem::getGameStats(const std::string& username) const {
    int idx = usernameToIndex.find(username);
    return players.getGameStats(idx);
}