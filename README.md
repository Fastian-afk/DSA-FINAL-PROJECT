# 🎮 XONIX GAME – Data Structures Project (Spring 2025)

A dynamic 2D multiplayer Xonix-style game implemented in **C++** using **SFML**, built from scratch to demonstrate the real-world application of custom data structures. This project was created as part of the **Data Structures** course (Section A) under the supervision of **Sir Zahoor Hassan**.

---

## 🧩 Overview

This project extends the original Xonix game by integrating gameplay enhancements, backend systems, and social features—simulating a modern multiplayer game ecosystem.

### 📅 Submission Date: May 13, 2025  
### 💻 Language & Libraries:
- **Language**: C++
- **Libraries Used**: 
  - `<SFML/Graphics.hpp>`
  - `<time.h>`
  - `<cmath>`
  - `<cstdlib>`
  - `<string>`, `<cstring>`  
> ❗ _Note: No built-in data structures (e.g., vectors, stacks, queues) were used._

---

## 🚀 Features

### 🎮 Gameplay
- **Single & Multiplayer Modes**
  - Player 1: Arrow keys
  - Player 2: W, A, S, D
- **Dynamic Power-Up System**  
  Unlock boosts (freeze enemies) based on score milestones.
- **Interactive Game Board**
  - Collisions, shared spaces, and strategic tile claiming.
  
### 🗂️ Main Menu
- Start Game
- Select Level
- Leaderboard
- Theme Customization (e.g., "Fire" theme)
- Exit Game

### 👥 Login & Authentication
- **Secure Player Registration**
  - Unique usernames, password validation
- **Player Login**
  - Credential validation with file persistence

---

## 🧠 Data Structures Implemented

| Feature | Data Structure | Purpose |
|--------|----------------|---------|
| Game Grid | 2D Array | Real-time tile management |
| Friends System | Linked List | Store dynamic friend lists |
| Friend Lookup | Hash Table | O(1) username search |
| Leaderboard | Min Heap | Top 10 score tracking |
| Themes | AVL Tree | Sorted and efficient theme browsing |
| Matchmaking | Priority Queue | Competitive player matching |
| Save System | Linked List + File I/O | Resume saved games from exact point |

---

## 🧾 Additional Functionalities

### 🏆 Leaderboard
- Top 10 players sorted using **Min-Heap**
- Auto-updated after every match

### 👨‍👩‍👧‍👦 Friend System
- Send/Accept friend requests
- View friends list
- Search using username (via Hash Table)

### 🔐 Save & Resume Game
- Save current game state (grid, score, etc.) with timestamp
- Resume later using a unique Save ID

### 🎨 Theme Inventory
- Explore visual themes using **AVL Tree**
- In-order browsing and quick ID search
- Persist selected theme per user

---

## 🧪 How to Run

1. Clone the repo:
   ```bash
   git clone https://github.com/your-username/xonix-ds-project.git
2. Open the project folder in your C++ IDE.

3. Make sure SFML is installed and linked properly.

4. Compile the project.

5. Run the main.cpp or the executable.

💡 Use the save/resume, multiplayer, and theme options to explore the full experience.

🧗 Challenges Overcome

Theme Persistence Bug: Resolved file-saving issue across sessions.

Resource Sharing in Multiplayer: Used modular design to ensure game state consistency.

Efficient Leaderboard Management: Implemented heap-based logic for real-time rank updates.

📂 Folder Structure

/xonix-ds-project
│
├── main.cpp
├── Menu.cpp
├── Game.cpp
├── Player.cpp
├── FriendSystem.cpp
├── ThemeInventory.cpp
├── SaveSystem.cpp
├── Leaderboard.cpp
├── Assets/           # Sprites, audio, etc.
├── Data/             # Save files, user data
├── README.md
└── ...

🧠 Learning Outcomes

Mastered core data structures (Heap, Hash Table, AVL Tree, etc.)

Gained experience with GUI development using SFML

Developed modular, scalable game architecture

Learned real-world debugging, file handling, and team coordination

📌 Future Plans

Add network-based multiplayer

Implement cloud-based account persistence

Introduce profile customization & game achievements

📎 References

SFML Documentation
Base Game Repo


