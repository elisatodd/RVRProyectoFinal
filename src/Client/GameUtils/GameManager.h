#ifndef GAME_MANAGER_H
#define GAME_MANAGER_H

#include <random>
#include "CompetitorsSystem.h"

#include "EnumDefs.h"
#include "../../SDLUtils/Singleton.h"
#include "../../SDLUtils/Vector2D.h"
#include "Coor.h"

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
    void Update();
    GameMode getGameMode();
    void setGameMode(GameMode gM);
    Tablero* getTablero();

    Vector2D coorToRenderPos(Coor c);

    const int BOX_WIDTH = 14;
    const int RENDER_OFFSET_X = 190;
    const int RENDER_OFFSET_Y = 180;

private:
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
