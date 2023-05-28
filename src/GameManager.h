#ifndef GAME_MANAGER_H
#define GAME_MANAGER_H

#include <random>
#include "CompetitorsSystem.h"

#include "EnumDefs.h"

class GameManager {
public:
    GameManager();
    ~GameManager();

    void Start();
    GameMode getGameMode();
    void setGameMode(GameMode gM);
    Tablero* getTablero();

private:
    void Update();
    void Render();
    void ShowWinner();

    State state_;
    GameMode gameMode_;
    Winner winner_;
    int renderOffsetX;
    int renderOffsetY;
    const int DELTA;

    Tablero* tab_;
    CompetitorsSystem* competitorsSystem_;
};

#endif
