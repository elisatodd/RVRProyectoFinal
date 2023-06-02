#include "TextObject.h"

#include <iostream>
#include <cassert>

#include "Window.h"

TextObject::TextObject(SDL_Renderer* renderer, const std::string& fontFile, int fontSize, const std::string& text, SDL_Color color)
    : renderer(renderer), font(nullptr), texture(nullptr), color(color), text(text)
{
    // Load the font
    font = TTF_OpenFont(fontFile.c_str(), fontSize);
    if (!font) {
        // Handle font loading error
        std::cout << "Failed to load font: " << TTF_GetError() << "\n";
        return;
    }

    // Create the texture
    updateTexture();
}

TextObject::~TextObject() {
    // Clean up resources
    if (texture) {
        SDL_DestroyTexture(texture);
        texture = nullptr;
    }
    if (font) {
        TTF_CloseFont(font);
        font = nullptr;
    }
}   

void TextObject::setText(const std::string& newText) {
    text = newText;
    updateTexture();
}

std::string TextObject::getText(){
    return text;
}

void TextObject::setColor(SDL_Color newColor) {
    color = newColor;
    updateTexture();
}

void TextObject::render() {
    if (texture == nullptr)
        return;

    SDL_Rect destRect;
    destRect.x = position.getX();
    destRect.y = position.getY();

  //  Query the texture to get its width and height
    SDL_QueryTexture(texture, nullptr, nullptr, &destRect.w, &destRect.h);

  //  Render the texture
    SDL_RenderCopy(renderer, texture, nullptr, &destRect);

    // SDL_Rect src = build_sdlrect(0, 0, textWidth, textHeight);
    // SDL_Rect dest = build_sdlrect(position, textWidth, textHeight);

    // std::cout << "Rendering text in: (" << position.getX() << ", " << position.getY() << ")\n";
    // SDL_RenderCopyEx(Window().renderer(), texture, &src, &dest, 0.0f, nullptr, SDL_FLIP_NONE);
}

void TextObject::updateTexture() {

    if (texture) {
        //std::cout << "\tPre-existing texture --> delete\n";
        SDL_DestroyTexture(texture);
        texture = nullptr;
        //std::cout << "\tDeleted pre-existing texture\n";
    }

    // Create a surface from the rendered text
    const char* t = text.c_str();
    SDL_Surface* surface = TTF_RenderText_Solid(font, t, color);
    if (!surface) {
        // Handle surface creation error
        std::cout << "Failed to create surface: " << TTF_GetError() << "\n";
        return;
    }

    int textWidth = surface->w;
    int textHeight = surface->h;

   // std::cout << "W: " << textWidth << " H: " << textHeight << " \n";

    // Create a texture from the surface
     texture = SDL_CreateTextureFromSurface(renderer, surface);
    if (!texture) {
        // Handle texture creation error
        std::cout << "Failed to create texture: " << SDL_GetError() << "\n";
        SDL_FreeSurface(surface); // Free the surface before returning
        return;
    }

    // Free the surface
    SDL_FreeSurface(surface);

    std::cout << "Finished updating texture\n";
}

void TextObject::setPosition(int x, int y){
    position = Vector2D(x, y);
}

void TextObject::setScore(int i){
    score = i;
}

int TextObject::getScore(){
    return score;
}
