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

        // 3x3 Izgara
        int startX = 415;  
        int startY = 135;  
        int spacing = 150; 

        for (int i = 0; i < 9; i++) {
            int row = i / 3; 
            int col = i % 3; 
            
            int x = startX + (col * spacing);
            int y = startY + (row * spacing);
            
            moles[i].init(renderer, x, y);
        }

        moles[4].popUp();
        moles[0].popUp();

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
    for (int i = 0; i < 9; i++) {
        moles[i].update();
    }
}

void Game::render() {
    SDL_RenderClear(renderer); 
    for (int i = 0; i < 9; i++) {
        moles[i].render(renderer);
    }
    SDL_RenderPresent(renderer); 
}

void Game::clean() {
    SDL_DestroyWindow(window);
    SDL_DestroyRenderer(renderer);
    SDL_Quit();
}