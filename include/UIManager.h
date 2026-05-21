#ifndef UI_MANAGER_H
#define UI_MANAGER_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <string>

class UIManager {
public:
    // UI Sistemini başlatma ve fontu yükleme
    bool init(SDL_Renderer *renderer, const char *fontPath, int fontSize);
    
    // Skor, Süre ve Menü yazılarını güncelleyen fonksiyonlar
    void updateScore(SDL_Renderer *renderer, int score);
    void updateTimer(SDL_Renderer *renderer, int timeRemaining);

    // Çizim fonksiyonları
    void renderMenu(SDL_Renderer *renderer);
    void renderPlaying(SDL_Renderer *renderer);
    void renderGameOver(SDL_Renderer *renderer, int finalScore);

    // Hafıza temizleme
    void clean();

    // Buton koordinatlarını dışarıya açan fonksiyonlar (Tıklama kontrolü için)
    SDL_Rect getMenuButtonRect() const { 
        return menuButtonRectangle; 
    }
    SDL_Rect getGameOverRect() const { 
        return gameOverRectangle; 
    }

private:
    TTF_Font *font = nullptr;

    SDL_Texture *scoreTexture = nullptr;
    SDL_Rect scoreRectangle;

    SDL_Texture *timerTexture = nullptr;
    SDL_Rect timerRectangle;

    SDL_Texture *menuTextTexture = nullptr;
    SDL_Rect menuButtonRectangle;

    SDL_Texture *gameOverTextTexture = nullptr;
    SDL_Rect gameOverRectangle;
};

#endif