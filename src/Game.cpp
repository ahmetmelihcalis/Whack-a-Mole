#include "Game.h"
#include <iostream>
#include <cstdlib>
#include <ctime>

using namespace std;

void Game::init(const char *title, int xposition, int yposition, int width, int height, bool fullscreen) {
    int flags = 0;
    if (fullscreen) flags = SDL_WINDOW_FULLSCREEN;

    if (SDL_Init(SDL_INIT_VIDEO) == 0) {
        window = SDL_CreateWindow(title, xposition, yposition, width, height, flags);
        renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
        
        TTF_Init();
        font = TTF_OpenFont("../assets/arial.ttf", 32);
        if (!font) {
            cout << "Font yüklenemedi!" << endl;
        }

        SDL_Color white = {255, 255, 255, 255};
        SDL_Surface *menuSurf = TTF_RenderText_Solid(font, "OYUNA BAŞLAMAK İÇİN TIKLA", white);
        menuTextTexture = SDL_CreateTextureFromSurface(renderer, menuSurf);
        
        menuButtonRectangle.x = (1280 / 2) - (menuSurf->w / 2);
        menuButtonRectangle.y = (720 / 2) - (menuSurf->h / 2);
        menuButtonRectangle.w = menuSurf->w;
        menuButtonRectangle.h = menuSurf->h;
        SDL_FreeSurface(menuSurf);

        scoreRectangle.x = 1000; 
        scoreRectangle.y = 30;
        updateScoreText();

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

void Game::updateScoreText() {
    if (scoreTexture != nullptr) {
        SDL_DestroyTexture(scoreTexture); 
    }

    string scoreText = "Skor: " + to_string(score);
    SDL_Color textColor = {255, 255, 255, 255}; 
    SDL_Surface *textSurface = TTF_RenderText_Solid(font, scoreText.c_str(), textColor);
    
    scoreRectangle.w = textSurface->w;
    scoreRectangle.h = textSurface->h;

    scoreTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
    SDL_FreeSurface(textSurface); 
}

void Game::handleEvents() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            isRunning = false;
        }

        if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT) {
            int mx = event.button.x;
            int my = event.button.y;

            if (currentState == MENU) {
                SDL_Point p = {mx, my};
                if (SDL_PointInRect(&p, &menuButtonRectangle)) {
                    currentState = PLAYING; 
                    cout << "Oyun Başladı!" << endl;
                }
            }
            else if (currentState == PLAYING) {
                for (int i = 0; i < 9; i++) {
                    if (moles[i].handleInput(mx, my)) {
                        score += 10;
                        updateScoreText(); 
                    }
                }
            }
        }
    }
}

void Game::update() {
    if (currentState == PLAYING) {
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
}

void Game::render() {
    if (currentState == MENU) {
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, menuTextTexture, NULL, &menuButtonRectangle);
    } 
    else if (currentState == PLAYING) {
        SDL_SetRenderDrawColor(renderer, 34, 139, 34, 255);
        SDL_RenderClear(renderer);

        for (int i = 0; i < 9; i++) {
            moles[i].render(renderer);
        }
        if (scoreTexture) {
            SDL_RenderCopy(renderer, scoreTexture, NULL, &scoreRectangle);
        }
    }

    SDL_RenderPresent(renderer); 
}

void Game::clean() {
    SDL_DestroyTexture(menuTextTexture);
    SDL_DestroyTexture(scoreTexture);
    TTF_CloseFont(font);
    TTF_Quit();

    SDL_DestroyWindow(window);
    SDL_DestroyRenderer(renderer);
    SDL_Quit();
}

bool Game::running() { 
    return isRunning; 
}