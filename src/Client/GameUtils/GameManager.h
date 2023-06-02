#ifndef GAME_MANAGER_H
#define GAME_MANAGER_H

#include <random>

#include "EnumDefs.h"
#include "../../SDLUtils/Singleton.h"
#include "../../SDLUtils/Vector2D.h"

#include "Coor.h"
#include "Tablero.h"

class Player;
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

    Tablero* getTablero();

    Vector2D coorToRenderPos(Coor c);

    const int BOX_WIDTH = 10;
    const int RENDER_OFFSET_X = 190;
    const int RENDER_OFFSET_Y = 180;

private:

    int renderOffsetX;
    int renderOffsetY;

    Tablero* tab_;
};
inline GameManager& GameManager() {
	return *GameManager::instance();
}

#endif
