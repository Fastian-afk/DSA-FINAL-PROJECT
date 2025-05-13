#include "Menu.h"
#include "Auth.h"
#include <iostream>
#include <sstream>
using namespace std;
using namespace sf;

Menu::Menu(float w, float h, const string items[], int cnt, FriendSystem* fs)
    : sel(0), count(cnt), currentState(""), currentTheme(nullptr), themeSel(0), themeCount(0), themes(nullptr),
    leaderboard(ScoreBoard::getInstance()), friendSystem(fs) {
    // Load fonts from the fonts folder
    if (!titleFont.loadFromFile("fonts/PressStart2P-Regular.ttf")) {
        titleFont.loadFromFile("C:/WINDOWS/FONTS/IMPACT.TTF"); // Fallback
    }

    if (!menuFont.loadFromFile("fonts/Montserrat-Medium.ttf")) {
        menuFont.loadFromFile("C:/WINDOWS/FONTS/ARIAL.TTF"); // Fallback
    }

    if (!uiFont.loadFromFile("fonts/Roboto-VariableFont_wdth,wght.ttf")) {
        uiFont.loadFromFile("C:/WINDOWS/FONTS/ARIAL.TTF"); // Fallback
    }

    // Set up menu items with the new font
    texts = new sf::Text[count];
    for (int i = 0; i < count; ++i) {
        texts[i].setFont(menuFont);
        texts[i].setString(items[i]);
        texts[i].setCharacterSize(26u); // Slightly smaller for better readability
        float x = w / 2.f - texts[i].getGlobalBounds().width / 2.f;
        float y = h / (count + 1.5) * (i + 1);
        texts[i].setPosition(x, y + 30);
        texts[i].setFillColor(i == 0 ? sf::Color::Yellow : sf::Color::White);
    }

    // Initialize themes
    initializeThemes();
}

Menu::~Menu() {
    delete[] texts;
    delete[] themes;
}

void Menu::initializeThemes() {
    inventory.initialize("themes.txt");
    themeCount = inventory.getThemeCount();
    themes = inventory.getAllThemes();
    if (themeCount > 0) {
        currentTheme = &themes[0];
    }
}

void Menu::setThemeByName(const std::string& themeName) {
    Theme* theme = inventory.searchByName(themeName);
    if (theme) {
        setTheme(theme);
    }
}

void Menu::draw(sf::RenderWindow& window, string name) {
    if (currentState == "welcome") {
        drawWelcomeScreen(window);
    }
    else if (currentState == "login") {
        drawLoginScreen(window, 'l');
    }
    else if (currentState == "register") {
        drawLoginScreen(window, 'r');
    }
    else if (currentState == "profile") {
        drawProfileScreen(window);
    }
    else if (currentState == "themes") {
        drawThemesScreen(window);
    }
    else if (currentState == "instructionsL") {
        drawInstructionsScreen(window, 'l');
    }
    else if (currentState == "instructionsM") {
        drawInstructionsScreen(window, 'm');
    }
    else if (currentState == "leaderboard") {
        drawLeaderboardScreen(window);
    }
    else if (currentState == "friends") {
        drawFriendsScreen(window);
    }
    else {
        // Apply current theme colors if available
        if (currentTheme) {
            window.clear(currentTheme->menuBgColor);
            for (int i = 0; i < count; ++i) {
                texts[i].setFillColor(i == sel ? sf::Color::Yellow : currentTheme->textColor);
            }
        }
        else {
            window.clear(sf::Color(0, 0, 50, 220));
            for (int i = 0; i < count; ++i) {
                texts[i].setFillColor(i == sel ? sf::Color::Yellow : sf::Color::White);
            }
        }

        sf::Text welcomeText;
        welcomeText.setString("Welcome " + name);
        welcomeText.setCharacterSize(28);
        welcomeText.setFillColor(currentTheme ? currentTheme->textColor : sf::Color(240, 240, 240));
        welcomeText.setFont(titleFont);

        sf::FloatRect textRect = welcomeText.getLocalBounds();
        welcomeText.setOrigin(textRect.left + textRect.width / 2.0f,
            textRect.top);
        welcomeText.setPosition(window.getSize().x / 2.0f, 30);

        window.draw(welcomeText);

        // Draw other menu items
        for (int i = 0; i < count; ++i) {
            window.draw(texts[i]);
        }
    }
}

void Menu::up() {
    if (sel > 0) {
        texts[sel].setFillColor(currentTheme ? currentTheme->textColor : sf::Color::White);
        --sel;
        texts[sel].setFillColor(sf::Color::Yellow);
    }
}

void Menu::down() {
    if (sel + 1 < count) {
        texts[sel].setFillColor(currentTheme ? currentTheme->textColor : sf::Color::White);
        ++sel;
        texts[sel].setFillColor(sf::Color::Yellow);
    }
}

int Menu::get() const {
    return sel;
}

string Menu::getState() const {
    return currentState;
}

void Menu::setState(const string& newState) {
    currentState = newState;
    if (currentState != "friends") {
        friendInput = "";
        friendSel = 0;
        friendFocused = false;
    }
}

void Menu::handleInput(Event e) {
    if (e.key.code == 9 && currentState == "login") {
        username = password = "";
        currentState = "register";
    }
    else if (e.key.code == 9 && currentState == "register") {
        username = password = "";
        currentState = "login";
    }
    else if (e.key.code == 8 && (currentState == "profile" || currentState == "themes" ||
        currentState == "instructionsL" || currentState == "instructionsM" ||
        currentState == "leaderboard" || currentState == "friends")) {
        currentState = "main";
        return;
    }

    if (currentState == "themes") {
        if (e.type == Event::KeyReleased) {
            if (e.key.code == Keyboard::Up) {
                if (themeSel > 0) themeSel--;
            }
            else if (e.key.code == Keyboard::Down) {
                if (themeSel < themeCount - 1) themeSel++;
            }
            else if (e.key.code == Keyboard::Enter) {
                currentTheme = &themes[themeSel];
                string currentUsername = username;

                // Read all lines
                ifstream inFile("users.txt");
                vector<string> userLines;
                string line;

                while (getline(inFile, line)) {
                    userLines.push_back(line);
                }
                inFile.close();

                // Update current user's line
                for (auto& userLine : userLines) {
                    vector<string> parts;
                    string tempLine = userLine;  // Work with a copy of the line
                    size_t pos = 0;
                    string delimiter = ":";

                    // Split line into parts without modifying original
                    while ((pos = tempLine.find(delimiter)) != string::npos) {
                        parts.push_back(tempLine.substr(0, pos));
                        tempLine.erase(0, pos + delimiter.length());
                    }
                    parts.push_back(tempLine);

                    // Check if this is the current user
                    if (parts.size() >= 3 && parts[1] == currentUsername) {
                        // Rebuild the line properly
                        string updatedLine;
                        updatedLine += parts[0] + ":" + parts[1] + ":" + parts[2];

                        // Only add theme if we have exactly 3 parts (no existing theme)
                        if (parts.size() == 3) {
                            updatedLine += ":" + currentTheme->name;
                        }
                        // If theme already exists, replace it
                        else if (parts.size() >= 4) {
                            updatedLine += ":" + currentTheme->name;
                            // Preserve any additional fields beyond theme if they exist
                            for (size_t i = 4; i < parts.size(); i++) {
                                updatedLine += ":" + parts[i];
                            }
                        }

                        userLine = updatedLine;
                        break;
                    }
                }

                // Write all lines back to file
                ofstream outFile("users.txt");
                for (const auto& updatedLine : userLines) {
                    outFile << updatedLine << "\n";
                }
                outFile.close();

                currentState = "main";
            }
        }
        return;
    }

    if (currentState == "friends") {
        if (e.type == Event::MouseButtonPressed) {
            Vector2f mousePos(static_cast<float>(e.mouseButton.x), static_cast<float>(e.mouseButton.y));
            if (friendBox.getGlobalBounds().contains(mousePos)) {
                friendFocused = true;
            }
            else {
                friendFocused = false;
            }
        }
        if (e.type == Event::TextEntered && friendFocused) {
            if (e.text.unicode == 8 && !friendInput.empty()) {
                friendInput.pop_back();
            }
            else if (e.text.unicode >= 32 && e.text.unicode <= 126) {
                friendInput += static_cast<char>(e.text.unicode);
            }
        }
        if (e.type == Event::KeyReleased) {
            if (e.key.code == Keyboard::Up) {
                if (friendSel > 0) friendSel--;
            }
            else if (e.key.code == Keyboard::Down) {
                friendSel++;
            }
            else if (e.key.code == Keyboard::Enter && friendFocused && !friendInput.empty()) {
                if (friendSystem->sendFriendRequest(username, friendInput)) {
                    error = false;
                    errorMessage = "Friend request sent to " + friendInput;
                }
                else {
                    error = true;
                    errorMessage = "Failed to send friend request";
                }
                friendInput = "";
            }
            else if (e.key.code == Keyboard::Enter && !friendFocused) {
                string pending = friendSystem->getPendingRequests(username);
                string curr = pending;
                int idx = 0;
                while (!curr.empty() && idx < friendSel) {
                    size_t comma = curr.find(", ");
                    curr = comma == std::string::npos ? "" : curr.substr(comma + 2);
                    idx++;
                }
                if (!curr.empty()) {
                    size_t comma = curr.find(", ");
                    string requester = comma == std::string::npos ? curr : curr.substr(0, comma);
                    if (friendSel % 2 == 0) {
                        if (friendSystem->acceptFriendRequest(username, requester)) {
                            error = false;
                            errorMessage = "Accepted friend request from " + requester;
                        }
                        else {
                            error = true;
                            errorMessage = "Failed to accept friend request";
                        }
                    }
                    else {
                        if (friendSystem->rejectFriendRequest(username, requester)) {
                            error = false;
                            errorMessage = "Rejected friend request from " + requester;
                        }
                        else {
                            error = true;
                            errorMessage = "Failed to reject friend request";
                        }
                    }
                }
            }
        }
        return;
    }

    if (e.key.code == 13 && (currentState == "login" || currentState == "register")) {
        error = false; errorMessage = "";

        cout << "username: " << username << endl;
        cout << "password: " << password << endl;

        // Auth Checking
        Auth newAuth("users.txt");
        bool success = newAuth.handleAuth(currentState, username, password);

        error = newAuth.isError();

        if (!error && success) {
            friendSystem->addPlayer(username);
            currentState = "main";
        }
        else
            errorMessage = newAuth.getError();
    }

    // mouse click detection
    if (e.type == Event::MouseButtonPressed && (currentState == "login" || currentState == "register")) {
        Vector2f mousePos(static_cast<float>(e.mouseButton.x), static_cast<float>(e.mouseButton.y));
        if (userBox.getGlobalBounds().contains(mousePos)) {
            userFocused = true;
            passFocused = false;
        }
        else if (passBox.getGlobalBounds().contains(mousePos)) {
            passFocused = true;
            userFocused = false;
        }
    }

    if (e.type == Event::TextEntered && (currentState == "login" || currentState == "register")) {
        if (e.text.unicode == 8) { // Backspace
            if (userFocused && !username.empty()) username.pop_back();
            else if (passFocused && !password.empty()) password.pop_back();
        }
        else if (e.text.unicode >= 32 && e.text.unicode <= 126) {
            char c = static_cast<char>(e.text.unicode);
            if (userFocused) username += c;
            else if (passFocused) password += c;
        }
    }
}

void Menu::drawWelcomeScreen(RenderWindow& window) {
    // Create a dark overlay for better text visibility
    sf::RectangleShape background(sf::Vector2f(window.getSize().x, window.getSize().y));
    background.setFillColor(currentTheme ? currentTheme->menuBgColor : sf::Color(0, 0, 50, 200));
    window.draw(background);

    // Game Title
    Text welcomeText;
    welcomeText.setFont(titleFont);
    welcomeText.setString("XONIX GAME");
    welcomeText.setCharacterSize(48);
    welcomeText.setFillColor(currentTheme ? currentTheme->textColor : Color::Yellow);
    welcomeText.setStyle(Text::Bold);
    welcomeText.setOutlineColor(sf::Color(255, 165, 0)); // Orange outline (keep as is for contrast)
    welcomeText.setOutlineThickness(2.0f);

    FloatRect textRect = welcomeText.getLocalBounds();
    welcomeText.setOrigin(textRect.left + textRect.width / 2.0f,
        textRect.top + textRect.height / 2.0f);
    welcomeText.setPosition(window.getSize().x / 2.0f,
        window.getSize().y / 2.0f - 50);

    // Subtitle
    Text subtitleText;
    subtitleText.setFont(uiFont);
    subtitleText.setString("Enhanced Edition");
    subtitleText.setCharacterSize(24);
    subtitleText.setFillColor(currentTheme ? currentTheme->textColor : sf::Color(255, 180, 0)); // Light orange

    FloatRect subtitleRect = subtitleText.getLocalBounds();
    subtitleText.setOrigin(subtitleRect.left + subtitleRect.width / 2.0f,
        subtitleRect.top + subtitleRect.height / 2.0f);
    subtitleText.setPosition(window.getSize().x / 2.0f,
        welcomeText.getPosition().y + textRect.height + 10);

    // "Press ENTER" prompt with animation effect (pulsing)
    Text promptText;
    promptText.setFont(uiFont);
    promptText.setString("Press ENTER to continue");
    promptText.setCharacterSize(20);

    // Create a subtle pulse animation
    float pulse = (sin(clock.getElapsedTime().asSeconds() * 3) + 1) * 30 + 195; // Value between 195-255
    promptText.setFillColor(currentTheme ? currentTheme->textColor : sf::Color(255, 255, 255, static_cast<sf::Uint8>(pulse)));

    FloatRect promptRect = promptText.getLocalBounds();
    promptText.setOrigin(promptRect.left + promptRect.width / 2.0f,
        promptRect.top + promptRect.height / 2.0f);
    promptText.setPosition(window.getSize().x / 2.0f,
        subtitleText.getPosition().y + subtitleRect.height + 60);

    window.draw(welcomeText);
    window.draw(subtitleText);
    window.draw(promptText);
}

void Menu::drawLoginScreen(RenderWindow& window, char type) {
    string text = "REGISTER";
    string text2 = "create an account";
    if (type == 'l') {
        text = "LOGIN";
        text2 = "login";
    }

    // Create a dark overlay for better UI visibility
    sf::RectangleShape background(sf::Vector2f(window.getSize().x, window.getSize().y));
    background.setFillColor(currentTheme ? currentTheme->menuBgColor : sf::Color(0, 0, 50, 220));
    window.draw(background);

    // Title
    Text titleText;
    titleText.setFont(titleFont);
    titleText.setString(text);
    titleText.setCharacterSize(36);
    titleText.setFillColor(currentTheme ? currentTheme->textColor : Color::Yellow);
    titleText.setOutlineColor(sf::Color(255, 165, 0));
    titleText.setOutlineThickness(1.0f);

    // Center title
    FloatRect titleRect = titleText.getLocalBounds();
    titleText.setOrigin(titleRect.left + titleRect.width / 2.0f,
        titleRect.top + titleRect.height / 2.0f);
    titleText.setPosition(window.getSize().x / 2.0f, 80);

    // Username label
    Text userLabel;
    userLabel.setFont(uiFont);
    userLabel.setString("Username:");
    userLabel.setCharacterSize(24);
    userLabel.setFillColor(currentTheme ? currentTheme->textColor : Color::White);
    userLabel.setPosition(150, 165);

    // Username underline
    RectangleShape userLine(Vector2f(400, 2));
    userLine.setFillColor(currentTheme ? currentTheme->textColor : Color::White);
    userLine.setPosition(150, 200);

    // Password label
    Text passLabel;
    passLabel.setFont(uiFont);
    passLabel.setString("Password:");
    passLabel.setCharacterSize(24);
    passLabel.setFillColor(currentTheme ? currentTheme->textColor : Color::White);
    passLabel.setPosition(150, 265);

    // Password underline
    RectangleShape passLine(Vector2f(400, 2));
    passLine.setFillColor(currentTheme ? currentTheme->textColor : Color::White);
    passLine.setPosition(150, 300);

    // Login prompt
    Text loginPrompt;
    loginPrompt.setFont(uiFont);
    loginPrompt.setString("Press ENTER to " + text2);
    loginPrompt.setCharacterSize(20);
    loginPrompt.setFillColor(Color::Green); // Keep as green for contrast
    loginPrompt.setPosition(150, 320);

    // Register prompt
    Text registerPrompt;
    registerPrompt.setFont(uiFont);
    registerPrompt.setCharacterSize(16);
    registerPrompt.setFillColor(Color::Cyan); // Keep as cyan for contrast
    registerPrompt.setPosition(150, 440);

    if (type == 'l') {
        registerPrompt.setString("Don't have an account? Press TAB to create one");
    }

    if (type == 'r') {
        registerPrompt.setString("Already have an account? Press TAB to login");
    }

    // focusable areas
    userBox.setSize(sf::Vector2f(400, 40));
    userBox.setPosition(150, 160);
    userBox.setFillColor(sf::Color(255, 255, 255, 20));
    userBox.setOutlineThickness(2);
    userBox.setOutlineColor(userFocused ? Color::Cyan : Color::Transparent);

    passBox.setSize(sf::Vector2f(400, 40));
    passBox.setPosition(150, 260);
    passBox.setFillColor(sf::Color(255, 255, 255, 20));
    passBox.setOutlineThickness(2);
    passBox.setOutlineColor(passFocused ? Color::Cyan : Color::Transparent);

    // Username text
    Text userText;
    userText.setFont(uiFont);
    userText.setCharacterSize(22);
    userText.setFillColor(currentTheme ? currentTheme->textColor : Color::White);
    userText.setString(username);
    userText.setPosition(userBox.getPosition().x + 5 + 125, userBox.getPosition().y + 7);

    // Password text (mask it with '*')
    Text passText;
    passText.setFont(uiFont);
    passText.setCharacterSize(22);
    passText.setFillColor(currentTheme ? currentTheme->textColor : Color::White);
    passText.setString(std::string(password.length(), '*'));
    passText.setPosition(passBox.getPosition().x + 5 + 125, passBox.getPosition().y + 10);

    // Error message
    Text errorText;
    errorText.setFont(uiFont);
    errorText.setCharacterSize(18);
    errorText.setFillColor(Color::Red); // Keep as red for contrast
    if (error)
        errorText.setString(errorMessage);
    errorText.setPosition(150, loginPrompt.getPosition().y + 50);

    // Draw all elements
    window.draw(titleText);
    window.draw(userLabel);
    window.draw(userLine);
    window.draw(passLabel);
    window.draw(passLine);
    window.draw(loginPrompt);
    window.draw(registerPrompt);
    window.draw(userBox);
    window.draw(passBox);
    window.draw(userText);
    window.draw(passText);
    window.draw(errorText);
}

void Menu::drawProfileScreen(RenderWindow& window) {
    // Create a dark overlay for better UI visibility
    sf::RectangleShape background(sf::Vector2f(window.getSize().x, window.getSize().y));
    background.setFillColor(currentTheme ? currentTheme->menuBgColor : sf::Color(0, 0, 20, 220));
    window.draw(background);

    // Title
    Text titleText;
    titleText.setFont(titleFont);
    titleText.setString("PLAYER PROFILE");
    titleText.setCharacterSize(24);
    titleText.setFillColor(currentTheme ? currentTheme->textColor : Color::Yellow);
    titleText.setOutlineColor(sf::Color(255, 165, 0));
    titleText.setOutlineThickness(1.0f);
    titleText.setPosition(20, 25);

    window.draw(titleText);

    // Player username display
    Text usernameText;
    usernameText.setFont(menuFont);
    usernameText.setString("Player: " + username);
    usernameText.setCharacterSize(20);
    usernameText.setFillColor(Color::Cyan); // Keep as cyan for contrast
    usernameText.setPosition(20, 65);
    window.draw(usernameText);

    // Friends list
    Text friendsText;
    friendsText.setFont(uiFont);
    string txt = friendSystem->getFriends(username);
    friendsText.setString(txt == "" ? "Friends: No friends currently" : "Friends: " + friendSystem->getFriends(username));
    friendsText.setCharacterSize(18);
    friendsText.setFillColor(currentTheme ? currentTheme->textColor : Color::White);
    friendsText.setPosition(20, 100);
    window.draw(friendsText);

    // Retrieve game stats from FriendSystem
    string statsStr = friendSystem->getGameStats(username);
    vector<string> stats;
    stringstream ss(statsStr);
    string line;
    while (getline(ss, line)) {
        stats.push_back(line);
    }
    if (stats.empty()) {
        stats.push_back("No game data available");
        stats.push_back("Play some games to see your stats!");
    }

    // Stats background
    RectangleShape statsBg(Vector2f(620, 320));
    statsBg.setFillColor(currentTheme ? sf::Color(currentTheme->menuBgColor.r, currentTheme->menuBgColor.g, currentTheme->menuBgColor.b, 180) : Color(0, 0, 80, 180));
    statsBg.setOutlineColor(currentTheme ? currentTheme->gridColor : Color(100, 100, 255));
    statsBg.setOutlineThickness(2);
    statsBg.setPosition(20, 130);
    window.draw(statsBg);

    // Stats title
    Text statsTitle;
    statsTitle.setFont(uiFont);
    statsTitle.setString("GAME STATISTICS");
    statsTitle.setCharacterSize(20);
    statsTitle.setFillColor(currentTheme ? currentTheme->textColor : Color(255, 200, 0));
    statsTitle.setStyle(Text::Bold);
    statsTitle.setPosition(50, 145);
    window.draw(statsTitle);

    // Display stats
    float yPos = 180;
    for (size_t i = 0; i < stats.size(); ++i) {
        Text statText;
        statText.setFont(uiFont);
        statText.setString(stats[i]);
        statText.setCharacterSize(18);

        // Alternate colors for better readability
        if (i % 2 == 0) {
            statText.setFillColor(currentTheme ? currentTheme->textColor : Color(200, 240, 255));
        }
        else {
            statText.setFillColor(currentTheme ? sf::Color(currentTheme->textColor.r, currentTheme->textColor.g, currentTheme->textColor.b, 200) : Color(180, 220, 255));
        }

        // Highlight important stats
        if (stats[i].find("High Score") != string::npos) {
            statText.setFillColor(Color::Yellow);
            statText.setStyle(Text::Bold);
        }

        statText.setPosition(50, yPos);
        window.draw(statText);
        yPos += 30;
    }

    // Back prompt
    Text backPrompt;
    backPrompt.setFont(uiFont);
    backPrompt.setString("Press BACKSPACE to return to main menu");
    backPrompt.setCharacterSize(18);
    backPrompt.setFillColor(Color(150, 255, 150)); // Keep as green for contrast
    backPrompt.setPosition(20, 450);
    window.draw(backPrompt);
}

void Menu::drawThemesScreen(sf::RenderWindow& window) {
    // Create a dark overlay for better UI visibility
    sf::RectangleShape background(sf::Vector2f(window.getSize().x, window.getSize().y));
    background.setFillColor(currentTheme ? currentTheme->menuBgColor : sf::Color(0, 0, 50, 220));
    window.draw(background);

    // Title
    Text titleText;
    titleText.setFont(titleFont);
    titleText.setString("THEMES");
    titleText.setCharacterSize(36);
    titleText.setFillColor(currentTheme ? currentTheme->textColor : sf::Color::Yellow);
    titleText.setPosition(15, 30);

    window.draw(titleText);

    // Display themes with better formatting
    float startY = 80;
    float spacing = 70; // More space for two lines per theme
    for (int i = 0; i < themeCount; ++i) {
        // Theme name (bold, yellow if selected)
        Text themeName;
        themeName.setFont(menuFont);
        themeName.setString(themes[i].name);
        themeName.setCharacterSize(26);
        themeName.setStyle(Text::Bold);
        themeName.setFillColor(i == themeSel ? sf::Color::Yellow : (currentTheme ? currentTheme->textColor : sf::Color(200, 200, 255)));
        themeName.setPosition(20, startY + i * spacing);
        window.draw(themeName);

        // Theme description (smaller, white)
        Text themeDesc;
        themeDesc.setFont(uiFont);
        themeDesc.setString(themes[i].description);
        themeDesc.setCharacterSize(18);
        themeDesc.setFillColor(currentTheme ? currentTheme->textColor : sf::Color(220, 220, 220));
        themeDesc.setPosition(20, startY + i * spacing + 30); // Slightly increased spacing
        window.draw(themeDesc);
    }

    // Back prompt
    Text backText;
    backText.setFont(uiFont);
    backText.setString("Press BACKSPACE to return | ENTER to select");
    backText.setCharacterSize(20);
    backText.setFillColor(currentTheme ? currentTheme->textColor : sf::Color(200, 200, 200));

    backText.setPosition(20, window.getSize().y - 40);
    window.draw(backText);
}

void Menu::drawInstructionsScreen(RenderWindow& window, char type) {
    string titleText;
    const int MAX_INSTRUCTIONS = 30; // Maximum number of instruction lines
    string instructions[MAX_INSTRUCTIONS];
    int numInstructions = 0;

    // Set title and instructions based on game type
    if (type == 'm') {
        titleText = "MultiPlayer Instructions";
        const string multiplayerInstructions[] = {
            "Game Objective:",
            "Capture more territory than your opponent while avoiding enemies.",
            "First player to claim 70% of the area wins the round.",
            "",
            "Player 1 Controls:",
            "Use arrow keys to move",
            "",
            "Player 2 Controls:",
            "Use WASD keys to move",
            "",
            "Scoring:",
            "- Each captured tile grants 1 point",
            "- Capturing opponent's trail ends the game",
            "- Power-ups affect both players",
            "",
            "Special Rules:",
            "- Colliding with opponent's trail ends your game",
            "- Enemies can destroy both players' trails",
            "",
            "Press P to pause/play the game",
            "Press ESC to exit the game"
        };
        numInstructions = sizeof(multiplayerInstructions) / sizeof(multiplayerInstructions[0]);
        for (int i = 0; i < numInstructions; i++) {
            instructions[i] = multiplayerInstructions[i];
        }
    }
    else {
        titleText = "Single Player Instructions";
        const string singleplayerInstructions[] = {
            "Game Objective:",
            "Capture territory by drawing lines across the game area without being hit",
            "by enemies. Claim at least 70% of the area to advance to the next level.",
            "",
            "Controls:",
            "Use arrow keys to move",
            "Press P to pause/play the game. Press SPACE to activate powerup",
            "",
            "Scoring:",
            "- Capturing a tile grants 1 point",
            "- Capturing more than 10 tiles at once awards double points",
            "",
            "Power-ups:",
            "- At 500 points, you earn a power-up to freeze enemies for 3 seconds",
            "- Additional power-ups at 1500, 2500 and so on",
            "",
        };
        numInstructions = sizeof(singleplayerInstructions) / sizeof(singleplayerInstructions[0]);
        for (int i = 0; i < numInstructions; i++) {
            instructions[i] = singleplayerInstructions[i];
        }
    }

    // Background
    sf::RectangleShape background(sf::Vector2f(window.getSize().x, window.getSize().y));
    background.setFillColor(currentTheme ? currentTheme->menuBgColor : sf::Color(0, 0, 50, 220));
    window.draw(background);

    // Title
    Text title;
    title.setFont(titleFont);
    title.setString(titleText);
    title.setCharacterSize(20);
    title.setFillColor(currentTheme ? currentTheme->textColor : Color::Yellow);
    title.setOutlineColor(sf::Color(255, 165, 0));
    title.setOutlineThickness(1.0f);
    title.setPosition(20, 22);
    window.draw(title);

    // Instructions area
    const float startY = 70;
    const float lineHeight = 24;
    const float visibleLines = 15; // Number of lines visible at once
    const float areaHeight = visibleLines * lineHeight;

    // Draw instructions background
    RectangleShape instructionsBg(Vector2f(window.getSize().x - 40, areaHeight + 20));
    instructionsBg.setFillColor(currentTheme ? sf::Color(currentTheme->menuBgColor.r, currentTheme->menuBgColor.g, currentTheme->menuBgColor.b, 180) : Color(0, 0, 80, 180));
    instructionsBg.setOutlineColor(currentTheme ? currentTheme->gridColor : Color(100, 100, 255));
    instructionsBg.setOutlineThickness(2);
    instructionsBg.setPosition(20, startY - 10);
    window.draw(instructionsBg);

    // Draw instructions with scroll offset
    float yPos = startY - scrollOffset;
    for (int i = 0; i < numInstructions; i++) {
        // Only draw visible lines
        if (yPos + lineHeight > startY && yPos < startY + areaHeight) {
            Text line;
            line.setFont(uiFont);
            line.setString(instructions[i]);

            // Style headings differently
            if (instructions[i].find(":") != string::npos ||
                instructions[i].find("Objective") != string::npos ||
                instructions[i].find("Rules") != string::npos) {
                line.setFillColor(Color::Cyan); // Keep as cyan for contrast
                line.setCharacterSize(20);
            }
            else {
                line.setFillColor(currentTheme ? currentTheme->textColor : Color::White);
                line.setCharacterSize(18);
            }

            line.setPosition(40, yPos);
            window.draw(line);
        }
        yPos += lineHeight;
    }

    // Bottom prompt with animation
    Text promptText;
    promptText.setFont(uiFont);
    promptText.setString("Press BACKSPACE to return to menu");
    promptText.setCharacterSize(18);

    promptText.setFillColor(currentTheme ? currentTheme->textColor : sf::Color(255, 255, 255));
    promptText.setPosition(20, window.getSize().y - 33);
    window.draw(promptText);
}

void Menu::drawLeaderboardScreen(sf::RenderWindow& window) {
    leaderboard.draw(window, uiFont);
}

void Menu::drawFriendsScreen(sf::RenderWindow& window) {
    // Background
    sf::RectangleShape background(sf::Vector2f(window.getSize().x, window.getSize().y));
    background.setFillColor(currentTheme ? currentTheme->menuBgColor : sf::Color(0, 0, 50, 220));
    window.draw(background);

    // Title
    Text titleText;
    titleText.setFont(titleFont);
    titleText.setString("FRIENDS");
    titleText.setCharacterSize(36);
    titleText.setFillColor(currentTheme ? currentTheme->textColor : sf::Color::Yellow);
    FloatRect titleRect = titleText.getLocalBounds();
    titleText.setOrigin(titleRect.left + titleRect.width / 2.0f, titleRect.top + titleRect.height / 2.0f);
    titleText.setPosition(window.getSize().x / 2.0f, 50);
    window.draw(titleText);

    // Friend search input
    Text friendLabel;
    friendLabel.setFont(uiFont);
    friendLabel.setString("Search Player:");
    friendLabel.setCharacterSize(24);
    friendLabel.setFillColor(currentTheme ? currentTheme->textColor : Color::White);
    friendLabel.setPosition(150, 100);
    window.draw(friendLabel);

    friendBox.setSize(sf::Vector2f(400, 40));
    friendBox.setPosition(150, 130);
    friendBox.setFillColor(sf::Color(255, 255, 255, 20));
    friendBox.setOutlineThickness(2);
    friendBox.setOutlineColor(friendFocused ? Color::Cyan : Color::Transparent);
    window.draw(friendBox);

    Text friendText;
    friendText.setFont(uiFont);
    friendText.setCharacterSize(22);
    friendText.setFillColor(currentTheme ? currentTheme->textColor : Color::White);
    friendText.setString(friendInput);
    friendText.setPosition(friendBox.getPosition().x + 5, friendBox.getPosition().y + 7);
    window.draw(friendText);

    Text friendPrompt;
    friendPrompt.setFont(uiFont);
    friendPrompt.setString("Press ENTER to send friend request");
    friendPrompt.setCharacterSize(20);
    friendPrompt.setFillColor(Color::Green); // Keep as green for contrast
    friendPrompt.setPosition(150, 170);
    window.draw(friendPrompt);

    // Friends list
    Text friendsTitle;
    friendsTitle.setFont(uiFont);
    friendsTitle.setString("Your Friends:");
    friendsTitle.setCharacterSize(20);
    friendsTitle.setFillColor(Color::Cyan); // Keep as cyan for contrast
    friendsTitle.setPosition(150, 210);
    window.draw(friendsTitle);

    Text friendsText;
    friendsText.setFont(uiFont);
    string txt = friendSystem->getFriends(username);
    friendsText.setString(txt == "" ? "No friends currently" : friendSystem->getFriends(username));
    friendsText.setCharacterSize(18);
    friendsText.setFillColor(currentTheme ? currentTheme->textColor : Color::White);
    friendsText.setPosition(150, 240);
    window.draw(friendsText);

    // Pending requests
    Text pendingTitle;
    pendingTitle.setFont(uiFont);
    pendingTitle.setString("Pending Requests:");
    pendingTitle.setCharacterSize(20);
    pendingTitle.setFillColor(Color::Cyan); // Keep as cyan for contrast
    pendingTitle.setPosition(150, 280);
    window.draw(pendingTitle);

    string pending = friendSystem->getPendingRequests(username);
    float yPos = 310;
    int idx = 0;
    string curr = pending;
    while (!curr.empty()) {
        size_t comma = curr.find(", ");
        string requester = comma == std::string::npos ? curr : curr.substr(0, comma);
        Text reqText;
        reqText.setFont(uiFont);
        reqText.setString("Accept: " + requester);
        reqText.setCharacterSize(18);
        reqText.setFillColor(idx == friendSel ? Color::Yellow : (currentTheme ? currentTheme->textColor : Color::White));
        reqText.setPosition(150, yPos);
        window.draw(reqText);
        yPos += 30;
        idx++;
        Text rejText;
        rejText.setFont(uiFont);
        rejText.setString("Reject: " + requester);
        rejText.setCharacterSize(18);
        rejText.setFillColor(idx == friendSel ? Color::Yellow : (currentTheme ? currentTheme->textColor : Color::White));
        rejText.setPosition(150, yPos);
        window.draw(rejText);
        yPos += 30;
        idx++;
        curr = comma == std::string::npos ? "" : curr.substr(comma + 2);
    }

    // Error/message
    Text errorText;
    errorText.setFont(uiFont);
    errorText.setCharacterSize(18);
    errorText.setFillColor(error ? Color::Red : Color::Green);
    errorText.setString(errorMessage);
    errorText.setPosition(150, yPos);
    window.draw(errorText);

    // Back prompt
    Text backText;
    backText.setFont(uiFont);
    backText.setString("Press BACKSPACE to return | ENTER to act on selection");
    backText.setCharacterSize(20);
    backText.setFillColor(currentTheme ? currentTheme->textColor : sf::Color(200, 200, 200));
    FloatRect backRect = backText.getLocalBounds();
    backText.setOrigin(backRect.left + backRect.width / 2.0f, backRect.top + backRect.height / 2.0f);
    backText.setPosition(window.getSize().x / 2.0f, window.getSize().y - 50);
    window.draw(backText);
}