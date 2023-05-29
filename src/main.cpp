#include <iostream>
#include "GameUtils/GameManager.h"
#include "SDLUtils/Window.h"

int main(){
    // GameManager* gM = new GameManager();
    // gM->Start();

    GameManager::init();
    Window::init("TRON", 900, 700);

    //delete gM;
    return 0;
}