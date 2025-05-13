#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <time.h>
#include "Menu.h"
#include "Game.h"
#include "Auth.h"
#include "FriendSystem.h"

using namespace std;
using namespace sf;

int main() {
    srand((unsigned)time(nullptr));
    const int TS = 18;
    const int M = 27, N = 40;

    RenderWindow window(VideoMode(N * TS, M * TS), "XONIX ENHANCED");
    window.setFramerateLimit(60);

    Music bgm;

    if (bgm.openFromFile("music/music.ogg")) {
        bgm.setLoop(true);
        bgm.play();
    }

    FriendSystem friendSystem; // Create FriendSystem instance

    bool mainReRun = false;
    string username = "";
    int enterCount = 0;

    const string mainIt[] = { "View Profile", "Friends", "Themes", "Single Player", "Multiplayer", "Exit" };
    Menu mainM((float)window.getSize().x, (float)window.getSize().y, mainIt, 6, &friendSystem);

    const string subIt[] = { "Start Game", "Select Level", "Leaderboard", "Instructions", "Back" };
    Menu subM((float)window.getSize().x, (float)window.getSize().y, subIt, 5, &friendSystem);

    const string lvlIt[] = { "Level 1", "Level 2", "Level 3", "Back" };
    Menu lvlM((float)window.getSize().x, (float)window.getSize().y, lvlIt, 4, &friendSystem);

    string currentUserTheme = "";  // Will store the theme name if found

    while (window.isOpen()) {
        bool multiMode = false;
        int selectedLevel = 1;

        if (!mainReRun) mainM.setState("welcome");
        else mainM.setState("main");

        mainReRun = false;
        bool running = true;

        while (running && window.isOpen()) {
            string state = mainM.getState();
            Event e;
            while (window.pollEvent(e)) {
                if (e.type == Event::Closed) return 0;
                if ((state == "login" || state == "register" || state == "profile" || state == "themes" || state == "friends") &&
                    (e.type == Event::TextEntered || e.type == Event::KeyReleased || e.type == Event::MouseButtonPressed)) {
                    mainM.handleInput(e);
                }

                if (e.type == Event::MouseButtonPressed)
                    mainM.handleInput(e);

                if (e.type == Event::KeyReleased) {
                    if (state == "welcome" && e.key.code == Keyboard::Enter) {
                        mainM.setState("login");
                    }
                    else if (state == "main") {
                        username = mainM.getName();

                        ifstream userFile("users.txt");
                        string line;

                        while (getline(userFile, line)) {
                            istringstream iss(line);
                            string id, uname, pass, theme;

                            // Parse the line
                            getline(iss, id, ':');
                            getline(iss, uname, ':');
                            getline(iss, pass, ':');

                            if (uname == username && getline(iss, theme)) {
                                currentUserTheme = theme;
                                break;
                            }
                        }
                        userFile.close();

                        if (!currentUserTheme.empty()) {
                            mainM.setThemeByName(currentUserTheme);
                            subM.setTheme(mainM.getCurrentTheme());
                            lvlM.setTheme(mainM.getCurrentTheme());
                        }

                        if (e.key.code == Keyboard::Up) mainM.up();
                        if (e.key.code == Keyboard::Down) mainM.down();
                        if (e.key.code == Keyboard::Enter) {
                            enterCount += 1;
                            int c = mainM.get();
                            if (c < 5 && enterCount > 1) {
                                if (c == 0)
                                    mainM.setState("profile");
                                else if (c == 1)
                                    mainM.setState("friends");
                                else if (c == 2)
                                    mainM.setState("themes");
                                else if (c > 2) {
                                    multiMode = (c == 4);
                                    running = false;
                                }
                            }
                            else if (c == 5)
                                return 0;
                        }
                    }
                }
            }

            if (state == "main" && mainM.getState() == "main" && mainM.getCurrentTheme()) {
                subM.setTheme(mainM.getCurrentTheme());
                lvlM.setTheme(mainM.getCurrentTheme());
            }

            window.clear();
            mainM.draw(window, username);
            window.display();
        }

        bool start = false;
        bool goBack = false;
        subM.setName(username);

        while (window.isOpen() && !start && !goBack) {
            string state = subM.getState();

            Event e;
            while (window.pollEvent(e)) {
                if (e.type == Event::Closed) return 0;
                if ((state == "instructionsL" || state == "instructionsM" || state == "leaderboard") && e.type == Event::TextEntered)
                    subM.handleInput(e);

                if (e.type == Event::KeyReleased) {
                    if (e.key.code == Keyboard::BackSpace && state == "leaderboard") {
                        subM.setState("main");
                        continue;
                    }
                    if (e.key.code == Keyboard::Up)    subM.up();
                    if (e.key.code == Keyboard::Down)  subM.down();
                    if (e.key.code == Keyboard::Enter) {
                        int c = subM.get();
                        if (c == 0) start = true;
                        else if (c == 1) {
                            bool chose = false;
                            while (window.isOpen() && !chose && !goBack) {
                                Event ev;
                                while (window.pollEvent(ev)) {
                                    if (ev.type == Event::Closed) return 0;
                                    if (ev.type == Event::KeyReleased) {
                                        if (ev.key.code == Keyboard::Up)   lvlM.up();
                                        if (ev.key.code == Keyboard::Down) lvlM.down();
                                        if (ev.key.code == Keyboard::Enter) {
                                            int l = lvlM.get();
                                            if (l < 3) {
                                                selectedLevel = l + 1;
                                                chose = true;
                                            }
                                            else goBack = true;
                                        }
                                    }
                                }
                                if (chose || goBack) break;
                                window.clear();
                                lvlM.draw(window, username);
                                window.display();
                            }
                        }
                        else if (c == 3) {
                            if (multiMode)
                                subM.setState("instructionsM");
                            else
                                subM.setState("instructionsL");
                        }
                        else if (c == 2) {
                            subM.setState("leaderboard");
                        }
                        else if (c == 4) {
                            goBack = true;
                        }
                    }
                }
            }
            if (start || goBack) break;
            window.clear();
            subM.draw(window, username);
            window.display();
        }

        if (goBack) {
            mainReRun = true;
            continue;
        }

        Game game(multiMode, selectedLevel, username, &subM, &friendSystem);
        bgm.pause();
        game.run(window);

        mainReRun = true;
    }

    return 0;
}