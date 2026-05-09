#include "Game.h"

void Game::init(const char *title, int xposition, int yposition, int width, int height, bool fullscreen) {
    int flags = 0;
    if (fullscreen) {
        flags = SDL_WINDOW_FULLSCREEN;
    }

    if (SDL_Init(SDL_INIT_VIDEO) == 0) {
        window = SDL_CreateWindow(title, xposition, yposition, width, height, flags);
        renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
        
        if (renderer) {
            SDL_SetRenderDrawColor(renderer, 34, 139, 34, 255);
        }

        SDL_Surface* rawImage = IMG_Load("../assets/mole.png");
        moleTexture = SDL_CreateTextureFromSurface(renderer, rawImage);
        SDL_FreeSurface(rawImage); 

        moleRectangle.x = 350; 
        moleRectangle.y = 250; 
        moleRectangle.w = 100; 
        moleRectangle.h = 100; 

        isRunning = true; 
    } else {
        isRunning = false; 
    }
}

void Game::handleEvents() {
    SDL_Event event;
    SDL_PollEvent(&event);
    
    if (event.type == SDL_QUIT) {
        isRunning = false;
    }
}

bool Game::running() {
    return isRunning;
}

void Game::update() {
    // Şimdilik boş bırakıldı
}

void Game::render() {
    SDL_RenderClear(renderer); 
    
    SDL_RenderCopy(renderer, moleTexture, NULL, &moleRectangle);
    
    SDL_RenderPresent(renderer); 
}

void Game::clean() {
    SDL_DestroyTexture(moleTexture);
    SDL_DestroyWindow(window);
    SDL_DestroyRenderer(renderer);
    SDL_Quit();
}