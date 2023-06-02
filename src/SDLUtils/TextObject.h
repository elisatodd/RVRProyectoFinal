#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <string>
#include <mutex>

#include "Vector2D.h"

class TextObject {
public:
    TextObject(SDL_Renderer* renderer, const std::string& fontFile, int fontSize, const std::string& text, SDL_Color color);

    ~TextObject();

    void setText(const std::string& newText);

    std::string getText();

    void setColor(SDL_Color newColor);

    void render();

    void setPosition(int x, int y);

    void setScore(int i);
    int getScore();

private:
    Vector2D position;

    SDL_Renderer* renderer;
    TTF_Font* font;
    SDL_Texture* texture;
    SDL_Color color;
    std::string text;

    int textWidth = 0;
    int textHeight = 0;

    int score = 0;

    std::mutex update_mutex;

    void updateTexture();
};