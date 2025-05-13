#pragma once
#include <SFML/Graphics.hpp>
#include <string>
#include <fstream>

struct Theme {
    std::string id;
    std::string name;
    std::string description;
    sf::Color menuBgColor;
    sf::Color textColor;
    sf::Color gridColor;
    sf::Color playerColor;

    // Comparison operators for AVL tree
    bool operator<(const Theme& other) const { return id < other.id; }
    bool operator>(const Theme& other) const { return id > other.id; }
    bool operator==(const Theme& other) const { return id == other.id; }
};

struct AVLNode {
    Theme data;
    AVLNode* left;
    AVLNode* right;
    int height;

    AVLNode(const Theme& theme)
        : data(theme), left(nullptr), right(nullptr), height(1) {}
};

class Inventory {
private:
    AVLNode* root;
    int nodeCount;

    // AVL Tree helper functions
    int getHeight(AVLNode* node);
    int getBalance(AVLNode* node);
    AVLNode* rightRotate(AVLNode* y);
    AVLNode* leftRotate(AVLNode* x);
    AVLNode* insert(AVLNode* node, const Theme& theme);
    AVLNode* minValueNode(AVLNode* node);
    void inOrderTraversal(AVLNode* node, Theme* arr, int& index);

    // Theme loading
    void loadThemesFromFile(const std::string& filename);
    Theme parseTheme(const std::string& line);

public:
    Inventory();
    ~Inventory();
    void clear(AVLNode* node);

    // Public interface
    void initialize(const std::string& filename);
    Theme* getAllThemes(); // Returns array of themes sorted by ID
    int getThemeCount() const { return nodeCount; }
    Theme* searchById(const std::string& id);
    Theme* searchByName(const std::string& name);

};