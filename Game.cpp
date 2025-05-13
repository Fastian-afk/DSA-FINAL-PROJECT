#include "Game.h"
#include "Player.h"
#include <cstdlib>
#include <iostream>
#include <cmath>
#include <string>
#include <SFML/Audio.hpp>

const int windowWidth = N * TS;
const int windowHeight = M * TS;

// Enemy constructor 
Game::Enemy::Enemy() {
    x = y = 300;
    dx = (rand() % 6 + 5) * (rand() % 2 ? 1 : -1);
    dy = (rand() % 6 + 5) * (rand() % 2 ? 1 : -1);
}

void Game::Enemy::move(int grid[M][N], int TS, bool powerUpActive) {
    if (powerUpActive) {
        return;
    }
    x += dx;
    if (grid[y / TS][x / TS] == 1) {
        dx = -dx;
        x += dx;
    }

    y += dy;
    int row = y / TS;

    if (row <= 2 || grid[row][x / TS] == 1) {
        dy = -dy;
        y += dy;

        if (y / TS <= 2) {
            y = TS * 2;
        }
    }

    if (grid[y / TS][x / TS] == 1) {
        dy = -dy;
        y += dy;
    }
}

Game::Game(bool multiplayer, int startLevel, const std::string& playerName, Menu* menu, FriendSystem* friendSystem)
    : multiMode(multiplayer), level(startLevel), playerName(playerName), menu(menu), friendSystem(friendSystem),
    score(0), dropCount(0), gameOver(false), selectedOption(0),
    gameTimer(0), maxTime(60), timerActive(true),
    bonusOccurrences(0), currentThreshold(10), currentMultiplier(2.0f), prevDropCount(0), isPaused(false), gameWon(false),
    powerUpCount(0), powerUpTimer(0), powerUpActive(false), powerUpAvailable(false) {

    setupLevel(level, false);

    if (!texEnemy.loadFromFile("enemy.png") ||
        !texGameover.loadFromFile("gameover.png") ||
        !texTiles.loadFromFile("tiles.png")) {
        std::cerr << "Failed to load textures\n";
    }

    sEnemy.setTexture(texEnemy);
    sGameover.setTexture(texGameover);
    sTile.setTexture(texTiles);

    sEnemy.setOrigin(20, 20);
    sGameover.setPosition(100, 50);

    if (!titleFont.loadFromFile("fonts/PressStart2P-Regular.ttf")) {
        if (!titleFont.loadFromFile("C:/WINDOWS/FONTS/IMPACT.TTF")) {
            std::cerr << "Failed to load title font\n";
        }
    }

    if (!uiFont.loadFromFile("fonts/Roboto-VariableFont_wdth,wght.ttf")) {
        if (!uiFont.loadFromFile("arial.ttf")) {
            std::cerr << "Failed to load UI font\n";
        }
    }
    if (levelUpBuffer.loadFromFile("music/levelUp.wav")) {
        levelUpSound.setBuffer(levelUpBuffer);
    }

    if (!gameOverBuffer.loadFromFile("music/gameOver.wav")) {
        std::cerr << "Failed to load gameover.wav" << std::endl;
    }
    gameOverSound.setBuffer(gameOverBuffer);

    setupHeaderUI();
    setupPauseUI();
}

Game::~Game() {
}

void Game::setupHeaderUI() {
    headerBg.setSize(sf::Vector2f(N * TS, 35));
    headerBg.setFillColor(sf::Color(0, 0, 80, 220));
    headerBg.setPosition(0, 0);

    levelText.setFont(titleFont);
    levelText.setCharacterSize(13);
    levelText.setFillColor(sf::Color::White);
    levelText.setString("Level:" + std::to_string(level));
    levelText.setPosition(9, 12);

    player1Text.setFont(titleFont);
    player1Text.setCharacterSize(11);
    player1Text.setFillColor(sf::Color::Yellow);
    player1Text.setString("P1: " + playerName);
    player1Text.setPosition(135, 13);

    scoreText.setFont(titleFont);
    scoreText.setCharacterSize(12);
    scoreText.setFillColor(sf::Color::White);
    scoreText.setString("Score: 0");
    scoreText.setPosition(300, 13);

    powerupText.setFont(titleFont);
    powerupText.setCharacterSize(9);
    powerupText.setFillColor(sf::Color::Green);
    powerupText.setString("Power-ups: Off");
    powerupText.setPosition(455, 7);

    timerText.setFont(titleFont);
    timerText.setCharacterSize(11);
    timerText.setFillColor(sf::Color::White);
    timerText.setString("Time: " + std::to_string(static_cast<int>(gameTimer)));
    timerText.setPosition(625, 14);

    bonusText.setFont(titleFont);
    bonusText.setCharacterSize(9);
    bonusText.setFillColor(sf::Color::Cyan);
    bonusText.setString("Bonus: 10x" + std::to_string(static_cast<int>(currentMultiplier)));
    bonusText.setPosition(455, 20);

    if (multiMode) {
        player1Text.setPosition(135, 8);
        player1Text.setCharacterSize(9);
        player1Text.setString("P1: " + playerName);

        player2Text.setFont(titleFont);
        player2Text.setCharacterSize(9);
        player2Text.setFillColor(sf::Color(255, 100, 100));
        player2Text.setString("P2: Player 2");
        player2Text.setPosition(135, 21);
    }
}

void Game::setupPauseUI() {
    pauseOverlay.setSize(sf::Vector2f(N * TS, M * TS));
    pauseOverlay.setFillColor(sf::Color(0, 0, 0, 150));
    pauseOverlay.setPosition(0, 0);

    pauseText.setFont(titleFont);
    pauseText.setString("PAUSED");
    pauseText.setCharacterSize(36);
    pauseText.setFillColor(sf::Color::Yellow);
    pauseText.setOutlineColor(sf::Color(255, 165, 0));
    pauseText.setOutlineThickness(2.0f);

    sf::FloatRect textRect = pauseText.getLocalBounds();
    pauseText.setOrigin(textRect.left + textRect.width / 2.0f,
        textRect.top + textRect.height / 2.0f);
    pauseText.setPosition(N * TS / 2.0f, M * TS / 2.0f - 20);

    pauseInstructions.setFont(uiFont);
    pauseInstructions.setString("Press P to resume");
    pauseInstructions.setCharacterSize(20);
    pauseInstructions.setFillColor(sf::Color::White);

    sf::FloatRect instrRect = pauseInstructions.getLocalBounds();
    pauseInstructions.setOrigin(instrRect.left + instrRect.width / 2.0f,
        instrRect.top + instrRect.height / 2.0f);
    pauseInstructions.setPosition(N * TS / 2.0f, M * TS / 2.0f + 30);
}

void Game::setupLevel(int lvl, bool levelUp = false) {
    level = lvl;
    enemyCount = 3 + (lvl - 1);
    gameOver = false;
    gameWon = false;
    lastPowerUpScore = 0;
    powerUpCount = 0;
    powerUpActive = false;
    powerUpAvailable = false;
    powerUpTimer = 0;
    powerupText.setString("Power-ups: None");

    if (!levelUp) {
        score = 0;
        levelText.setString("Level:" + std::to_string(level));

    }
    prevDropCount = 0;
    selectedOption = 0;
    gameTimer = maxTime;
    timerActive = true;
    isPaused = false;

    // Reset bonus system
    bonusOccurrences = 0;
    currentThreshold = 10;
    currentMultiplier = 2.0f;
    bonusText.setString("Bonus: 10x" + std::to_string(static_cast<int>(currentMultiplier)));

    for (int i = 0; i < M; i++) {
        for (int j = 0; j < N; j++) {
            grid[i][j] = (i == 0 || j == 0 || i == M - 1 || j == N - 1) ? 1 : 0;
        }
    }

    for (int i = 0; i < enemyCount; i++) {
        enemies[i] = Enemy();
    }

    p1x = 10; p1y = 0; p1dx = p1dy = 0;
    if (multiMode) {
        p2x = N - 10; p2y = M - 1; p2dx = p2dy = 0;
    }

    if (levelUp) {
        levelUpSound.play();
    }
}

int Game::drop(int y, int x) {
    if (grid[y][x] == 0) {
        grid[y][x] = -1;
        return 1 + drop(y - 1, x) + drop(y + 1, x) + drop(y, x - 1) + drop(y, x + 1);
    }
    return 0;
}

void Game::togglePause() {
    isPaused = !isPaused;
    if (isPaused) {
        savedP1dx = p1dx; savedP1dy = p1dy;
        p1dx = p1dy = 0;
        if (multiMode) {
            savedP2dx = p2dx; savedP2dy = p2dy;
            p2dx = p2dy = 0;
        }
    }
    else {
        p1dx = savedP1dx; p1dy = savedP1dy;
        if (multiMode) {
            p2dx = savedP2dx; p2dy = savedP2dy;
        }
    }
}

void Game::handlePowerUp() {
    // Determine if a new threshold is crossed
    int nextThreshold = 500 + 1000 * powerUpCount;

    if (score >= nextThreshold && lastPowerUpScore < nextThreshold) {
        powerUpAvailable = true;
        lastPowerUpScore = nextThreshold;
        powerUpCount++;  // only increment when we actually grant it
        powerupText.setString("Power-up: Ready!");
    }

    // Handle activation
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space)) {
        if (powerUpAvailable && !powerUpActive) {
            powerUpActive = true;
            powerUpAvailable = false;
            powerUpTimer = 3.0f;
            powerUpClock.restart();
            powerupText.setString("Power-up: Active!");
        }
    }

    // Handle timer
    if (powerUpActive) {
        powerUpTimer -= powerUpClock.restart().asSeconds();
        if (powerUpTimer <= 0) {
            powerUpActive = false;
            powerupText.setString("Power-up: Off");
        }
    }
}

void Game::handleInput() {
    if (gameOver) return;

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) { p1dx = -1; p1dy = 0; }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) { p1dx = 1; p1dy = 0; }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) { p1dx = 0; p1dy = -1; }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) { p1dx = 0; p1dy = 1; }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space) && powerUpCount > 0 && !powerUpActive) {
        handlePowerUp();
    }

    if (multiMode) {
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) { p2dx = -1; p2dy = 0; }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) { p2dx = 1; p2dy = 0; }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) { p2dx = 0; p2dy = -1; }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) { p2dx = 0; p2dy = 1; }
    }
}

void Game::update(float dt) {
    if (gameOver || isPaused) return;

    if (timerActive) {
        gameTimer -= 0.1;
        timerText.setString("Time: " + std::to_string(static_cast<int>(gameTimer)));

        if (gameTimer <= 10.0f) {
            timerText.setFillColor(sf::Color::Red);
        }
        else {
            timerText.setFillColor(sf::Color::White);
        }

        if (gameTimer <= 0) {
            gameTimer = 0;
            timerActive = false;
            gameOverSound.play();
            gameOver = true;
            return;
        }
    }

    p1x += p1dx; p1y += p1dy;
    p1x = std::max(0, std::min(N - 1, p1x));
    p1y = std::max(0, std::min(M - 1, p1y));

    if (grid[p1y][p1x] == GRID_EMPTY) grid[p1y][p1x] = GRID_P1_TRAIL;

    if (multiMode) {
        p2x += p2dx; p2y += p2dy;
        p2x = std::max(0, std::min(N - 1, p2x));
        p2y = std::max(0, std::min(M - 1, p2y));
        if (grid[p2y][p2x] == GRID_P1_TRAIL)
        {
            gameOverSound.play();
            gameOver = true;
        }
        if (grid[p2y][p2x] == GRID_EMPTY) grid[p2y][p2x] = GRID_P2_TRAIL;
    }

    for (int i = 0; i < enemyCount; i++) {
        enemies[i].move(grid, TS, powerUpActive);
    }

    if (grid[p1y][p1x] == GRID_WALL || (multiMode && grid[p2y][p2x] == GRID_WALL)) {
        p1dx = p1dy = 0;
        if (multiMode) p2dx = p2dy = 0;

        for (int i = 0; i < enemyCount; i++) {
            drop(enemies[i].y / TS, enemies[i].x / TS);
        }

        // Convert filled areas
        for (int i = 0; i < M; i++) {
            for (int j = 0; j < N; j++) {
                if (grid[i][j] == -1) grid[i][j] = GRID_EMPTY;
                else if (grid[i][j] == GRID_EMPTY) grid[i][j] = GRID_WALL;
                else if (grid[i][j] == GRID_P1_TRAIL || grid[i][j] == GRID_P2_TRAIL) {
                    grid[i][j] = GRID_WALL;
                }
            }
        }

        // handle powerup

        handlePowerUp();


        // Update score based on claimed area  
        int dropCount = 0;
        int totalCells = (M - 2) * (N - 2); // Total cells excluding border

        // Count the number of claimed cells
        for (int i = 1; i < M - 1; i++) {
            for (int j = 1; j < N - 1; j++) {
                if (grid[i][j] == 1) dropCount++;
            }
        }


        int currentDropCount = dropCount - prevDropCount;


        // New scoring system
        if (currentDropCount >= currentThreshold) {
            score += static_cast<int>(currentDropCount * currentMultiplier);
            bonusOccurrences++;

            if (bonusOccurrences >= 5) {
                currentThreshold = 5;
                currentMultiplier = 4.0f;
            }
            else if (bonusOccurrences >= 3) {
                currentThreshold = 5;
                currentMultiplier = 2.0f;
            }

            bonusText.setString("Bonus: " + std::to_string(currentThreshold) +
                "x" + std::to_string(static_cast<int>(currentMultiplier)));
        }
        else {
            score += currentDropCount;
        }

        scoreText.setString("Score: " + std::to_string(score));

        float fillPercentage = (float)dropCount / totalCells * 100;
        if (fillPercentage >= 70) {
            if (level == 3) {
                levelUpSound.play();
                gameOver = true;
                return;
            }
            prevDropCount = 0;
            dropCount = 0;
            level++;
            levelText.setString("Level:" + std::to_string(level));

            setupLevel(level, true);
        }

        prevDropCount = dropCount;

    }

    for (int i = 0; i < enemyCount; i++) {
        int enemyGridY = enemies[i].y / TS;
        int enemyGridX = enemies[i].x / TS;

        if (grid[enemyGridY][enemyGridX] == GRID_P1_TRAIL ||
            grid[enemyGridY][enemyGridX] == GRID_P2_TRAIL) {
            gameOverSound.play();
            gameOver = true;
        }

        if ((enemyGridX == p1x && enemyGridY == p1y) ||
            (multiMode && (enemyGridX == p2x && enemyGridY == p2y))) {
            gameOverSound.play();
            gameOver = true;
        }
    }
}

void Game::render(sf::RenderWindow& window) {
    window.clear();

    if (!gameOver) {
        for (int i = 0; i < M; i++) {
            for (int j = 0; j < N; j++) {
                if (grid[i][j] == GRID_EMPTY) continue;

                if (grid[i][j] == GRID_WALL) {
                    sTile.setTextureRect(sf::IntRect(0, 0, TS, TS));
                }
                else if (grid[i][j] == GRID_P1_TRAIL) {
                    sTile.setTextureRect(sf::IntRect(54, 0, TS, TS));
                    sTile.setColor(sf::Color::White);
                }
                else if (grid[i][j] == GRID_P2_TRAIL) {
                    sTile.setTextureRect(sf::IntRect(54, 0, TS, TS));
                    sTile.setColor(sf::Color(255, 100, 100));
                }

                sTile.setPosition(j * TS, (i + 2) * TS);
                window.draw(sTile);
            }
        }

        sTile.setTextureRect(sf::IntRect(36, 0, TS, TS));
        sTile.setColor(sf::Color::White);
        sTile.setPosition(p1x * TS, (p1y + 2) * TS);
        window.draw(sTile);

        if (multiMode) {
            sTile.setColor(sf::Color(255, 100, 100));
            sTile.setPosition(p2x * TS, (p2y + 2) * TS);
            window.draw(sTile);
        }

        if (!isPaused) {
            sEnemy.rotate(10);
        }

        for (int i = 0; i < enemyCount; i++) {
            sEnemy.setPosition(enemies[i].x, enemies[i].y);
            window.draw(sEnemy);
        }

        window.draw(headerBg);
        window.draw(levelText);
        window.draw(player1Text);
        window.draw(scoreText);
        window.draw(powerupText);
        window.draw(timerText);
        window.draw(bonusText);

        if (multiMode) {
            window.draw(player2Text);
        }

        if (isPaused) {
            window.draw(pauseOverlay);
            window.draw(pauseText);
            window.draw(pauseInstructions);
        }
    }
    else {
        window.clear();

        sGameover.setPosition(window.getSize().x / 2 - sGameover.getGlobalBounds().width / 2, 0);
        window.draw(sGameover);

        sf::Text scoreDisplay;
        scoreDisplay.setFont(uiFont);
        scoreDisplay.setString("Score: " + std::to_string(score));
        scoreDisplay.setCharacterSize(22);
        scoreDisplay.setFillColor(sf::Color::White);
        scoreDisplay.setPosition(window.getSize().x / 2 - scoreDisplay.getGlobalBounds().width / 2, 160);
        window.draw(scoreDisplay);

        sf::Text levelDisplay;
        levelDisplay.setFont(uiFont);
        levelDisplay.setString("Level Reached: " + std::to_string(level));
        levelDisplay.setCharacterSize(22);
        levelDisplay.setFillColor(sf::Color::White);
        levelDisplay.setPosition(window.getSize().x / 2 - levelDisplay.getGlobalBounds().width / 2, 200);
        window.draw(levelDisplay);

        sf::Text timeDisplay;
        timeDisplay.setFont(uiFont);
        timeDisplay.setString("Time Remaining: " + std::to_string(static_cast<int>(gameTimer)));
        timeDisplay.setCharacterSize(22);
        timeDisplay.setFillColor(sf::Color::White);
        timeDisplay.setPosition(window.getSize().x / 2 - timeDisplay.getGlobalBounds().width / 2, 240);
        window.draw(timeDisplay);

        const std::string options[] = { "Restart", "Main Menu", "Exit Game" };
        float startY = 320;

        for (int i = 0; i < 3; i++) {
            sf::Text option;
            option.setFont(uiFont);
            option.setString(options[i]);
            option.setCharacterSize(28);
            option.setFillColor(i == selectedOption ? sf::Color::Yellow : sf::Color::White);
            option.setPosition(window.getSize().x / 2 - option.getGlobalBounds().width / 2,
                startY + i * 50);
            window.draw(option);
        }
    }

    window.display();
}

void Game::updateLeaderboard() {
    if (menu && !multiMode) {
        menu->updateLeaderboard(playerName, score);
    }
}

void Game::run(sf::RenderWindow& window) {
    sf::Clock clock;
    float timer = 0, delay = 0.07f;
    bool pKeyWasPressed = false;

    while (window.isOpen()) {
        float dt = clock.restart().asSeconds();
        if (!isPaused) {
            timer += dt;
        }

        sf::Event e;
        while (window.pollEvent(e)) {
            if (e.type == sf::Event::Closed) window.close();
            if (e.type == sf::Event::KeyReleased && e.key.code == sf::Keyboard::Escape) {
                friendSystem->updateGameStats(playerName, score, gameWon);
                window.close();
                return;
            }

            if (e.type == sf::Event::KeyReleased && e.key.code == sf::Keyboard::P && !gameOver) {
                togglePause();
            }

            if (gameOver) {
                if (e.type == sf::Event::KeyReleased) {
                    if (e.key.code == sf::Keyboard::Up) {
                        selectedOption = (selectedOption - 1 + 3) % 3;
                    }
                    else if (e.key.code == sf::Keyboard::Down) {
                        selectedOption = (selectedOption + 1) % 3;
                    }
                    else if (e.key.code == sf::Keyboard::Enter) {
                        friendSystem->updateGameStats(playerName, score, gameWon);
                        if (selectedOption == 0) {
                            level = 1;
                            setupLevel(1);
                            gameOver = false;
                        }
                        else if (selectedOption == 1) {
                            updateLeaderboard();
                            return;
                        }
                        else if (selectedOption == 2) {
                            updateLeaderboard();
                            window.close();
                        }
                    }
                }
            }
            else {
                if (e.type == sf::Event::KeyPressed && e.key.code == sf::Keyboard::Escape) {
                    setupLevel(level, false);
                }
            }
        }

        if (!gameOver && !isPaused) {
            handleInput();
            if (timer > delay) {
                update(dt);
                timer = 0;
            }
        }
        render(window);
    }
}