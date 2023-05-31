#include "Tablero.h"

#include <fstream> 
#include <stdexcept>    
#include <iostream>

#include <SDL2/SDL_image.h>

#include "../../SDLUtils/Window.h"
#include "GameManager.h"

Tablero::Tablero(const std::string& file) {
    GenerateTablero(file);
    setTexture("./assets/images/Obstacle.png");
}

Tablero::~Tablero() {
    DeleteTablero();
}

void Tablero::GenerateTablero(const std::string& file) {
    std::ifstream f(file);
    if (!f.is_open()) {
        throw std::runtime_error("No se encontró el fichero con el tablero");
    }

    std::string line;
    std::string data;

    while (std::getline(f, line)) {
        yLength++;
        data += line;
        if (!line.empty() && xLength < line.length())
            xLength = line.length();
    }

    f.close();

    // tab = TipoCasilla[xLength];
    // for (int i = 0; i < xLength; i++) {
    //     tab[i] = new TipoCasilla[yLength];
    // }
    tab.resize(xLength, std::vector<TipoCasilla>(yLength));
    int cont = 0;

    for (int i = 0; i < yLength; i++) {
        for (int j = 0; j < xLength; j++) {
            switch (data[cont]) {
                case '0':
                    tab[j][i] = TipoCasilla::FREE;
                    break;
                case '1':
                    tab[j][i] = TipoCasilla::WALL;
                    break;
                case '2':
                    tab[j][i] = TipoCasilla::FREE;
                    player1_IniPos = Coor(j, i);
                    break;
                case '3':
                    tab[j][i] = TipoCasilla::FREE;
                    player2_IniPos = Coor(j, i);
                    break;
                default:
                    break;
            }
            if (cont < data.length())
                cont++;
        }
    }
}

// void Tablero::Render() {
//     // for (int i = 0; i < getWidth(); i++) {
//     //     for (int j = 0; j < getHight(); j++) {
//     //         Console::SetCursorPosition((i * 2) + offsetX, j + offsetY);
//     //         switch (tab[i][j]) {
//     //             case TipoCasilla::FREE:
//     //                 Console::BackgroundColor = ConsoleColor::Gray;
//     //                 break;
//     //             case TipoCasilla::WALL:
//     //                 Console::BackgroundColor = ConsoleColor::DarkGray;
//     //                 break;
//     //             default:
//     //                 break;
//     //         }
//     //         Console::Write("  ");
//     //     }
//     // }

//     // if (renderInitPos) {
//     //     DrawInitPlayer(offsetX, offsetY, player1_IniPos, ConsoleColor::DarkMagenta);
//     //     DrawInitPlayer(offsetX, offsetY, player2_IniPos, ConsoleColor::Blue);
//     // }
// }

void Tablero::render() {
    std::cout << "Dibujando tablero\n";

    int screenWidth = Window().width();
    int screenHeight = Window().height();

    int xOffset = (screenWidth - xLength * OBSTACLE_SIZE) / 2;
    int yOffset = (screenHeight - yLength * OBSTACLE_SIZE) / 2;

    SDL_Rect src = build_sdlrect(0, 0, 1, 1); // texture size

    for (int i = 0; i < getWidth(); i++) {
        for (int j = 0; j < getHeight(); j++) {
            if  (tab[i][j] == TipoCasilla::WALL){
                Coor pos = Coor(i,j);
                Vector2D renderPos = GameManager().coorToRenderPos(pos);

                SDL_Rect dest = build_sdlrect(renderPos, OBSTACLE_SIZE, OBSTACLE_SIZE);
                SDL_RenderCopyEx(Window().renderer(), m_texture, &src, &dest, m_rotation, nullptr, SDL_FLIP_NONE);

            }
        }
    }
}

void Tablero::DeleteTablero() {
    tab.clear();
}

bool Tablero::thereIsWall(Coor c){
    return tab[c.x][c.y] == WALL;
}

int Tablero::getWidth() const {
    return xLength;
}

int Tablero::getHeight() const {
    return yLength;
}

Coor Tablero::getPlayerOneInitialPosition() const {
    return player1_IniPos;
}

Coor Tablero::getPlayerTwoInitialPosition() const {
    return player2_IniPos;
}
