#include "ScoreDisplay.h"

ScoreDisplay::ScoreDisplay() : fontLoaded(false) {
    player1Text.setString("Player 1: 0");
    player2Text.setString("Player 2: 0");
    levelText.setString("Level: 1");
}

bool ScoreDisplay::loadFont(const std::string& fontPath) {
    // Try to load the modern font first
    if (!font.loadFromFile("Roboto-VariableFont_wdth,wght.ttf")) {
        // Fallback to the specified font path or system font
        if (!font.loadFromFile(fontPath)) {
            return false;
        }
    }

    player1Text.setFont(font);
    player2Text.setFont(font);
    levelText.setFont(font);

    player1Text.setCharacterSize(20); // Slightly smaller for better readability
    player2Text.setCharacterSize(20);
    levelText.setCharacterSize(20);

    // Add some style to make the text look better
    player1Text.setStyle(sf::Text::Bold);
    player2Text.setStyle(sf::Text::Bold);
    levelText.setStyle(sf::Text::Bold);

    player1Text.setPosition(10, 10);
    player2Text.setPosition(10, 40);
    levelText.setPosition(10, 70);

    fontLoaded = true;
    return true;
}

void ScoreDisplay::update(int p1Score, int p2Score, int level, bool gameOver, bool multiplayer) {
    if (!fontLoaded) return;

    player1Text.setString("Player 1: " + std::to_string(p1Score));
    levelText.setString("Level: " + std::to_string(level));

    if (multiplayer) {
        player2Text.setString("Player 2: " + std::to_string(p2Score));
    }
    else {
        player2Text.setString("");
    }

    // If game over, could change text color to red
    if (gameOver) {
        player1Text.setFillColor(sf::Color::Red);
        player2Text.setFillColor(sf::Color::Red);
        levelText.setFillColor(sf::Color::Red);
    }
    else {
        player1Text.setFillColor(sf::Color::White);
        player2Text.setFillColor(sf::Color::White);
        levelText.setFillColor(sf::Color::White);
    }
}

void ScoreDisplay::draw(sf::RenderWindow& window) {
    if (!fontLoaded) return;

    window.draw(player1Text);
    window.draw(player2Text);
    window.draw(levelText);
}