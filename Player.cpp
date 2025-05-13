#include "Player.h"
#include <iostream>
#include <cmath>
using namespace std;

Player::Player(int startX, int startY, sf::Keyboard::Key left, sf::Keyboard::Key right,
    sf::Keyboard::Key up, sf::Keyboard::Key down, sf::Color color) :
    x(startX), y(startY), dx(0), dy(0), score(0), alive(true), color(color),
    moveLeft(left), moveRight(right), moveUp(up), moveDown(down) {}

void Player::update() {
    // Handle movement input
    if (sf::Keyboard::isKeyPressed(moveUp))    dy = -1;
    if (sf::Keyboard::isKeyPressed(moveDown))  dy = 1;
    if (sf::Keyboard::isKeyPressed(moveLeft))  dx = -1;
    if (sf::Keyboard::isKeyPressed(moveRight)) dx = 1;

    //cout << x << endl;
    // Apply movement
    x += dx;
    y += dy;

    // Reset movement direction after applying
    dx = dy = 0;
}

void Player::reset(int newX, int newY) {
    x = newX;
    y = newY;
    dx = dy = 0;
    alive = true;
}

void Player::handleInput() {
    if (sf::Keyboard::isKeyPressed(moveLeft)) { dx = -1; dy = 0; }
    if (sf::Keyboard::isKeyPressed(moveRight)) { dx = 1; dy = 0; }
    if (sf::Keyboard::isKeyPressed(moveUp)) { dx = 0; dy = -1; }
    if (sf::Keyboard::isKeyPressed(moveDown)) { dx = 0; dy = 1; }
}

void Player::draw(sf::RenderWindow& window, const sf::Sprite& tileSprite) const {
    sf::Sprite playerSprite = tileSprite;
    playerSprite.setTextureRect(sf::IntRect(36, 0, TS, TS));
    playerSprite.setColor(color);
    playerSprite.setPosition(x * TS, y * TS);
    window.draw(playerSprite);
}

void Player::setPosition(int newX, int newY) {
    x = max(0, min(N - 1, newX));
    y = max(0, min(M - 1, newY));
}

void Player::stop() {
    dx = 0;
    dy = 0;
}