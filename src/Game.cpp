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
        
        // Font sistemini başlatma ve yükleme
        TTF_Init();
        font = TTF_OpenFont("../assets/arial.ttf", 32);
        
        // Ses Sistemini Başlatma (Mixer)
        if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
            cout << "Ses sistemi başlatılamadı! Hata: " << Mix_GetError() << endl;
        }
        
        // Vuruş sesini hafızaya yükleme
        hitSound = Mix_LoadWAV("../assets/hit.wav");
        if (hitSound == nullptr) {
            cout << "hit.wav dosyası yüklenemedi! Hata: " << Mix_GetError() << endl;
        }
        
        SDL_Color white = {255, 255, 255, 255}; // Beyaz renk tanımlaması

        // Menü yazısını resme dönüştürme
        SDL_Surface *menuSurf = TTF_RenderText_Solid(font, "OYUNA BAŞLAMAK İÇİN TIKLA", white);
        menuTextTexture = SDL_CreateTextureFromSurface(renderer, menuSurf);
        // Yazı hizalama
        menuButtonRectangle.x = (1280 / 2) - (menuSurf->w / 2);
        menuButtonRectangle.y = (720 / 2) - (menuSurf->h / 2);
        menuButtonRectangle.w = menuSurf->w;
        menuButtonRectangle.h = menuSurf->h;
        SDL_FreeSurface(menuSurf); // Hafızayı temizlememe

        SDL_Surface *gameOverSurf = TTF_RenderText_Solid(font, "SÜRE BİTTİ! TIKLA VE MENÜDEN BAŞTAN BAŞLA", white);
        gameOverTextTexture = SDL_CreateTextureFromSurface(renderer, gameOverSurf);
        gameOverRectangle.x = (1280 / 2) - (gameOverSurf->w / 2);
        gameOverRectangle.y = (720 / 2) - (gameOverSurf->h / 2);
        gameOverRectangle.w = gameOverSurf->w;
        gameOverRectangle.h = gameOverSurf->h;
        SDL_FreeSurface(gameOverSurf);

        // Skor ve Süre Hazırlığı
        scoreRectangle.x = 1000; 
        scoreRectangle.y = 30;
        updateScoreText(); 

        timerRectangle.x = 50;  
        timerRectangle.y = 30;
        updateTimerText();

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

// Skoru metin olarak güncelleme ve hafızaya alma
void Game::updateScoreText() {
    if (scoreTexture != nullptr) SDL_DestroyTexture(scoreTexture); 
    string scoreText = "Skor: " + to_string(score);
    SDL_Surface *textSurface = TTF_RenderText_Solid(font, scoreText.c_str(), {255, 255, 255, 255});
    scoreRectangle.w = textSurface->w;
    scoreRectangle.h = textSurface->h;
    scoreTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
    SDL_FreeSurface(textSurface); 
}

// Kalan süreyi metin olarak güncelleme ve hafızaya alma
void Game::updateTimerText() {
    if (timerTexture != nullptr) SDL_DestroyTexture(timerTexture);
    string timerText = "Sure: " + to_string(timeRemaining);
    SDL_Surface *textSurface = TTF_RenderText_Solid(font, timerText.c_str(), {255, 255, 255, 255});
    timerRectangle.w = textSurface->w;
    timerRectangle.h = textSurface->h;
    timerTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
    SDL_FreeSurface(textSurface); 
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
                if (SDL_PointInRect(&p, &menuButtonRectangle)) {
                    score = 0;
                    timeRemaining = timeLimit;
                    gameStartTime = SDL_GetTicks();
                    updateScoreText();
                    updateTimerText();
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
                        updateScoreText();
                    }
                }
            }
            // DURUM 3: Oyun Bittiyse
            else if (currentState == GAMEOVER) {
                if (SDL_PointInRect(&p, &gameOverRectangle)) {
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
            updateTimerText(); 
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
        SDL_RenderCopy(renderer, menuTextTexture, NULL, &menuButtonRectangle); // Menü metni
    } 
    else if (currentState == PLAYING) {
        SDL_SetRenderDrawColor(renderer, 34, 139, 34, 255);
        SDL_RenderClear(renderer);

        for (int i = 0; i < 9; i++) moles[i].render(renderer); // Köstebekleri çizme
        
        if (scoreTexture) SDL_RenderCopy(renderer, scoreTexture, NULL, &scoreRectangle); // Skor yazısı
        if (timerTexture) SDL_RenderCopy(renderer, timerTexture, NULL, &timerRectangle); // Süre yazısı
    }
    else if (currentState == GAMEOVER) {
        SDL_SetRenderDrawColor(renderer, 100, 0, 0, 255);
        SDL_RenderClear(renderer);
        
        if (scoreTexture) {
            // Skoru Game Over yazısının üstüne çizme
            SDL_Rect finalScoreRectangle = { (1280/2) - (scoreRectangle.w/2), (720/2) - 100, scoreRectangle.w, scoreRectangle.h };
            SDL_RenderCopy(renderer, scoreTexture, NULL, &finalScoreRectangle);
        }
        SDL_RenderCopy(renderer, gameOverTextTexture, NULL, &gameOverRectangle); // Bitiş metni
    }

    SDL_RenderPresent(renderer); // Çizimleri ekrana yansıtma
}

// Oyun kapatıldığında hafıza temizleme
void Game::clean() {
    SDL_DestroyTexture(menuTextTexture);
    SDL_DestroyTexture(scoreTexture);
    SDL_DestroyTexture(timerTexture);
    SDL_DestroyTexture(gameOverTextTexture);
    
    // Ses dosyasını ve Mixer sistemini hafızadan temizleme
    if (hitSound != nullptr) {
        Mix_FreeChunk(hitSound);
    }
    Mix_CloseAudio();
    
    TTF_CloseFont(font);
    TTF_Quit();

    SDL_DestroyWindow(window);
    SDL_DestroyRenderer(renderer);
    SDL_Quit();
}

// Oyunun çalışma durumunu döndürme
bool Game::running() { 
    return isRunning; 
}