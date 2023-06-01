#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <string>

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

    int score = 0;

    void updateTexture();
};

// int main(int argc, char* argv[]) {
//     // Initialize SDL and TTF
//     SDL_Init(SDL_INIT_VIDEO);
//     TTF_Init();

//     // Create the window and renderer
//     SDL_Window* window = SDL_CreateWindow("Text Object Example", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 800, 600, 0);
//     SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

//     // Create the text object
//     TextObject textObject(renderer, "arial.ttf", 24, "Hello, SDL!", {255, 255, 255});

//     bool quit = false;
//     SDL_Event event;
//     while (!quit) {
//         while (SDL_PollEvent(&event)) {
//             if (event.type == SDL_QUIT) {
//                 quit = true;
//             }
//         }

//         SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
//         SDL_RenderClear(renderer);

//         // Render the text object
//         textObject.render(400, 300);

//         SDL_RenderPresent(renderer);
//     }

//     // Clean up
//     SDL_DestroyRenderer(renderer);
//     SDL_DestroyWindow(window);
//     TTF_Quit();
//     SDL_Quit();

//     return 0;
// }
