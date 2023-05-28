#ifndef TABLERO_H
#define TABLERO_H

#include <vector>
#include <random>

struct Coor {
    int x;
    int y;

    Coor(int x, int y) : x(x), y(y) {}
};

enum TipoCasilla {
    FREE,
    WALL
};

class Tablero {
private:
    TipoCasilla** tab;
    Wormhole* wH_;
    Coor player1_IniPos;
    Coor player2_IniPos;
    std::default_random_engine rnd;

    int xLength = 0;
    int yLength = 0;

public:
    Tablero(int width, int height);
    ~Tablero();
    void Init();
    void CreateWormhole();
    void CheckWormholeCollision();
    void DeleteWormholes();
    void DeleteTablero();

    bool ThereIsWall(Coor C);

    int getWidth() const;
    int getHeight() const;
    Coor getPlayer1InitialPosition() const;
    Coor getPlayer2InitialPosition() const;
};

#endif