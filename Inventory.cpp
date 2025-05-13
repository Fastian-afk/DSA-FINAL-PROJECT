#include "Inventory.h"
#include <iostream>

Inventory::Inventory() : root(nullptr), nodeCount(0) {}

Inventory::~Inventory() {
    clear(root);
}

void Inventory::clear(AVLNode* node) {
    if (node) {
        clear(node->left);
        clear(node->right);
        delete node;
    }
}

int Inventory::getHeight(AVLNode* node) {
    return node ? node->height : 0;
}

int Inventory::getBalance(AVLNode* node) {
    return node ? getHeight(node->left) - getHeight(node->right) : 0;
}

AVLNode* Inventory::rightRotate(AVLNode* y) {
    AVLNode* x = y->left;
    AVLNode* T2 = x->right;

    x->right = y;
    y->left = T2;

    y->height = std::max(getHeight(y->left), getHeight(y->right)) + 1;
    x->height = std::max(getHeight(x->left), getHeight(x->right)) + 1;

    return x;
}

AVLNode* Inventory::leftRotate(AVLNode* x) {
    AVLNode* y = x->right;
    AVLNode* T2 = y->left;

    y->left = x;
    x->right = T2;

    x->height = std::max(getHeight(x->left), getHeight(x->right)) + 1;
    y->height = std::max(getHeight(y->left), getHeight(y->right)) + 1;

    return y;
}

AVLNode* Inventory::insert(AVLNode* node, const Theme& theme) {
    if (!node) {
        nodeCount++;
        return new AVLNode(theme);
    }

    if (theme < node->data) {
        node->left = insert(node->left, theme);
    }
    else if (theme > node->data) {
        node->right = insert(node->right, theme);
    }
    else {
        return node; // Duplicate IDs not allowed
    }

    node->height = 1 + std::max(getHeight(node->left), getHeight(node->right));

    int balance = getBalance(node);

    // Left Left Case
    if (balance > 1 && theme < node->left->data)
        return rightRotate(node);

    // Right Right Case
    if (balance < -1 && theme > node->right->data)
        return leftRotate(node);

    // Left Right Case
    if (balance > 1 && theme > node->left->data) {
        node->left = leftRotate(node->left);
        return rightRotate(node);
    }

    // Right Left Case
    if (balance < -1 && theme < node->right->data) {
        node->right = rightRotate(node->right);
        return leftRotate(node);
    }

    return node;
}

void Inventory::inOrderTraversal(AVLNode* node, Theme* arr, int& index) {
    if (node) {
        inOrderTraversal(node->left, arr, index);
        arr[index++] = node->data;
        inOrderTraversal(node->right, arr, index);
    }
}

Theme Inventory::parseTheme(const std::string& line) {
    size_t pos1 = line.find(':');
    size_t pos2 = line.find(':', pos1 + 1);

    Theme theme;
    theme.id = line.substr(0, pos1);
    theme.name = line.substr(pos1 + 1, pos2 - pos1 - 1);
    theme.description = line.substr(pos2 + 1);

    // Assign colors based on theme ID (simple mapping)
    int idNum = std::stoi(theme.id);
    switch (idNum % 5) {
    case 0: // Classic
        theme.menuBgColor = sf::Color(0, 0, 0);
        theme.textColor = sf::Color(255, 255, 255);
        theme.gridColor = sf::Color(0, 0, 255);
        theme.playerColor = sf::Color(255, 0, 0);
        break;
    case 1: // Ocean
        theme.menuBgColor = sf::Color(0, 0, 100);
        theme.textColor = sf::Color(0, 255, 255);
        theme.gridColor = sf::Color(0, 0, 150);
        theme.playerColor = sf::Color(0, 255, 0);
        break;
    case 2: // Forest
        theme.menuBgColor = sf::Color(0, 50, 0);
        theme.textColor = sf::Color(0, 255, 0);
        theme.gridColor = sf::Color(0, 100, 0);
        theme.playerColor = sf::Color(255, 255, 0);
        break;
    case 3: // Fire
        theme.menuBgColor = sf::Color(50, 0, 0);
        theme.textColor = sf::Color(255, 165, 0);
        theme.gridColor = sf::Color(100, 0, 0);
        theme.playerColor = sf::Color(255, 255, 255);
        break;
    case 4: // Cyber
        theme.menuBgColor = sf::Color(50, 0, 50);
        theme.textColor = sf::Color(255, 0, 255);
        theme.gridColor = sf::Color(100, 0, 100);
        theme.playerColor = sf::Color(0, 255, 255);
        break;
    }

    return theme;
}

void Inventory::loadThemesFromFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Failed to open themes file: " << filename << std::endl;
        return;
    }

    std::string line;
    while (std::getline(file, line)) {
        if (!line.empty()) {
            Theme theme = parseTheme(line);
            root = insert(root, theme);
        }
    }
    file.close();
}

void Inventory::initialize(const std::string& filename) {
    clear(root);
    root = nullptr;
    nodeCount = 0;
    loadThemesFromFile(filename);
}

Theme* Inventory::getAllThemes() {
    Theme* themes = new Theme[nodeCount];
    int index = 0;
    inOrderTraversal(root, themes, index);
    return themes;
}

Theme* Inventory::searchById(const std::string& id) {
    AVLNode* current = root;
    Theme temp;
    temp.id = id;

    while (current) {
        if (temp < current->data) {
            current = current->left;
        }
        else if (temp > current->data) {
            current = current->right;
        }
        else {
            return &(current->data);
        }
    }
    return nullptr;
}

Theme* Inventory::searchByName(const std::string& name) {
    Theme* allThemes = getAllThemes();
    for (int i = 0; i < nodeCount; i++) {
        if (allThemes[i].name == name) {
            Theme* result = new Theme(allThemes[i]);
            delete[] allThemes;
            return result;
        }
    }
    delete[] allThemes;
    return nullptr;
}

AVLNode* Inventory::minValueNode(AVLNode* node) {
    AVLNode* current = node;
    while (current && current->left) {
        current = current->left;
    }
    return current;
}