#pragma once
#include <string>
#include <SFML/Graphics.hpp>
#include <fstream>
#include <iostream>
#include <sstream>

struct PlayerScore {
    std::string username;
    int score;

    PlayerScore() : username(""), score(0) {}
    PlayerScore(const std::string& name, int s) : username(name), score(s) {}
};

class ScoreBoard {
private:
    static const int MAX_SIZE = 10;
    PlayerScore scores[MAX_SIZE];
    int currentSize;
    const std::string SCORES_FILE = "leaderboard.txt";

    // Singleton instance
    static ScoreBoard* instance;

    // Private constructor for singleton
    ScoreBoard() : currentSize(0) {
        loadScores();
    }

    void heapifyDown(int index) {
        int smallest = index;
        int left = 2 * index + 1;
        int right = 2 * index + 2;

        if (left < currentSize && scores[left].score < scores[smallest].score)
            smallest = left;

        if (right < currentSize && scores[right].score < scores[smallest].score)
            smallest = right;

        if (smallest != index) {
            PlayerScore temp = scores[index];
            scores[index] = scores[smallest];
            scores[smallest] = temp;
            heapifyDown(smallest);
        }
    }

    void heapifyUp(int index) {
        int parent = (index - 1) / 2;
        if (index > 0 && scores[index].score < scores[parent].score) {
            PlayerScore temp = scores[index];
            scores[index] = scores[parent];
            scores[parent] = temp;
            heapifyUp(parent);
        }
    }

    void buildHeap() {
        // Start from the last non-leaf node and heapify down
        for (int i = (currentSize - 1) / 2; i >= 0; i--) {
            heapifyDown(i);
        }
    }

    void sortScores(PlayerScore* sorted, int size) {
        // Simple bubble sort for descending order
        for (int i = 0; i < size - 1; i++) {
            for (int j = 0; j < size - i - 1; j++) {
                if (sorted[j].score < sorted[j + 1].score) {
                    PlayerScore temp = sorted[j];
                    sorted[j] = sorted[j + 1];
                    sorted[j + 1] = temp;
                }
            }
        }
    }

    bool saveScores() {
        try {
            std::ofstream file(SCORES_FILE, std::ios::out);
            if (!file.is_open()) {
                std::cerr << "Failed to open leaderboard file for writing" << std::endl;
                return false;
            }

            // Save scores in sorted order for better readability
            PlayerScore sorted[MAX_SIZE];
            getSortedScores(sorted);

            file.seekp(0);
            file.clear();
            for (int i = 0; i < currentSize; i++) {
                file << sorted[i].username << " " << sorted[i].score << "\n";
            }

            file.close();
            std::cout << "Saved " << currentSize << " scores to leaderboard" << std::endl;
            return true;
        }
        catch (const std::exception& e) {
            std::cerr << "Error saving scores: " << e.what() << std::endl;
            return false;
        }
    }

    bool loadScores() {
        try {
            std::ifstream file(SCORES_FILE);
            if (!file.is_open()) {
                std::cout << "No existing leaderboard file found, starting fresh" << std::endl;
                return true;
            }

            currentSize = 0;
            std::string line;
            while (std::getline(file, line)) {
                size_t spacePos = line.find(" ");
                if (spacePos != std::string::npos) {
                    std::string username = line.substr(0, spacePos);
                    std::string scoreStr = line.substr(spacePos + 1);
                    int score = 0;
                    for (char c : scoreStr) {
                        if (c >= '0' && c <= '9') {
                            score = score * 10 + (c - '0');
                        }
                    }
                    if (!username.empty() && score > 0) {
                        insert(username, score);
                    }
                }
            }

            file.close();
            if (currentSize > 0) {
                std::cout << "Loaded " << currentSize << " scores from leaderboard" << std::endl;
            }
            else {
                std::cout << "Loaded 0 valid scores from leaderboard" << std::endl;
            }
            return true;
        }
        catch (const std::exception& e) {
            std::cerr << "Error loading scores: " << e.what() << std::endl;
            return false;
        }
    }

    int findPlayer(const std::string& username) {
        for (int i = 0; i < currentSize; i++) {
            if (scores[i].username == username) return i;
        }
        return -1;
    }

public:
    ScoreBoard(const ScoreBoard&) = delete;
    ScoreBoard& operator=(const ScoreBoard&) = delete;

    static ScoreBoard& getInstance() {
        if (instance == nullptr) {
            instance = new ScoreBoard();
        }
        return *instance;
    }

    void insert(const std::string& username, int score) {
        if (username.empty() || score <= 0) return;

        int existingIndex = findPlayer(username);
        if (existingIndex != -1) {
            if (score > scores[existingIndex].score) {
                scores[existingIndex].score = score;
                heapifyDown(existingIndex);
            }
        }
        else if (currentSize < MAX_SIZE) {
            scores[currentSize] = PlayerScore(username, score);
            heapifyUp(currentSize);
            currentSize++;
        }
        else if (score > scores[0].score) {
            scores[0] = PlayerScore(username, score);
            heapifyDown(0);
        }

        if (!saveScores()) {
            std::cerr << "Failed to save scores after insertion" << std::endl;
        }
    }

    void getSortedScores(PlayerScore* sorted) {
        for (int i = 0; i < currentSize; i++) {
            sorted[i] = scores[i];
        }
        sortScores(sorted, currentSize);
    }

    void clear() {
        currentSize = 0;
        if (!saveScores()) {
            std::cerr << "Failed to save scores after clearing" << std::endl;
        }
    }

    int size() const {
        return currentSize;
    }

    void draw(sf::RenderWindow& window, const sf::Font& font) {
        // Create background
        sf::RectangleShape background(sf::Vector2f(window.getSize().x, window.getSize().y));
        background.setFillColor(sf::Color(0, 0, 50, 220));
        window.draw(background);

        // Title
        sf::Text title;
        title.setFont(font);
        title.setString("LEADERBOARD");
        title.setCharacterSize(30);
        title.setFillColor(sf::Color::Yellow);
        title.setPosition(window.getSize().x / 2 - title.getGlobalBounds().width / 2, 20);
        window.draw(title);

        // Get sorted scores
        PlayerScore sorted[MAX_SIZE];
        getSortedScores(sorted);

        // Draw scores
        for (int i = 0; i < currentSize; i++) {
            sf::Text rank;
            rank.setFont(font);
            rank.setString(std::to_string(i + 1) + ".");
            rank.setCharacterSize(24);
            rank.setFillColor(sf::Color::White);
            rank.setPosition(100, 80 + i * 40);
            window.draw(rank);

            sf::Text name;
            name.setFont(font);
            name.setString(sorted[i].username);
            name.setCharacterSize(24);
            name.setFillColor(sf::Color::White);
            name.setPosition(150, 80 + i * 40);
            window.draw(name);

            sf::Text score;
            score.setFont(font);
            score.setString(std::to_string(sorted[i].score));
            score.setCharacterSize(24);
            score.setFillColor(sf::Color::White);
            score.setPosition(window.getSize().x - 150, 80 + i * 40);
            window.draw(score);
        }

        // Back prompt (adjusted position to avoid overlap)
        sf::Text backPrompt;
        backPrompt.setFont(font);
        backPrompt.setString("Press BACKSPACE to return");
        backPrompt.setCharacterSize(20);
        backPrompt.setFillColor(sf::Color(150, 255, 150));
        backPrompt.setPosition(20, 460); // Adjusted to be below the 10th entry (440 + 20 buffer)
        window.draw(backPrompt);
    }
};