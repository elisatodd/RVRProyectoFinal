#ifndef COOR_H
#define COOR_H

class Coor {
public:
    int x;
    int y;

    Coor(int _fil = 0, int _col = 0);
    Coor operator+(const Coor& c) const;
    Coor operator-() const;
    bool operator==(const Coor& c) const;
    bool operator!=(const Coor& c) const;
    bool Equals(const Coor& c) const;
};

#endif
 