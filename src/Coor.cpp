#include "Coor.h"

Coor::Coor(int _fil, int _col) : x(_fil), y(_col) {}

Coor Coor::operator+(const Coor& c) const {
    return Coor(x + c.x, y + c.y);
}

Coor Coor::operator-() const {
    return Coor(-x, -y);
}

bool Coor::operator==(const Coor& c) const {
    return x == c.x && y == c.y;
}

bool Coor::operator!=(const Coor& c) const {
    return !(*this == c);
}

bool Coor::Equals(const Coor& c) const {
    return *this == c;
}
