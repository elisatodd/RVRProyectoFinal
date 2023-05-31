#include "GameManager.h"

#include "Tablero.h"
#include <iostream>
#include <thread>

GameManager::GameManager() : renderOffsetX(30), renderOffsetY(5), DELTA(200) {
    state_ = MENU_STATE;
    gameMode_ = SINGLE_PLAYER;
    //competitorsSystem_ = new CompetitorsSystem(SINGLE_PLAYER, 3, tab_);
}

GameManager::~GameManager()
{
    delete tab_;
}

void GameManager::Start() {
    winner_ = NONE;
    tab_ = new Tablero("assets/levels/level1.txt");
    competitorsSystem_ = new CompetitorsSystem(gameMode_, 3, tab_);
    std::cout << "[GM]: Reading Level Map\n";
    //Update();
}

GameMode GameManager::getGameMode() {
    return gameMode_;
}

void GameManager::setGameMode(GameMode gM) {
    gameMode_ = gM;
}


Tablero* GameManager::getTablero(){
    return tab_;
}

Vector2D GameManager::coorToRenderPos(Coor c){
    Vector2D renderPosition;
    renderPosition.setX(RENDER_OFFSET_X + (c.x * BOX_WIDTH));
    renderPosition.setY(RENDER_OFFSET_Y + (c.y * BOX_WIDTH));
    return renderPosition;
}

void GameManager::Update() {

    //competitorsSystem_->Update(tab_, gameMode_);
    // Hacer método update del tablero? creo que no es necesario
    //tab_->Update(this, competitorsSystem_);
    // winner_ = competitorsSystem_->CheckCollisions(tab_, renderOffsetX, renderOffsetY);
    // std::this_thread::sleep_for(std::chrono::milliseconds(DELTA));
    
    // ShowWinner();
}

void GameManager::ShowWinner() {
    // if (winner_ != NONE) {
    //     std::cout << "\033[" << renderOffsetY - 1 << ";" << renderOffsetX + tab_->getWidth() - 5 << "H";
    //     std::cout << "\033[30;47m"; // White text on black background

    //     switch (winner_) {
    //         case PLAYER1:
    //         case PLAYER2:
    //             std::cout << "Player ";
    //             std::cout << "\033[" << 30 + (winner_ - 1) << "m"; // Set player color
    //             std::cout << "ºº";
    //             std::cout << "\033[0m"; // Reset console colors
    //             std::cout << " winner";
    //             break;
    //         case NONE:
    //         default:
    //             break;
    //     }

    //     std::cout << "\033[0m"; // Reset console colors
    //     std::cout << "\033[0;0H";

    //     std::this_thread::sleep_for(std::chrono::milliseconds(2000));
    // }
     std::cout << "Tenemos ganador\n";
}

// void GameManager::Menu() {
//     gameMode_ = menu_.Start();

//     if (gameMode_ == CREATE) {
//         menu_.CreateLevel(tab_, renderOffsetX, renderOffsetY);
//         Menu();
//     }

//     std::string s = menu_.LevelMenu(tab_, gameMode_, renderOffsetX, renderOffsetY);
//     Start();
// }


