#ifndef TABLERO_H
#define TABLERO_H

#include <vector>
#include <random>

#include "../../SDLUtils/GameObject.h"

#include "Coor.h"
#include "EnumDefs.h"

#define OBSTACLE_SIZE 10

class Tablero : public GameObject {
private:
    std::vector<std::vector<TipoCasilla>> tab;
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
    //void Render();

    void render() override;

    bool thereIsWall(Coor C);
    int getWidth() const;
    int getHeight() const;
    Coor getPlayerOneInitialPosition() const;
    Coor getPlayerTwoInitialPosition() const;
};

#endif