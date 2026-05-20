#ifndef GAME_H
#define GAME_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h> 
#include <string>
#include "Mole.h"

enum GameState { MENU, PLAYING, GAMEOVER };

class Game {
public:
    void init(const char *title, int xposition, int yposition, int width, int height, bool fullscreen);
    void handleEvents();
    void update();
    void render();
    void clean();
    bool running(); 
    void updateScoreText();
    void updateTimerText();

private:
    bool isRunning = false; 
    SDL_Window *window = nullptr;
    SDL_Renderer *renderer = nullptr;
    
    Mole moles[9];

    // Oyun Motorunun Zamanlayıcı Değişkenleri
    Uint32 lastSpawnTime = 0;     // Son köstebeğin çıktığı an
    Uint32 spawnInterval = 800;   // Her 800 milisaniyede bir deneme yapılması için
    int score = 0; 

    TTF_Font *font = nullptr;
    SDL_Texture *scoreTexture = nullptr;
    SDL_Rect scoreRectangle;

    // Durum Makinesi ve Menü Elemanları
    GameState currentState = MENU; 
    SDL_Texture *menuTextTexture = nullptr;
    SDL_Rect menuButtonRectangle;

    // Süre ve Game Over Değişkenleri
    Uint32 gameStartTime = 0;      // Oyunun başladığı an
    int timeLimit = 60;            // Oyun süresi
    int timeRemaining = 60;        // Kalan süre
    
    SDL_Texture *timerTexture = nullptr;
    SDL_Rect timerRectangle;       // Sayacın ekrandaki yeri
    
    SDL_Texture *gameOverTextTexture = nullptr;
    SDL_Rect gameOverRectangle;    // Game over yazısının yeri
    
    // Ses efekti için kullanılacak pointer
    Mix_Chunk *hitSound = nullptr; 
};

#endif