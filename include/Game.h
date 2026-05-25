#ifndef GAME_H
#define GAME_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <vector>
#include "Mole.h"
#include "UIManager.h"

enum GameState { MENU, PLAYING, GAMEOVER, HIGHSCORES };

class Game {
public:
    void init(const char *title, int xposition, int yposition, int width, int height, bool fullscreen);
    void handleEvents();
    void update();
    void render();
    void clean();
    bool running();

private:
    // Dosyadan skor yükleme ve kaydetme fonksiyonları
    void loadHighScores();
    void checkAndSaveHighScore(int currentScore);

    bool isRunning = false;
    SDL_Window *window = nullptr;
    SDL_Renderer *renderer = nullptr;

    // İki farklı arka plan için değişkenler
    SDL_Texture *menuBgTexture = nullptr;
    SDL_Texture *gameBgTexture = nullptr;

    Mole moles[9];
    UIManager ui;

    // Oyun zamanlayıcı değişkenleri
    Uint32 lastSpawnTime = 0;
    Uint32 spawnInterval = 800;
    Uint32 gameStartTime = 0;

    // Oyun durumu değişkenleri
    GameState currentState = MENU;
    int score = 0;
    int timeLimit = 30;
    int timeRemaining = 30;

    // Ses efekti için kullanılacak pointer
    Mix_Chunk *hitSound = nullptr;

    // En yüksek 5 skoru tutacak liste
    std::vector<int> highScores;
};

#endif