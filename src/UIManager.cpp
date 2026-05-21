#include "UIManager.h"
#include <iostream>

using namespace std;

bool UIManager::init(SDL_Renderer *renderer, const char *fontPath, int fontSize) {
    if (TTF_Init() == -1) {
        cout << "TTF başlatılamadı! Hata: " << TTF_GetError() << endl;
        return false;
    }

    font = TTF_OpenFont(fontPath, fontSize);
    if (!font) {
        cout << "Font yüklenemedi! Hata: " << TTF_GetError() << endl;
        return false;
    }

    SDL_Color white = {255, 255, 255, 255}; // Beyaz renk tanımlaması

    // Menü hazırlığı
    SDL_Surface *menuSurf = TTF_RenderText_Solid(font, "OYUNA BAŞLAMAK İÇİN TIKLA", white);
    menuTextTexture = SDL_CreateTextureFromSurface(renderer, menuSurf);
    menuButtonRectangle.x = (1280 / 2) - (menuSurf->w / 2);
    menuButtonRectangle.y = (720 / 2) - (menuSurf->h / 2);
    menuButtonRectangle.w = menuSurf->w;
    menuButtonRectangle.h = menuSurf->h;
    SDL_FreeSurface(menuSurf);

    // Game Over hazırlığı
    SDL_Surface *gameOverSurf = TTF_RenderText_Solid(font, "SÜRE BİTTİ! TIKLA VE MENÜDEN BAŞTAN BAŞLA", white);
    gameOverTextTexture = SDL_CreateTextureFromSurface(renderer, gameOverSurf);
    gameOverRectangle.x = (1280 / 2) - (gameOverSurf->w / 2);
    gameOverRectangle.y = (720 / 2) - (gameOverSurf->h / 2);
    gameOverRectangle.w = gameOverSurf->w;
    gameOverRectangle.h = gameOverSurf->h;
    SDL_FreeSurface(gameOverSurf);

    // Skor ve Süre Sabit Yerleri
    scoreRectangle.x = 1000;
    scoreRectangle.y = 30;

    timerRectangle.x = 50;
    timerRectangle.y = 30;

    return true;
}

void UIManager::updateScore(SDL_Renderer* renderer, int score) {
    if (scoreTexture != nullptr) SDL_DestroyTexture(scoreTexture);
    string scoreText = "Skor: " + to_string(score);
    SDL_Surface* textSurface = TTF_RenderText_Solid(font, scoreText.c_str(), {255, 255, 255, 255});
    scoreRectangle.w = textSurface->w;
    scoreRectangle.h = textSurface->h;
    scoreTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
    SDL_FreeSurface(textSurface);
}

void UIManager::updateTimer(SDL_Renderer* renderer, int timeRemaining) {
    if (timerTexture != nullptr) SDL_DestroyTexture(timerTexture);
    string timerText = "Sure: " + to_string(timeRemaining);
    SDL_Surface* textSurface = TTF_RenderText_Solid(font, timerText.c_str(), {255, 255, 255, 255});
    timerRectangle.w = textSurface->w;
    timerRectangle.h = textSurface->h;
    timerTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
    SDL_FreeSurface(textSurface);
}

void UIManager::renderMenu(SDL_Renderer* renderer) {
    SDL_RenderCopy(renderer, menuTextTexture, NULL, &menuButtonRectangle); // Menü metni
}

void UIManager::renderPlaying(SDL_Renderer* renderer) {
    if (scoreTexture) SDL_RenderCopy(renderer, scoreTexture, NULL, &scoreRectangle); // Skor yazısı
    if (timerTexture) SDL_RenderCopy(renderer, timerTexture, NULL, &timerRectangle); // Süre yazısı
}

void UIManager::renderGameOver(SDL_Renderer* renderer, int finalScore) {
    if (scoreTexture) {
        // Skoru Game Over yazısının üstüne çizme
        SDL_Rect finalScoreRectangle = { (1280 / 2) - (scoreRectangle.w / 2), (720 / 2) - 100, scoreRectangle.w, scoreRectangle.h };
        SDL_RenderCopy(renderer, scoreTexture, NULL, &finalScoreRectangle);
    }
    SDL_RenderCopy(renderer, gameOverTextTexture, NULL, &gameOverRectangle); // Bitiş metni
}

void UIManager::clean() {
    SDL_DestroyTexture(menuTextTexture);
    SDL_DestroyTexture(scoreTexture);
    SDL_DestroyTexture(timerTexture);
    SDL_DestroyTexture(gameOverTextTexture);
    if (font) TTF_CloseFont(font);
    TTF_Quit();
}