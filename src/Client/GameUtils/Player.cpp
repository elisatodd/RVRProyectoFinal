#include "Player.h"
#include "Tablero.h"
#include "GameManager.h"

#include <iostream>

#include <../../SDL2/SDL_image.h>
#include "../../SDLUtils/Window.h"
#include "../../SDLUtils/SDLUtils.h"
#include "../../SDLUtils/Vector2D.h"

Player::Player(Coor c) {
    init_pos = c;
    player.push_back(c);
    dir_ = Coor(0, 0);
    init_dir = dir_;
}

void Player::playerUpdate() {
    if(player.size()>0 && dir_ != Coor(0, 0)){
        player.push_back(player[player.size() - 1] + dir_);
    }
}

Coor Player::getPlayerHead() {
    return player[player.size() - 1];
}

void Player::setPlayerHead(Coor c)
{
    player[player.size() - 1] = c;
}

bool Player::collisionWithThisBody(Coor c) {
    int i = 0;
    while (i < player.size() - 1 && c != player[i])
        i++;
    return i != player.size() - 1;
}

void Player::render(){
    
    assert(m_texture != nullptr);
    Vector2D renderPosition = GameManager::instance()->coorToRenderPos(getPlayerHead());
    SDL_Rect src = build_sdlrect(0, 0, m_texture_size.getX(), m_texture_size.getY());
    
    for(int i = 0; i < player.size(); i++){
        Vector2D renderPos = GameManager().coorToRenderPos(player[i]);
        SDL_Rect dest = build_sdlrect(renderPos, m_size.getX(), m_size.getY());
        SDL_RenderCopyEx(Window().renderer(), m_texture, &src, &dest, m_rotation, nullptr, SDL_FLIP_NONE);
    }
}

void Player::ChangeDir(Coor dir) {
    if (-dir != dir_)
        dir_ = dir;
}

void Player::ResetPosition() {
    player.clear();
    player.push_back(init_pos);
    dir_ = init_dir;
}
