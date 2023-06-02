#ifndef PLAYER_H
#define PLAYER_H

#include <vector>
#include <string>
#include <random>

#include "Coor.h"
#include "ListaCoor.h"
#include "EnumDefs.h"
#include "../../SDLUtils/GameObject.h"

class Tablero;
class CompetitorsSystem;

enum Directions { UP = 0, DOWN, RIGHT, LEFT };

class Player : public GameObject {
public:
    Player(Coor c);
    virtual ~Player() {}; 

    void playerUpdate();
    Coor getPlayerHead();
    void setPlayerHead(Coor c);
    bool collisionWithThisBody(Coor c);

    virtual void render() override;
    
    void ChangeDir(Coor dir);
    void ResetPosition();

private:
    int id;
    std::vector<Coor> player;
    Coor init_pos;
    Coor dir_;
    Coor init_dir;
    std::string moves[4];
    std::random_device rnd;
};

#endif
