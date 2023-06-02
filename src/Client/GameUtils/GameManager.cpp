#include "GameManager.h"

#include <iostream>
#include <thread>

GameManager::GameManager() : renderOffsetX(30), renderOffsetY(5) {
}

GameManager::~GameManager()
{
    delete tab_;
}

void GameManager::Start() {
    tab_ = new Tablero("assets/levels/level1.txt");
    std::cout << "[GM]: Reading Level Map\n";
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