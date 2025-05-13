#pragma once
#ifndef SCOREDISPLAY_H
#define SCOREDISPLAY_H

#include <SFML/Graphics.hpp>
#include <string>

class ScoreDisplay {
private:
    sf::Font font;
    sf::Text player1Text;
    sf::Text player2Text;
    sf::Text levelText;
    bool fontLoaded;

public:
    ScoreDisplay();
    bool loadFont(const std::string& fontPath = "arial.ttf");
    const sf::Font* getFont() const { return &font; }

    void update(int p1Score, int p2Score, int level, bool gameOver, bool multiplayer);
    void draw(sf::RenderWindow& window);
};

#endif