#ifndef TABLERO_H
#define TABLERO_H

#include <vector>
#include <random>
#include "Coor.h"

enum TipoCasilla {
    FREE,
    WALL
};

class Tablero {
private:
    TipoCasilla** tab;
    Coor player1_IniPos;
    Coor player2_IniPos;
    std::default_random_engine rnd;

    int xLength = 0;
    int yLength = 0;

public:
    Tablero(const std::string& file);
    ~Tablero();

    void GenerateTablero(const std::string& file);
    void DeleteTablero();
    void Render(int offsetX, int offsetY, bool renderInitPos);

    bool thereIsWall(Coor C);
    int getWidth() const;
    int getHeight() const;
    Coor getPlayer1InitialPosition() const;
    Coor getPlayer2InitialPosition() const;
};

#endif