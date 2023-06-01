#ifndef TABLERO_H
#define TABLERO_H

#include <vector>
#include <random>

#include "../../SDLUtils/GameObject.h"

#include "Coor.h"
#include "EnumDefs.h"

class Tablero : public GameObject {
private:
    std::vector<std::vector<TipoCasilla>> tab;
    std::vector<std::vector<TipoCasilla>> tab_original_copy;

    Coor player1_IniPos;
    Coor player2_IniPos;
    std::default_random_engine rnd;

    int xLength = 0;
    int yLength = 0;
    int OBSTACLE_SIZE = 10;
public:
    Tablero(const std::string& file);
    ~Tablero();

    void GenerateTablero(const std::string& file);
    void DeleteTablero();
    void ResetTableroToDefault();
    //void Render();

    void render() override;

    bool thereIsWall(Coor C);
    bool outOfBounds(Coor C);
    int getWidth() const;
    int getHeight() const;
    void setWall(Coor c);
    Coor getPlayerOneInitialPosition() const;
    Coor getPlayerTwoInitialPosition() const;
};

#endif