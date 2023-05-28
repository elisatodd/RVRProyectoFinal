#ifndef LISTACOOR_H
#define LISTACOOR_H

#include "Coor.h"

class ListaCoor {
private:
    struct Nodo {
        Coor dato;
        Nodo* sig;
    };
    Nodo* pri;

    Nodo* BuscaNodo(const Coor& e) const;
    Nodo* nEsimoNodo(int n) const;

public:
    ListaCoor();
    void InsertaFinal(const Coor& e);
    bool BuscaDato(const Coor& e) const;
    void BorraElto(const Coor& e);
    Coor nEsimo(int n) const;
    int CuentaEltos() const;
    void BorraTodos();
};

#endif
