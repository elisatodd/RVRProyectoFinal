#include <iostream>
#include "GameUtils/GameManager.h"
#include "SDLUtils/Window.h"

int main(){

    GameManager::init();
    Window::init("TRON", 900, 700);

    GameManager().Start();

    return 0;
}