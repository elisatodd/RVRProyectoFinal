#include "TextObject.h"

TextObject::TextObject(SDL_Renderer* renderer, const std::string& fontFile, int fontSize, const std::string& text, SDL_Color color)
    : renderer(renderer), font(nullptr), texture(nullptr), color(color), text(text)
{
    // Load the font
    font = TTF_OpenFont(fontFile.c_str(), fontSize);
    if (!font) {
        // Handle font loading error
        SDL_Log("Failed to load font: %s", TTF_GetError());
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
    SDL_Rect destRect;
    destRect.x = position.getX();
    destRect.y = position.getY();

    // Query the texture to get its width and height
    SDL_QueryTexture(texture, nullptr, nullptr, &destRect.w, &destRect.h);

    // Render the texture
    SDL_RenderCopy(renderer, texture, nullptr, &destRect);
}

void TextObject::updateTexture() {
    if (texture) {
        SDL_DestroyTexture(texture);
        texture = nullptr;
    }

    // Create a surface from the rendered text
    SDL_Surface* surface = TTF_RenderText_Solid(font, text.c_str(), color);
    if (!surface) {
        // Handle surface creation error
        SDL_Log("Failed to create surface: %s", TTF_GetError());
        return;
    }

    // Create a texture from the surface
    texture = SDL_CreateTextureFromSurface(renderer, surface);
    if (!texture) {
        // Handle texture creation error
        SDL_Log("Failed to create texture: %s", SDL_GetError());
    }

    // Free the surface
    SDL_FreeSurface(surface);
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
