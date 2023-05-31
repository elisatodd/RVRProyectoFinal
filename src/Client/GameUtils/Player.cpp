#include "Player.h"
#include "CompetitorsSystem.h"
#include "Tablero.h"
#include "GameManager.h"
#include <iostream>

#include <../../SDL2/SDL_image.h>
#include "../../SDLUtils/Window.h"
#include "../../SDLUtils/SDLUtils.h"
#include "../../SDLUtils/Vector2D.h"

Player::Player(Coor c) {
    init_pos = c;
    player.InsertaFinal(c);
    dir_ = Coor(1, 0);
    init_dir = dir_;
}

void Player::setControls(std::string u, std::string d, std::string r, std::string l) {
    moves[Directions::UP] = u;
    moves[Directions::DOWN] = d;
    moves[Directions::RIGHT] = r;
    moves[Directions::LEFT] = l;
}

void Player::handleInput(const SDL_Event &e) {
    // TO DO
}

//Actualización de juego
void Player::playerUpdate() {
    //procesaInput(input, gameMode);
    player.InsertaFinal(player.nEsimo(player.CuentaEltos() - 1) + dir_);
}

Coor Player::getPlayerHead() {
    return player.nEsimo(player.CuentaEltos() - 1);
}

void Player::setPlayerHead(Coor c)
{
    player.nEsimo(player.CuentaEltos() - 1) = c;
}

bool Player::collisionWithThisBody(Coor c) {
    int i = 0;
    while (i < player.CuentaEltos() - 1 && c != player.nEsimo(i))
        i++;
    return i != player.CuentaEltos() - 1;
}

void Player::render(){
    
    assert(m_texture != nullptr);
    Vector2D renderPosition = GameManager::instance()->coorToRenderPos(getPlayerHead());
    SDL_Rect src = build_sdlrect(0, 0, m_texture_size.getX(), m_texture_size.getY());
    SDL_Rect dest;
    
    for(int i = 0; i < player.CuentaEltos(); i++){
        Vector2D renderPos = GameManager().coorToRenderPos(player.nEsimo(i));
        dest = build_sdlrect(renderPosition, m_size.getX(), m_size.getY());
        Render(src, dest);
    }

    dest = build_sdlrect(renderPosition, m_size.getX(), m_size.getY());
    Render(src, dest);
}

void Player::procesaInput(char c, GameMode& gameMode) {
    switch (c) {
        case 'u':
            ChangeDir(Coor(0, -1));
            break;
        case 'd':
            ChangeDir(Coor(0, 1));
            break;
        case 'r':
            ChangeDir(Coor(1, 0));
            break;
        case 'l':
            ChangeDir(Coor(-1, 0));
            break;
        case 'q':
            gameMode = GameMode::QUIT;
            break;
        default:
            break;
    }
}

void Player::ChangeDir(Coor dir) {
    if (-dir != dir_)
        dir_ = dir;
}

void Player::ResetPosition() {
    player.BorraTodos();
    player.InsertaFinal(init_pos);
    dir_ = init_dir;
}

std::string Player::getMoveString(Directions dir) {
    return moves[dir];
}

bool Player::hasThisMove(std::string s) {
    int i = 0;
    while (i < 4 && moves[i] != s)
        i++;

    return i != 4;
}

char Player::ConvertStringToDir(std::string s) {
    char c = ' ';
    if (s == moves[Directions::UP])
        c = 'u';
    else if (s == moves[Directions::DOWN])
        c = 'd';
    else if (s == moves[Directions::LEFT])
        c = 'l';
    else if (s == moves[Directions::RIGHT])
        c = 'r';
    return c;
}

void Player::Teleport(Coor c) {
    player.BorraElto(getPlayerHead());
    player.InsertaFinal(c + dir_);
}
