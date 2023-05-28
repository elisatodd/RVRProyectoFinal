#include "Tablero.h"

Tablero::Tablero(const std::string& file) {
    GenerateTablero(file);
}

Tablero::~Tablero() {
    DeleteTablero();
}

void Tablero::GenerateTablero(const std::string& file) {
    std::ifstream f(file);
    if (!f.is_open()) {
        throw std::runtime_error("Error: Could not open file");
    }

    wH_ = nullptr;

    std::string line;
    std::string data;

    while (std::getline(f, line)) {
        yLength++;
        data += line;
        if (!line.empty() && xLength < line.length())
            xLength = line.length();
    }

    f.close();
    tab = new TipoCasilla*[xLength];
    for (int i = 0; i < xLength; i++) {
        tab[i] = new TipoCasilla[yLength];
    }

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

    std::random_device rd;
    rnd.seed(rd());
    wormholeFrameTimer_ = rnd() % 10;
}

void Tablero::Render(int offsetX, int offsetY, bool renderInitPos = false) {
    // for (int i = 0; i < getWidth(); i++) {
    //     for (int j = 0; j < getHight(); j++) {
    //         Console::SetCursorPosition((i * 2) + offsetX, j + offsetY);
    //         switch (tab[i][j]) {
    //             case TipoCasilla::FREE:
    //                 Console::BackgroundColor = ConsoleColor::Gray;
    //                 break;
    //             case TipoCasilla::WALL:
    //                 Console::BackgroundColor = ConsoleColor::DarkGray;
    //                 break;
    //             default:
    //                 break;
    //         }
    //         Console::Write("  ");
    //     }
    // }

    // if (renderInitPos) {
    //     DrawInitPlayer(offsetX, offsetY, player1_IniPos, ConsoleColor::DarkMagenta);
    //     DrawInitPlayer(offsetX, offsetY, player2_IniPos, ConsoleColor::Blue);
    // }
}

void Tablero::DeleteTablero() {
    for (int i = 0; i < getWidth(); i++) {
        delete[] tab[i];
    }
    delete[] tab;
}

bool Tablero::ThereIsWall(Coor c){
    return tab[c.x][c.y] == WALL;
}

int Tablero::getWidth() const {
    return xLength;
}

int Tablero::getHight() const {
    return yLength;
}

Coor Tablero::getPlayer1InitialPosition() const {
    return player1_IniPos;
}

Coor Tablero::getPlayer2InitialPosition() const {
    return player2_IniPos;
}
