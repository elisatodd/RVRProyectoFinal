#ifndef PLAYER_H
#define PLAYER_H

#include <vector>
#include <string>
#include <random>

#include "Coor.h"
#include "ListaCoor.h"
#include "EnumDefs.h"

class Tablero;
class CompetitorsSystem;

enum Directions { UP = 0, DOWN, RIGHT, LEFT };

class Player {
public:
    Player(Coor c);
    void setControls(std::string u, std::string d, std::string r, std::string l);
    void update(CompetitorsSystem* cS, Tablero* tab, char input, GameMode& gameMode);
    Coor getPlayerHead();
    bool collisionWithThisBody(Coor c);
    //void render(ConsoleColor color, int offsetX, int offsetY);
    void procesaInput(char c, GameMode& gameMode);
    void ChangeDir(Coor dir);
    void ResetPosition();
    
    std::string getMoveString(Directions dir);
    bool hasThisMove(std::string s);
    char ConvertStringToDir(std::string s);
    void Teleport(Coor c);

private:
    int id;
    ListaCoor player;
    Coor init_pos;
    Coor dir_;
    Coor init_dir;
    std::string moves[4];
    std::random_device rnd;
};

#endif
