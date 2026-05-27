#ifndef UI_MANAGER_H
#define UI_MANAGER_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <vector>

struct Effect {
    bool active = false;
    int x = 0;
    int y = 0;
    Uint32 startTime = 0;
};

class UIManager {
public:
    // UI sistemini başlatma ve fontu yükleme
    bool init(SDL_Renderer *renderer, const char *fontPath, int fontSize);

    // Skor, süre ve menü yazılarını güncelleyen fonksiyonlar
    void updateScore(SDL_Renderer *renderer, int score);
    void updateTimer(SDL_Renderer *renderer, int timeRemaining);

    // Oyun bittiğinde en yüksek 5 skoru ekrana çizilmek üzere hazırlama
    void updateLeaderboard(SDL_Renderer *renderer, int finalScore, std::vector<int> topScores);

    // Çizim fonksiyonları
    void renderMenu(SDL_Renderer *renderer);
    void renderPlaying(SDL_Renderer *renderer);
    void renderGameOver(SDL_Renderer *renderer);
    void renderHighScores(SDL_Renderer *renderer);
    void renderEffects(SDL_Renderer *renderer);

    // Kısa süreli vuruş efektini yönetme
    void showHitEffect(int x, int y);
    void updateHitEffects();
    void clearHitEffects();

    // Hafıza temizleme
    void clean();

    // Buton koordinatlarını dışarıya açan fonksiyonlar
    SDL_Rect getPlayButtonRectangle() {
        return playButtonRectangle;
    }

    SDL_Rect getLeaderboardButtonRectangle() {
        return leaderboardButtonRectangle;
    }

    SDL_Rect getExitButtonRectangle() {
        return exitButtonRectangle;
    }

    SDL_Rect getBackButtonRectangle() {
        return backButtonRectangle;
    }

    SDL_Rect getBackIconRectangle() {
        return backIconRectangle;
    }

    SDL_Rect getGameOverRectangle() {
        return gameOverRectangle;
    }

private:
    void drawLeaderboardRows(SDL_Renderer *renderer, int startX, int startY, int panelWidth);

    TTF_Font *font = nullptr;

    SDL_Texture *scoreTexture = nullptr;
    SDL_Rect scoreRectangle;

    SDL_Texture *timerTexture = nullptr;
    SDL_Rect timerRectangle;

    SDL_Texture *mainTitleTexture = nullptr;
    SDL_Rect mainTitleRectangle;

    // Ana menü butonları
    SDL_Texture *playButtonTexture = nullptr;
    SDL_Rect playButtonRectangle;
    SDL_Texture *leaderboardButtonTexture = nullptr;
    SDL_Rect leaderboardButtonRectangle;
    SDL_Texture *exitButtonTexture = nullptr;
    SDL_Rect exitButtonRectangle;

    // Geri dönüş butonu
    SDL_Texture *backButtonTexture = nullptr;
    SDL_Rect backButtonRectangle;

    // Oyun içi sol üst çıkış ikonu
    SDL_Texture *backIconTexture = nullptr;
    SDL_Rect backIconRectangle;

    SDL_Texture *gameOverTextTexture = nullptr;
    SDL_Rect gameOverRectangle;
    SDL_Texture *resultsTitleTexture = nullptr;
    SDL_Rect resultsTitleRectangle;

    // En yüksek 5 skorun ekrandaki resimleri ve yerleri
    SDL_Texture *highScoreTextures[5] = {nullptr};
    SDL_Rect highScoreRectangles[5];
    SDL_Texture *leaderBoardTitleTexture = nullptr;
    SDL_Rect leaderBoardTitleRectangle;

    SDL_Texture *floatingScoreTexture = nullptr;
    SDL_Rect floatingScoreRectangle;

    Effect floatingTexts[6];
    Effect hitSparks[6];

    int scorePulseAmount = 0;
    int currentTimeRemaining = 30;
};

#endif
