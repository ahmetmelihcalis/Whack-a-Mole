#include "Game.h"
#include <iostream>
#include <cstdlib>
#include <ctime>

using namespace std;

void Game::init(const char *title, int xposition, int yposition, int width, int height, bool fullscreen) {
    int flags = 0;
    if (fullscreen) flags = SDL_WINDOW_FULLSCREEN; // Tam ekran kontrolü

    // SDL'i başlatma ve pencereyi oluşturma
    if (SDL_Init(SDL_INIT_VIDEO) == 0) {
        window = SDL_CreateWindow(title, xposition, yposition, width, height, flags);
        renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
        
        // UI sistemini başlatma
        ui.init(renderer, "../assets/arial.ttf", 32);

        // Ses Sistemini Başlatma (Mixer)
        if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
            cout << "Ses sistemi baslatilamadi! Hata: " << Mix_GetError() << endl;
        }
        
        // Vuruş sesini hafızaya yükleme
        hitSound = Mix_LoadWAV("../assets/hit.wav");
        if (hitSound == nullptr) {
            cout << "hit.wav dosyasi yuklenemedi! Hata: " << Mix_GetError() << endl;
        }
        
        // İlk Skor ve Süre metinlerini hazırlama
        ui.updateScore(renderer, score);
        ui.updateTimer(renderer, timeRemaining);

        // 3x3 Izgara
        int startX = 415;  
        int startY = 135;  
        int spacing = 150; 
        for (int i = 0; i < 9; i++) {
            int row = i / 3;
            int col = i % 3; 
            moles[i].init(renderer, startX + (col * spacing), startY + (row * spacing));
        }

        srand(static_cast<unsigned int>(time(nullptr))); // Rastgele sayı üretecini başlatma
        isRunning = true; // Oyun döngüsünü aktif etme
    } else {
        isRunning = false; // Oyunu kapatma
    }
}

// Kullanıcı girdilerini yakalama
void Game::handleEvents() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            isRunning = false;
        }

        if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT) {
            int mx = event.button.x;
            int my = event.button.y;
            SDL_Point p = {mx, my};

            // DURUM 1: Ana Menü
            if (currentState == MENU) {
                SDL_Rect menuBtn = ui.getMenuButtonRect(); // Buton yerini UI'dan istiyoruz
                if (SDL_PointInRect(&p, &menuBtn)) {
                    score = 0;
                    timeRemaining = timeLimit;
                    gameStartTime = SDL_GetTicks();
                    ui.updateScore(renderer, score);
                    ui.updateTimer(renderer, timeRemaining);
                    currentState = PLAYING;
                }
            }
            // DURUM 2: Oyun Oynanıyorsa
            else if (currentState == PLAYING) {
                for (int i = 0; i < 9; i++) {
                    if (moles[i].handleInput(mx, my)) { 
                        // Köstebeğe başarıyla vurulduğunda sesi çalma
                        if (hitSound != nullptr) {
                            Mix_PlayChannel(-1, hitSound, 0); 
                        }
                        
                        score += 10; 
                        ui.updateScore(renderer, score); // Skor yazısını ui güncelliyor
                    }
                }
            }
            // DURUM 3: Oyun Bittiyse
            else if (currentState == GAMEOVER) {
                SDL_Rect goBtn = ui.getGameOverRect(); // Buton yerini UI'dan istiyoruz
                if (SDL_PointInRect(&p, &goBtn)) {
                    currentState = MENU;
                }
            }
        }
    }
}

// Oyun mantığını, zamanı ve animasyonları güncelleme
void Game::update() {
    if (currentState == PLAYING) {
        
        // Zaman Kontrolü
        int elapsedSeconds = (SDL_GetTicks() - gameStartTime) / 1000;
        int newTimeRemaining = timeLimit - elapsedSeconds;

        if (newTimeRemaining != timeRemaining) {
            timeRemaining = newTimeRemaining;
            ui.updateTimer(renderer, timeRemaining); // Süre yazısını ui güncelliyor
        }

        if (timeRemaining <= 0) {
            currentState = GAMEOVER;
            for (int i = 0; i < 9; i++) {
                moles[i].hide();
            }
        }
        
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

// Görselleri ekrana çizme
void Game::render() {
    if (currentState == MENU) {
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);
        ui.renderMenu(renderer); // Menü arayüzünü ui çiziyor
    } 
    else if (currentState == PLAYING) {
        SDL_SetRenderDrawColor(renderer, 34, 139, 34, 255);
        SDL_RenderClear(renderer);

        for (int i = 0; i < 9; i++) moles[i].render(renderer); // Köstebekleri çizme
        
        ui.renderPlaying(renderer); // Skor ve süre yazılarını ui çiziyor
    }
    else if (currentState == GAMEOVER) {
        SDL_SetRenderDrawColor(renderer, 100, 0, 0, 255);
        SDL_RenderClear(renderer);
        
        ui.renderGameOver(renderer, score); // Oyun bitti ekranını ui çiziyor
    }

    SDL_RenderPresent(renderer); // Çizimleri ekrana yansıtma
}

// Oyun kapatıldığında hafıza temizleme
void Game::clean() {
    ui.clean(); // Arayüz hafızasını UIManager temizliyor
    
    // Ses dosyasını ve Mixer sistemini hafızadan temizleme
    if (hitSound != nullptr) {
        Mix_FreeChunk(hitSound);
    }
    Mix_CloseAudio();

    SDL_DestroyWindow(window);
    SDL_DestroyRenderer(renderer);
    SDL_Quit();
}

// Oyunun çalışma durumunu döndürme
bool Game::running() { 
    return isRunning; 
}