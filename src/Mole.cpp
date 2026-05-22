#include "Mole.h"
#include <iostream>

void Mole::init(SDL_Renderer *renderer, int startX, int startY) {
    SDL_Surface *rawImage = IMG_Load("../assets/mole.png");
    if (rawImage != nullptr) {
        texture = SDL_CreateTextureFromSurface(renderer, rawImage);
        SDL_FreeSurface(rawImage);
    } else {
        std::cout << "mole.png bulunamadi! Hata: " << IMG_GetError() << std::endl;
    }

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

bool Mole::handleInput(int mouseX, int mouseY) {
    if (isUp) {
        SDL_Point mousePoint = { mouseX, mouseY };
        if (SDL_PointInRect(&mousePoint, &moleRectangle)) {
            hide();
            return true;
        }
    }
    return false;
}

void Mole::update() {
    if (isUp) {
        if (SDL_GetTicks() - popUpTime > stayUpDuration) {
            hide();
        }
    }
}

void Mole::render(SDL_Renderer *renderer) {
    if (isUp && texture != nullptr) {
        SDL_RenderCopy(renderer, texture, NULL, &moleRectangle);
    }
}

// Hafızayı temizleme fonksiyonu
void Mole::clean() {
    if (texture != nullptr) {
        SDL_DestroyTexture(texture);
        texture = nullptr;
    }
}