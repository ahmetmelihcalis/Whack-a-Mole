#ifndef UI_MANAGER_H
#define UI_MANAGER_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <string>
#include <vector> // Skor listesi için eklendi

class UIManager {
public:
    // UI sistemini başlatma ve fontu yükleme
    bool init(SDL_Renderer *renderer, const char *fontPath, int fontSize);
    
    // Skor, süre ve menü yazılarını güncelleyen fonksiyonlar
    void updateScore(SDL_Renderer *renderer, int score);
    void updateTimer(SDL_Renderer *renderer, int timeRemaining);

    // Oyun bittiğinde en yüksek 5 skoru ekrana çizilmek üzere hazırlama
    void prepareLeaderboard(SDL_Renderer *renderer, int finalScore, const std::vector<int>& topScores);

    // Çizim fonksiyonları
    void renderMenu(SDL_Renderer *renderer);
    void renderPlaying(SDL_Renderer *renderer);
    void renderGameOver(SDL_Renderer *renderer);
    void renderHighScores(SDL_Renderer *renderer); // Skor ekranını çizme
    void renderEffects(SDL_Renderer *renderer);

    // Kısa süreli vuruş efektini yönetme
    void addHitEffect(int x, int y);
    void updateEffects();
    void clearEffects();

    // Hafıza temizleme
    void clean();

    // Buton koordinatlarını dışarıya açan fonksiyonlar
    SDL_Rect getPlayButtonRect() const { return playButtonRect; }
    SDL_Rect getLeaderboardButtonRect() const { return leaderboardButtonRect; }
    SDL_Rect getExitButtonRect() const { return exitButtonRect; }
    SDL_Rect getBackButtonRect() const { return backButtonRect; }
    SDL_Rect getGameOverRect() const { return gameOverRectangle; }

private:
    void renderLeaderboardRows(SDL_Renderer *renderer, int startX, int startY, int panelWidth);

    struct FloatingTextEffect {
        bool active = false;
        int x = 0;
        int y = 0;
        Uint32 startTime = 0;
    };

    struct HitSparkEffect {
        bool active = false;
        int x = 0;
        int y = 0;
        Uint32 startTime = 0;
    };

    TTF_Font *font = nullptr;

    SDL_Texture *scoreTexture = nullptr;
    SDL_Rect scoreRectangle;

    SDL_Texture *timerTexture = nullptr;
    SDL_Rect timerRectangle;

    SDL_Texture *mainTitleTexture = nullptr;
    SDL_Rect mainTitleRect;

    // Ana menü butonları
    SDL_Texture *playButtonTexture = nullptr;
    SDL_Rect playButtonRect;
    int playTextWidth = 0;
    int playTextHeight = 0;
    SDL_Texture *leaderboardButtonTexture = nullptr;
    SDL_Rect leaderboardButtonRect;
    int leaderboardTextWidth = 0;
    int leaderboardTextHeight = 0;
    SDL_Texture *exitButtonTexture = nullptr;
    SDL_Rect exitButtonRect;
    int exitTextWidth = 0;
    int exitTextHeight = 0;

    // Geri dönüş butonu
    SDL_Texture *backButtonTexture = nullptr;
    SDL_Rect backButtonRect;
    int backTextWidth = 0;
    int backTextHeight = 0;

    SDL_Texture *gameOverTextTexture = nullptr;
    SDL_Rect gameOverRectangle;
    SDL_Texture *resultsTitleTexture = nullptr;
    SDL_Rect resultsTitleRectangle;

    // En yüksek 5 skorun ekrandaki resimleri ve yerleri
    SDL_Texture *highScoreTextures[5] = {
        nullptr
    };
    SDL_Rect highScoreRectangles[5];
    SDL_Texture *leaderBoardTitleTexture = nullptr;
    SDL_Rect leaderBoardTitleRectangle;

    SDL_Texture *floatingScoreTexture = nullptr;
    SDL_Rect floatingScoreRect;
    FloatingTextEffect floatingTexts[6];
    HitSparkEffect hitSparks[6];
    int scorePulseAmount = 0;
    int currentTimeRemaining = 30;
};

#endif
