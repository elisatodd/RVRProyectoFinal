#include "ListaCoor.h"

ListaCoor::ListaCoor() : pri(nullptr) {}

void ListaCoor::InsertaFinal(const Coor& e) {
    if (pri == nullptr) {
        pri = new Nodo();
        pri->dato = e;
        pri->sig = nullptr;
    }
    else {
        Nodo* aux = pri;
        while (aux->sig != nullptr) {
            aux = aux->sig;
        }
        aux->sig = new Nodo();
        aux = aux->sig;
        aux->dato = e;
        aux->sig = nullptr;
    }
}

bool ListaCoor::BuscaDato(const Coor& e) const {
    Nodo* aux = BuscaNodo(e);
    return (aux != nullptr);
}

ListaCoor::Nodo* ListaCoor::BuscaNodo(const Coor& e) const {
    Nodo* aux = pri;
    while (aux != nullptr && aux->dato != e) {
        aux = aux->sig;
    }
    return aux;
}

void ListaCoor::BorraElto(const Coor& e) {
    if (pri == nullptr) {
        throw std::exception("Error: Lista vacía");
    }
    else if (pri->dato == e) {
        Nodo* temp = pri;
        pri = pri->sig;
        delete temp;
    }
    else {
        Nodo* aux = pri;
        while (aux->sig != nullptr && aux->sig->dato != e) {
            aux = aux->sig;
        }
        if (aux->sig == nullptr) {
            throw std::exception("Error: Elemento no encontrado");
        }
        else {
            Nodo* temp = aux->sig;
            aux->sig = aux->sig->sig;
            delete temp;
        }
    }
}

ListaCoor::Nodo* ListaCoor::nEsimoNodo(int n) const {
    if (n < 0) {
        throw std::exception("Error: Intentando acceder a un índice negativo");
    }
    else {
        Nodo* nEsimo = pri;
        int i = 0;
        while (nEsimo != nullptr && i < n) {
            i++;
            nEsimo = nEsimo->sig;
        }
        return nEsimo;
    }
}

Coor ListaCoor::nEsimo(int n) const {
    Nodo* aux = nEsimoNodo(n);
    if (aux == nullptr) {
        throw std::exception("Índice fuera de los límites de la lista");
    }
    else {
        return aux->dato;
    }
}

int ListaCoor::CuentaEltos() const {
    Nodo* aux = pri;
    int cont = 0;
    while (aux != nullptr) {
        aux = aux->sig;
        cont++;
    }
    return cont;
}

void ListaCoor::BorraTodos() {
    Nodo* aux = pri;
    while (aux != nullptr) {
        Nodo* temp = aux;
        aux = aux->sig;
        delete temp;
    }
    pri = nullptr;
}
