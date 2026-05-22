#ifndef UI_MANAGER_H
#define UI_MANAGER_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <string>
#include <vector> // Skor listesi için eklendi

class UIManager {
public:
    // UI Sistemini başlatma ve fontu yükleme
    bool init(SDL_Renderer *renderer, const char *fontPath, int fontSize);
    
    // Skor, Süre ve Menü yazılarını güncelleyen fonksiyonlar
    void updateScore(SDL_Renderer *renderer, int score);
    void updateTimer(SDL_Renderer *renderer, int timeRemaining);

    // Oyun bittiğinde en yüksek 5 skoru ekrana çizilmek üzere hazırlar
    void prepareLeaderboard(SDL_Renderer *renderer, int finalScore, const std::vector<int>& topScores);

    // Çizim fonksiyonları
    void renderMenu(SDL_Renderer *renderer);
    void renderPlaying(SDL_Renderer *renderer);
    void renderGameOver(SDL_Renderer *renderer); 
    void renderHighScores(SDL_Renderer *renderer); // Skor ekranını çizer

    // Hafıza temizleme
    void clean();

    // Buton koordinatlarını dışarıya açan fonksiyonlar (Tıklama kontrolü için)
    SDL_Rect getPlayButtonRect() const { return playButtonRect; }
    SDL_Rect getLeaderboardButtonRect() const { return leaderboardButtonRect; }
    SDL_Rect getExitButtonRect() const { return exitButtonRect; }
    SDL_Rect getBackButtonRect() const { return backButtonRect; }
    SDL_Rect getGameOverRect() const { return gameOverRectangle; }

private:
    TTF_Font *font = nullptr;

    SDL_Texture *scoreTexture = nullptr;
    SDL_Rect scoreRectangle;

    SDL_Texture *timerTexture = nullptr;
    SDL_Rect timerRectangle;

    // Ana Menü Butonları
    SDL_Texture *playButtonTexture = nullptr;
    SDL_Rect playButtonRect;
    SDL_Texture *leaderboardButtonTexture = nullptr;
    SDL_Rect leaderboardButtonRect;
    SDL_Texture *exitButtonTexture = nullptr;
    SDL_Rect exitButtonRect;

    // Geri Dönüş Butonu
    SDL_Texture *backButtonTexture = nullptr;
    SDL_Rect backButtonRect;

    SDL_Texture *gameOverTextTexture = nullptr;
    SDL_Rect gameOverRectangle;

    // En yüksek 5 skorun ekrandaki resimleri ve yerleri
    SDL_Texture *highScoreTextures[5] = { nullptr };
    SDL_Rect highScoreRectangles[5];
    SDL_Texture *leaderBoardTitleTexture = nullptr;
    SDL_Rect leaderBoardTitleRectangle;
};

#endif