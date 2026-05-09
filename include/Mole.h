#ifndef MOLE_H
#define MOLE_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

class Mole {
public:
    void init(SDL_Renderer *renderer, int startX, int startY);
    void update();
    void render(SDL_Renderer *renderer);

    void popUp();
    void hide();
    bool isShowing();

private:
    SDL_Texture *texture = nullptr; 
    SDL_Rect moleRectangle;
    
    bool isUp = false; 
};

#endif