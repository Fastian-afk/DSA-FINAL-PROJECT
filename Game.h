#pragma once
#include <SFML/Graphics.hpp>
#include "Menu.h"
#include "FriendSystem.h"
#include <SFML/Audio.hpp>

const int M = 25; // Grid rows
const int N = 40; // Grid columns
const int TS = 18; // Tile size

// Grid state constants
const int GRID_EMPTY = 0;
const int GRID_WALL = 1;
const int GRID_P1_TRAIL = 2;
const int GRID_P2_TRAIL = 3;

class Game {
public:
    // Enemy structure
    struct Enemy {
        int x, y, dx, dy;  // Position and velocity
        Enemy();           // Constructor
        void move(int grid[M][N], int TS, bool powerUpActive);  // Movement logic
    };

    // Constructor
    Game(bool multiplayer, int startLevel, const std::string& playerName, Menu* menu, FriendSystem* friendSystem);

    // Main game loop
    void run(sf::RenderWindow& window);

    // Destructor
    ~Game();

private:
    // Game initialization
    void setupLevel(int lvl, bool levelUp);      // Set up level with given number
    void setupHeaderUI();          // Initialize UI elements
    void setupPauseUI();

    // Game logic
    int drop(int y, int x);        // Flood fill algorithm
    void handleInput();            // Process player input
    void update(float dt);         // Update game state
    void render(sf::RenderWindow& window); // Render everything
    void updateLeaderboard();
    void togglePause();
    void handlePowerUp();
    // Player data
    int p1x, p1y, p1dx, p1dy;     // Player 1 position and velocity
    int p2x, p2y, p2dx, p2dy;     // Player 2 position and velocity (for multiplayer)

    // Saved movement variables for pause/resume
    int savedP1dx = 0, savedP1dy = 0;
    int savedP2dx = 0, savedP2dy = 0;


    // Game state
    int grid[M][N];                // Game grid
    int level;                     // Current level
    int score;                     // Player score
    int dropCount;                 // Count of captured tiles
    int enemyCount;                // Number of active enemies
    bool gameOver;                 // Game over flag
    bool multiMode;                // Multiplayer mode flag
    std::string playerName;        // Player name for leaderboard
    int selectedOption;            // Selected menu option

    // Scoring system variables
    int bonusOccurrences;          // Tracks how many times bonus has been earned
    int currentThreshold;          // Current threshold for bonus (starts at 10)
    float currentMultiplier;       // Current points multiplier (starts at 2.0)
    int prevDropCount;

    int powerUpCount;
    float powerUpTimer;
    bool powerUpAvailable;
    bool powerUpActive;
    int lastPowerUpScore = 0;
    sf::Clock powerUpClock;

    // Timer system
    float gameTimer;               // Current game time
    float maxTime;                 // Maximum time per level
    bool timerActive;              // Whether timer is running
    bool isPaused;
    bool gameWon; // Track if the game was won

    // Enemies
    Enemy enemies[20];             // Array of enemies

    // Graphics resources
    sf::Texture texEnemy;          // Enemy texture
    sf::Texture texGameover;       // Game over texture
    sf::Texture texTiles;          // Tile texture
    sf::Sprite sEnemy;             // Enemy sprite
    sf::Sprite sGameover;          // Game over sprite
    sf::Sprite sTile;              // Tile sprite

    // Fonts
    sf::Font titleFont;            // For headings and titles
    sf::Font uiFont;               // For UI elements and text

    // UI Header elements
    sf::RectangleShape headerBg;   // Background for header
    sf::Text levelText;            // Level display text
    sf::Text player1Text;          // Player 1 info text
    sf::Text player2Text;          // Player 2 info text (multiplayer)
    sf::Text scoreText;            // Score display text
    sf::Text powerupText;          // Power-up status text
    sf::Text timerText;            // Timer display text
    sf::Text bonusText;            // Bonus status text

    // Pause UI elements
    sf::RectangleShape pauseOverlay;
    sf::Text pauseText;
    sf::Text pauseInstructions;

    sf::SoundBuffer levelUpBuffer;
    sf::Sound levelUpSound;
    sf::SoundBuffer gameOverBuffer;
    sf::Sound gameOverSound;

    Menu* menu;
    FriendSystem* friendSystem; // Pointer to FriendSystem to update stats
};