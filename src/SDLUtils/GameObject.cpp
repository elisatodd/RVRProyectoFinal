#include "GameObject.h"

#include <SDL2/SDL_image.h>

#include "Window.h"
#include "SDLUtils.h"

GameObject::~GameObject() {
    if(m_texture != nullptr)
        SDL_DestroyTexture(m_texture);
}

void GameObject::setTransform(const int x, const int y) {
    m_transform.set(x, y);
}

void GameObject::setTransform(Vector2D tr) {
    m_transform.set(tr);
}

void GameObject::setRotation(const float rot) {
    m_rotation = rot;
}

void GameObject::setSize(const int w, const int h) {
    m_size.set(w, h);
}

Vector2D GameObject::getSize() const {
    return m_size;
}

Vector2D GameObject::getTransform() const {
    return m_transform;
}

void GameObject::setTexture(const std::string &path) {
    SDL_Surface *surface = IMG_Load(path.c_str());
    if (surface == nullptr)
        throw "Couldn't load image: " + path;

    m_texture = SDL_CreateTextureFromSurface(Window().renderer(), surface);
    if (m_texture == nullptr) {
        SDL_FreeSurface(surface);
        throw "Couldn't load image: " + path;
    }

    m_texture_size.setX(surface->w);
    m_texture_size.setY(surface->h);

    SDL_FreeSurface(surface);
}

void GameObject::render()
{
    assert(m_texture != nullptr);

    SDL_Rect src = build_sdlrect(0, 0, m_texture_size.getX(), m_texture_size.getY());
    SDL_Rect dest = build_sdlrect(m_transform, m_size.getX(), m_size.getY());
    render(src, dest);
}

void GameObject::render(const SDL_Rect &src, const SDL_Rect &dest,
                const SDL_Point *p, SDL_RendererFlip flip)
{
    SDL_RenderCopyEx(Window().renderer(), m_texture, &src, &dest, m_rotation, p, flip);
}