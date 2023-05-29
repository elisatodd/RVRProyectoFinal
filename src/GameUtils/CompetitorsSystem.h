#ifndef COMPETITORSSYSTEM_H
#define COMPETITORSSYSTEM_H

#include <iostream>
#include <vector>
#include "Player.h"
class Tablero;

struct Competitor {
    Player* player;
    int points;
};

class CompetitorsSystem {
private:
    std::vector<Competitor*> players;
    //std::vector<ConsoleColor> motorbikeColor;
    int maxScore_;

public:
    CompetitorsSystem(GameMode gM, int maxScores, Tablero* tab);
    //Borra sus elementos y a los jugadores
    ~CompetitorsSystem();

    Winner CheckCollisions(Tablero* tab, int renderOffsetX, int renderOffsetY);
    void render(int offsetX, int offsetY);
    void showPoints(int offsetX);
    void Update(Tablero* tab, GameMode& gameMode);
    bool thisCollide(Coor c, Tablero* tab);
    //ConsoleColor getMotorbikeColor(int i);
    void RoundOver();

private:
    Winner addPoints(CollisionState cS, int offsetX, int offsetY);
    std::vector<char> getInput2(GameMode& gameMode);
};

#endif  // COMPETITORSSYSTEM_H

