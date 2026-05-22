#include "UIManager.h"
#include <iostream>

using namespace std;

// Yazılara otomatik gölge ekleyerek her arka planda okunmasını sağlayan yardımcı fonksiyon
static void renderShadowed(SDL_Renderer *renderer, SDL_Texture* texture, SDL_Rect* rect) {
    if (!texture) return;
    
    SDL_Rect shadowRect = {rect->x + 3, rect->y + 3, rect->w, rect->h};
    SDL_SetTextureColorMod(texture, 0, 0, 0);       
    SDL_SetTextureAlphaMod(texture, 150);           
    SDL_RenderCopy(renderer, texture, NULL, &shadowRect);

    SDL_SetTextureColorMod(texture, 255, 255, 255); 
    SDL_SetTextureAlphaMod(texture, 255);           
    SDL_RenderCopy(renderer, texture, NULL, rect);
}

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
    SDL_Color green = {100, 255, 100, 255}; 
    SDL_Color red = {255, 100, 100, 255};

    // Ana Menü Hazırlığı
    // HD ve pürüzsüz yazılar için -> TTF_RenderText_Blended
    SDL_Surface *playSurf = TTF_RenderText_Blended(font, "PLAY", green);
    playButtonTexture = SDL_CreateTextureFromSurface(renderer, playSurf);
    playButtonRect = { (1280 / 2) - (playSurf->w / 2), 300, playSurf->w, playSurf->h };
    SDL_FreeSurface(playSurf);

    SDL_Surface *lbSurf = TTF_RenderText_Blended(font, "HIGH SCORES", white);
    leaderboardButtonTexture = SDL_CreateTextureFromSurface(renderer, lbSurf);
    leaderboardButtonRect = { (1280 / 2) - (lbSurf->w / 2), 380, lbSurf->w, lbSurf->h };
    SDL_FreeSurface(lbSurf);

    SDL_Surface *exitSurf = TTF_RenderText_Blended(font, "EXIT", red);
    exitButtonTexture = SDL_CreateTextureFromSurface(renderer, exitSurf);
    exitButtonRect = { (1280 / 2) - (exitSurf->w / 2), 460, exitSurf->w, exitSurf->h };
    SDL_FreeSurface(exitSurf);

    // Geri Dönüş Butonu
    SDL_Surface *backSurf = TTF_RenderText_Blended(font, "BACK TO MENU", white);
    backButtonTexture = SDL_CreateTextureFromSurface(renderer, backSurf);
    backButtonRect = { (1280 / 2) - (backSurf->w / 2), 580, backSurf->w, backSurf->h };
    SDL_FreeSurface(backSurf);

    // Game Over Hazırlığı
    SDL_Surface *gameOverSurf = TTF_RenderText_Blended(font, "TIME IS UP! CLICK TO CONTINUE", white);
    gameOverTextTexture = SDL_CreateTextureFromSurface(renderer, gameOverSurf);
    gameOverRectangle.x = (1280 / 2) - (gameOverSurf->w / 2);
    gameOverRectangle.y = 480;
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

void UIManager::updateScore(SDL_Renderer *renderer, int score) {
    if (scoreTexture != nullptr) SDL_DestroyTexture(scoreTexture);
    string scoreText = "Score: " + to_string(score);
    SDL_Surface* textSurface = TTF_RenderText_Blended(font, scoreText.c_str(), {255, 255, 255, 255});
    scoreRectangle.w = textSurface->w;
    scoreRectangle.h = textSurface->h;
    scoreTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
    SDL_FreeSurface(textSurface);
}

void UIManager::updateTimer(SDL_Renderer *renderer, int timeRemaining) {
    if (timerTexture != nullptr) SDL_DestroyTexture(timerTexture);
    string timerText = "Time: " + to_string(timeRemaining);
    SDL_Surface* textSurface = TTF_RenderText_Blended(font, timerText.c_str(), {255, 255, 255, 255});
    timerRectangle.w = textSurface->w;
    timerRectangle.h = textSurface->h;
    timerTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
    SDL_FreeSurface(textSurface);
}

// Skor tablolarını hafızada tek seferlik hazırlama
void UIManager::prepareLeaderboard(SDL_Renderer *renderer, int finalScore, const std::vector<int>& topScores) {
    if (leaderBoardTitleTexture) SDL_DestroyTexture(leaderBoardTitleTexture);
    for (int i = 0; i < 5; i++) {
        if (highScoreTextures[i]) SDL_DestroyTexture(highScoreTextures[i]);
    }

    SDL_Color yellow = {255, 255, 0, 255}; 
    SDL_Color white = {255, 255, 255, 255};

    SDL_Surface* titleSurf = TTF_RenderText_Blended(font, "--- TOP 5 SCORES ---", yellow);
    leaderBoardTitleTexture = SDL_CreateTextureFromSurface(renderer, titleSurf);
    leaderBoardTitleRectangle = { (1280 / 2) - (titleSurf->w / 2), 100, titleSurf->w, titleSurf->h };
    SDL_FreeSurface(titleSurf);

    int startY = 170;
    int spacing = 50; 
    for (size_t i = 0; i < topScores.size() && i < 5; i++) {
        string text = to_string(i + 1) + ". Rank: " + to_string(topScores[i]);
        
        if (topScores[i] == finalScore && finalScore > 0) { 
            text += " (YOUR SCORE!)";
        }

        SDL_Surface *scoreSurf = TTF_RenderText_Blended(font, text.c_str(), white);
        highScoreTextures[i] = SDL_CreateTextureFromSurface(renderer, scoreSurf);
        highScoreRectangles[i] = { (1280 / 2) - (scoreSurf->w / 2), startY + (static_cast<int>(i) * spacing), scoreSurf->w, scoreSurf->h };
        SDL_FreeSurface(scoreSurf);
    }
}

void UIManager::renderMenu(SDL_Renderer *renderer) {
    renderShadowed(renderer, playButtonTexture, &playButtonRect);
    renderShadowed(renderer, leaderboardButtonTexture, &leaderboardButtonRect);
    renderShadowed(renderer, exitButtonTexture, &exitButtonRect);
}

void UIManager::renderPlaying(SDL_Renderer* renderer) {
    if (scoreTexture) renderShadowed(renderer, scoreTexture, &scoreRectangle); // Skor yazısı
    if (timerTexture) renderShadowed(renderer, timerTexture, &timerRectangle); // Süre yazısı
}

void UIManager::renderHighScores(SDL_Renderer* renderer) {
    if (leaderBoardTitleTexture) renderShadowed(renderer, leaderBoardTitleTexture, &leaderBoardTitleRectangle);
    for (int i = 0; i < 5; i++) {
        if (highScoreTextures[i]) {
            renderShadowed(renderer, highScoreTextures[i], &highScoreRectangles[i]);
        }
    }
    renderShadowed(renderer, backButtonTexture, &backButtonRect); // Geri dön butonu
}

void UIManager::renderGameOver(SDL_Renderer* renderer) {
    if (scoreTexture) {
        // Skoru Game Over yazısının üstüne çizme
        SDL_Rect finalScoreRectangle = { (1280 / 2) - (scoreRectangle.w / 2), 40, scoreRectangle.w, scoreRectangle.h };
        renderShadowed(renderer, scoreTexture, &finalScoreRectangle);
    }

    renderHighScores(renderer); // Liderlik tablosunu ve geri dön butonunu çizme
    renderShadowed(renderer, gameOverTextTexture, &gameOverRectangle); // Bitiş metni
}

void UIManager::clean() {
    SDL_DestroyTexture(playButtonTexture);
    SDL_DestroyTexture(leaderboardButtonTexture);
    SDL_DestroyTexture(exitButtonTexture);
    SDL_DestroyTexture(backButtonTexture);
    SDL_DestroyTexture(scoreTexture);
    SDL_DestroyTexture(timerTexture);
    SDL_DestroyTexture(gameOverTextTexture);
    
    // Eklenen yeni skor dokularını temizleme
    if (leaderBoardTitleTexture) SDL_DestroyTexture(leaderBoardTitleTexture);
    for (int i = 0; i < 5; i++) {
        if (highScoreTextures[i]) SDL_DestroyTexture(highScoreTextures[i]);
    }

    if (font) TTF_CloseFont(font);
    TTF_Quit();
}