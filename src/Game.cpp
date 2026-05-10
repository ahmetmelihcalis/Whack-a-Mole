#include "Game.h"
#include <iostream>
#include <cstdlib>
#include <ctime>

using namespace std;

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
            moles[i].init(renderer, startX + (col * spacing), startY + (row * spacing));
        }

        srand(static_cast<unsigned int>(time(nullptr))); 
        lastSpawnTime = SDL_GetTicks();
        isRunning = true; 
    } else {
        isRunning = false;
    }
}

void Game::handleEvents() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            isRunning = false;
        }
        
        // Vuruş Tespiti
        if (event.type == SDL_MOUSEBUTTONDOWN) {
            if (event.button.button == SDL_BUTTON_LEFT) {
                int mouseX = event.button.x;
                int mouseY = event.button.y;

                for (int i = 0; i < 9; i++) {
                    if (moles[i].handleInput(mouseX, mouseY)) {
                        score += 10;
                        cout << "VURDUN! Skor: " << score << endl;
                    }
                }
            }
        }
    }
}

void Game::update() {
    for (int i = 0; i < 9; i++) {
        moles[i].update();
    }

    if (SDL_GetTicks() - lastSpawnTime > spawnInterval) {
        int randomIndex = rand() % 9;
        if (!moles[randomIndex].isShowing()) {
            moles[randomIndex].popUp();
        }
        lastSpawnTime = SDL_GetTicks();
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

bool Game::running() { 
    return isRunning; 
}