#include "Mole.h"

void Mole::init(SDL_Renderer* renderer, int startX, int startY) {
    SDL_Surface *rawImage = IMG_Load("../assets/mole.png");
    texture = SDL_CreateTextureFromSurface(renderer, rawImage);
    SDL_FreeSurface(rawImage);

    moleRectangle.x = startX;
    moleRectangle.y = startY;
    moleRectangle.w = 100;
    moleRectangle.h = 100;
    
    isUp = false; 
}

void Mole::popUp() {
    if (!isUp) {
        isUp = true;
        popUpTime = SDL_GetTicks();
    }
}

void Mole::hide() {
    isUp = false;
}

bool Mole::isShowing() {
    return isUp;
}

void Mole::update() {
    if (isUp) {
        if (SDL_GetTicks() - popUpTime > stayUpDuration) {
            hide();
        }
    }
}

void Mole::render(SDL_Renderer* renderer) {
    if (isUp) {
        SDL_RenderCopy(renderer, texture, NULL, &moleRectangle);
    }
}