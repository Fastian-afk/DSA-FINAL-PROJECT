#pragma once
#include <SFML/Graphics.hpp>

class Player {
public:
    Player(int startX, int startY, sf::Keyboard::Key left, sf::Keyboard::Key right,
        sf::Keyboard::Key up, sf::Keyboard::Key down, sf::Color color);

    void update();
    void reset(int x, int y);
    void handleInput();
    void draw(sf::RenderWindow& window, const sf::Sprite& tileSprite) const;
    void stop();

    // Getters
    int getX() const { return x; }
    int getY() const { return y; }
    int getScore() const { return score; }
    bool isAlive() const { return alive; }

    // Setters
    void setPosition(int newX, int newY);
    void addScore(int points) { score += points; }
    void setAlive(bool status) { alive = status; }
    void resetScore() { score = 0; }

private:
    int x, y;
    int dx, dy;
    int score;
    bool alive;
    // Constants
    static const int M = 25, N = 40, TS = 18;
    sf::Color color;

    // Control keys
    sf::Keyboard::Key moveLeft;
    sf::Keyboard::Key moveRight;
    sf::Keyboard::Key moveUp;
    sf::Keyboard::Key moveDown;
};