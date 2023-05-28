#include "GameManager.h"
#include <iostream>
#include <thread>

GameManager::GameManager() : renderOffsetX(30), renderOffsetY(5), DELTA(200), tab_("level1.txt"), competitorsSystem_(SINGLE_PLAYER, 3, tab_), menu_(), rnd() {
    state_ = MENU_STATE;
    gameMode_ = SINGLE_PLAYER;
}

void GameManager::Start() {
    winner_ = NONE;
    competitorsSystem_ = CompetitorsSystem(gameMode_, 3, tab_);
    Update();
}

GameMode GameManager::getGameMode() {
    return gameMode_;
}

void GameManager::setGameMode(GameMode gM) {
    gameMode_ = gM;
}

void GameManager::Update() {
    state_ = PLAYING;
    while (state_ == PLAYING && gameMode_ != QUIT && winner_ == NONE) {
        competitorsSystem_.Update(tab_, gameMode_);
        tab_.Update(this, competitorsSystem_);
        winner_ = competitorsSystem_.CheckCollisions(tab_, renderOffsetX, renderOffsetY);
        Render();
        std::this_thread::sleep_for(std::chrono::milliseconds(DELTA));
    }
    ShowWinner();
    Menu();
}

void GameManager::Render() {
    system("clear");

    tab_.Render(renderOffsetX, renderOffsetY);
    competitorsSystem_.render(renderOffsetX, renderOffsetY);

    std::cout << "\033[0m"; // Reset console colors
}

void GameManager::ShowWinner() {
    if (winner_ != NONE) {
        std::cout << "\033[" << renderOffsetY - 1 << ";" << renderOffsetX + tab_.getWidth() - 5 << "H";
        std::cout << "\033[30;47m"; // White text on black background

        switch (winner_) {
            case PLAYER1:
            case PLAYER2:
                std::cout << "Player ";
                std::cout << "\033[" << 30 + (winner_ - 1) << "m"; // Set player color
                std::cout << "ºº";
                std::cout << "\033[0m"; // Reset console colors
                std::cout << " winner";
                break;
            case NONE:
            default:
                break;
        }

        std::cout << "\033[0m"; // Reset console colors
        std::cout << "\033[0;0H";

        std::this_thread::sleep_for(std::chrono::milliseconds(2000));
    }
}

void GameManager::Menu() {
    gameMode_ = menu_.Start();

    if (gameMode_ == CREATE) {
        menu_.CreateLevel(tab_, renderOffsetX, renderOffsetY);
        Menu();
    }

    std::string s = menu_.LevelMenu(tab_, gameMode_, renderOffsetX, renderOffsetY);
    Start();
}


