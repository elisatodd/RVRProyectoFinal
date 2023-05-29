#include <iostream>
#include "GameUtils/GameManager.h"

int main(){
    GameManager* gM = new GameManager();
    gM->Start();
    delete gM;
    return 0;
}