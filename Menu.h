#pragma once
#include <SFML/Graphics.hpp>
#include <string>
#include "Inventory.h"
#include "ScoreBoard.h"
#include "FriendSystem.h"

class Menu {
private:
    int sel, count;
    sf::Font titleFont;  // For game title
    sf::Font menuFont;   // For menu items
    sf::Font uiFont;     // For UI elements
    sf::Text* texts;
    std::string currentState;
    std::string username = "";
    std::string password = "";
    std::string friendInput = ""; // For friend search/request
    int friendSel = 0; // Selected friend/pending request

    bool userFocused = true;
    bool passFocused = false;
    bool friendFocused = false;
    sf::RectangleShape userBox, passBox, friendBox;
    bool error = false;
    std::string errorMessage = "";
    sf::Clock clock;  // For animations
    float scrollOffset = 0;

    // Theme management
    Inventory inventory;
    Theme* currentTheme;
    int themeSel = 0;
    int themeCount = 0;
    Theme* themes = nullptr;

    // Leaderboard reference
    ScoreBoard& leaderboard;

    // Friend system pointer (shared instance)
    FriendSystem* friendSystem;

public:
    // Constructor/Destructor
    Menu(float width, float height, const std::string items[], int itemCount, FriendSystem* fs);
    ~Menu();

    // Drawing
    void draw(sf::RenderWindow& window, std::string name);
    void drawWelcomeScreen(sf::RenderWindow& window);
    void drawLoginScreen(sf::RenderWindow& window, char type);
    void drawProfileScreen(sf::RenderWindow& window);
    void drawInstructionsScreen(sf::RenderWindow& window, char type);
    void drawThemesScreen(sf::RenderWindow& window);
    void drawLeaderboardScreen(sf::RenderWindow& window);
    void drawFriendsScreen(sf::RenderWindow& window);

    void handleInput(sf::Event e);

    // Navigation
    void up();
    void down();
    int get() const;

    // State management
    std::string getState() const;
    void setState(const std::string& adapts);
    void setName(std::string s) { username = s; }
    std::string getName() { return username; }

    // Access to fonts for other classes
    const sf::Font& getTitleFont() const { return titleFont; }
    const sf::Font& getUIFont() const { return uiFont; }

    // Theme management
    void initializeThemes();
    Theme* getCurrentTheme() const { return currentTheme; }
    void setTheme(Theme* theme) { currentTheme = theme; } // New method to set the theme
    void setThemeByName(const std::string& themeName);

    // Leaderboard management
    void updateLeaderboard(const std::string& username, int score) {
        leaderboard.insert(username, score);
    }

    // Friend system access
    FriendSystem* getFriendSystem() { return friendSystem; }
};