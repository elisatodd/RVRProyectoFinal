#ifndef GAME_MANAGER_H
#define GAME_MANAGER_H

#include <random>
#include "CompetitorsSystem.h"

#include "EnumDefs.h"
#include "../SDLUtils/Singleton.h"

class GameManager : public Singleton<GameManager>{

	friend Singleton<GameManager>;

public:
    GameManager();
    virtual ~GameManager();

    GameManager(GameManager&) = delete;
	GameManager(GameManager&&) = delete;
	GameManager& operator=(GameManager&) = delete;
	GameManager& operator=(GameManager&&) = delete;

    void Start();
    GameMode getGameMode();
    void setGameMode(GameMode gM);
    Tablero* getTablero();

private:
    void Update();
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
inline GameManager& GameManager() {
	return *GameManager::instance();
}

#endif
