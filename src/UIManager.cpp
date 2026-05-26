#include "UIManager.h"
#include <iostream>
#include <string>

using namespace std;

// Texture'ı güvenli silme
static SDL_Texture *safeDestroyTexture(SDL_Texture *texture) {
    if (texture != nullptr) {
        SDL_DestroyTexture(texture);
    }
    return nullptr;
}

// Yazı texture'ı oluşturma
static SDL_Texture *createTextTexture(SDL_Renderer *renderer, TTF_Font *font, string text, SDL_Color color, SDL_Rect *outRectangle = nullptr) {
    SDL_Surface *surface = TTF_RenderText_Blended(font, text.c_str(), color);
    if (surface == nullptr) {
        return nullptr;
    }
    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
    if (outRectangle != nullptr) {
        outRectangle->w = surface->w;
        outRectangle->h = surface->h;
    }
    SDL_FreeSurface(surface);
    return texture;
}

// Efekti aktif etme
static void activateEffect(Effect effects[], int count, int x, int y) {
    for (int i = 0; i < count; i++) {
        if (!effects[i].active) {
            effects[i].active = true;
            effects[i].x = x;
            effects[i].y = y;
            effects[i].startTime = SDL_GetTicks();
            break;
        }
    }
}

// Yazıların her arka planda okunmasını sağlama
static void drawShadowText(SDL_Renderer *renderer, SDL_Texture *texture, SDL_Rect *rectangle, Uint8 alpha = 255) {
    if (texture == nullptr) {
        return;
    }

    SDL_Rect shadowRectangle = {rectangle->x + 3, rectangle->y + 3, rectangle->w, rectangle->h};
    SDL_SetTextureColorMod(texture, 0, 0, 0);
    Uint8 shadowAlpha = alpha;
    if (shadowAlpha > 150) {
        shadowAlpha = 150;
    }
    SDL_SetTextureAlphaMod(texture, shadowAlpha);
    SDL_RenderCopy(renderer, texture, NULL, &shadowRectangle);

    SDL_SetTextureColorMod(texture, 255, 255, 255);
    SDL_SetTextureAlphaMod(texture, alpha);
    SDL_RenderCopy(renderer, texture, NULL, rectangle);
}

// Texture'ı ortalayarak çizme
static void drawCenteredAt(SDL_Renderer *renderer, SDL_Texture *texture, SDL_Rect rectangle, int y) {
    if (texture != nullptr) {
        rectangle.x = (1280 / 2) - (rectangle.w / 2);
        rectangle.y = y;
        drawShadowText(renderer, texture, &rectangle);
    }
}

// Panelleri tek tip bir görünümle çizme
static void drawPanel(SDL_Renderer *renderer, SDL_Rect rectangle, SDL_Color fillColor, SDL_Color borderColor) {
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

    SDL_Rect shadowRectangle = {rectangle.x + 4, rectangle.y + 5, rectangle.w, rectangle.h};
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 46);
    SDL_RenderFillRect(renderer, &shadowRectangle);

    SDL_SetRenderDrawColor(renderer, fillColor.r, fillColor.g, fillColor.b, fillColor.a);
    SDL_RenderFillRect(renderer, &rectangle);

    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 18);
    SDL_Rect topHighlight = {rectangle.x + 2, rectangle.y + 2, rectangle.w - 4, 4};
    SDL_RenderFillRect(renderer, &topHighlight);

    if (borderColor.a > 0) {
        SDL_SetRenderDrawColor(renderer, borderColor.r, borderColor.g, borderColor.b, borderColor.a);
        SDL_RenderDrawLine(renderer, rectangle.x + 2, rectangle.y + rectangle.h - 1, rectangle.x + rectangle.w - 3, rectangle.y + rectangle.h - 1);
    }
}

// Butonu panel ve yazısıyla birlikte çizme
static void drawButton(SDL_Renderer *renderer, SDL_Rect rectangle, SDL_Color fillColor, SDL_Color borderColor,
                       SDL_Texture *texture, bool isHovered) {
    if (isHovered) {
        rectangle.x -= 8;
        rectangle.y -= 4;
        rectangle.w += 16;
        rectangle.h += 8;
    }

    drawPanel(renderer, rectangle, fillColor, borderColor);

    int textWidth = 0;
    int textHeight = 0;
    SDL_QueryTexture(texture, nullptr, nullptr, &textWidth, &textHeight);

    SDL_Rect textRectangle = {
        rectangle.x + (rectangle.w - textWidth) / 2,
        rectangle.y + (rectangle.h - textHeight) / 2,
        textWidth,
        textHeight
    };
    drawShadowText(renderer, texture, &textRectangle);
}

// UI sistemini başlatma ve gerekli yazıları hazırlama
bool UIManager::init(SDL_Renderer *renderer, const char *fontPath, int fontSize) {
    if (TTF_Init() == -1) {
        cout << "Failed to initialize TTF. Error: " << TTF_GetError() << endl;
        return false;
    }

    font = TTF_OpenFont(fontPath, fontSize);
    if (font == nullptr) {
        cout << "Failed to load font. Error: " << TTF_GetError() << endl;
        return false;
    }

    TTF_SetFontStyle(font, TTF_STYLE_BOLD);

    SDL_Color white = {255, 255, 255, 255};
    SDL_Color green = {120, 255, 140, 255};
    SDL_Color red = {255, 130, 130, 255};
    SDL_Color yellow = {255, 232, 138, 255};

    SDL_Surface *surface = IMG_Load("../assets/main_text.png");
    if (surface != nullptr) {
        mainTitleTexture = SDL_CreateTextureFromSurface(renderer, surface);
        mainTitleRectangle.w = 460;
        mainTitleRectangle.h = (surface->h * mainTitleRectangle.w) / surface->w;
        mainTitleRectangle.x = (1280 / 2) - (mainTitleRectangle.w / 2);
        mainTitleRectangle.y = 70;
        SDL_FreeSurface(surface);
    }

    playButtonRectangle = {(1280 / 2) - 170, 300, 340, 66};
    playButtonTexture = createTextTexture(renderer, font, "PLAY", green);

    leaderboardButtonRectangle = {(1280 / 2) - 170, 380, 340, 66};
    leaderboardButtonTexture = createTextTexture(renderer, font, "HIGH SCORES", white);

    exitButtonRectangle = {(1280 / 2) - 170, 460, 340, 66};
    exitButtonTexture = createTextTexture(renderer, font, "EXIT", red);

    backButtonRectangle = {(1280 / 2) - 170, 524, 340, 66};
    backButtonTexture = createTextTexture(renderer, font, "BACK TO MENU", white);

    backIconTexture = createTextTexture(renderer, font, "<-", white, &backIconRectangle);
    backIconRectangle.x = 40;
    backIconRectangle.y = 30;

    resultsTitleTexture = createTextTexture(renderer, font, "ROUND RESULTS", yellow, &resultsTitleRectangle);
    resultsTitleRectangle.x = (1280 / 2) - (resultsTitleRectangle.w / 2);
    resultsTitleRectangle.y = 138;

    gameOverTextTexture = createTextTexture(renderer, font, "TIME IS UP", yellow, &gameOverRectangle);
    gameOverRectangle.x = (1280 / 2) - (gameOverRectangle.w / 2);
    gameOverRectangle.y = 194;

    floatingScoreTexture = createTextTexture(renderer, font, "+10", yellow, &floatingScoreRectangle);

    scoreRectangle.x = 1000;
    scoreRectangle.y = 30;
    timerRectangle.x = 150;
    timerRectangle.y = 30;

    return true;
}

// Skor yazısını güncelleme
void UIManager::updateScore(SDL_Renderer *renderer, int score) {
    scoreTexture = safeDestroyTexture(scoreTexture);
    scoreTexture = createTextTexture(renderer, font, "Score: " + to_string(score), {255, 255, 255, 255}, &scoreRectangle);
    scoreRectangle.x = 1000;
    scoreRectangle.y = 30;
    scorePulseAmount = 6;
}

// Süre yazısını güncelleme
void UIManager::updateTimer(SDL_Renderer *renderer, int timeRemaining) {
    timerTexture = safeDestroyTexture(timerTexture);
    currentTimeRemaining = timeRemaining;

    SDL_Color timerColor = {255, 255, 255, 255};
    if (timeRemaining <= 10) {
        timerColor = {255, 110, 110, 255};
    }

    timerTexture = createTextTexture(renderer, font, "Time: " + to_string(timeRemaining), timerColor, &timerRectangle);
    timerRectangle.x = 150;
    timerRectangle.y = 30;
}

// Skor tablosunu hafızada tek seferlik hazırlama
void UIManager::updateLeaderboard(SDL_Renderer *renderer, int finalScore, std::vector<int> topScores) {
    leaderBoardTitleTexture = safeDestroyTexture(leaderBoardTitleTexture);
    for (int i = 0; i < 5; i++) {
        highScoreTextures[i] = safeDestroyTexture(highScoreTextures[i]);
    }

    SDL_Color yellow = {255, 255, 0, 255};
    SDL_Color white = {255, 255, 255, 255};

    leaderBoardTitleTexture = createTextTexture(renderer, font, "TOP SCORES", yellow, &leaderBoardTitleRectangle);
    leaderBoardTitleRectangle.x = (1280 / 2) - (leaderBoardTitleRectangle.w / 2);
    leaderBoardTitleRectangle.y = 100;

    bool didMarkPlayerScore = false;
    for (int i = 0; i < 5; i++) {
        string text = to_string(i + 1) + ". Rank: " + to_string(topScores[i]);

        if (topScores[i] == finalScore && finalScore > 0 && !didMarkPlayerScore) {
            text += " [YOU]";
            didMarkPlayerScore = true;
        }

        highScoreTextures[i] = createTextTexture(renderer, font, text, white, &highScoreRectangles[i]);
        highScoreRectangles[i].x = (1280 / 2) - (highScoreRectangles[i].w / 2);
        highScoreRectangles[i].y = 170 + (i * 50);
    }
}

// Skor satırlarını aynı düzenle ekrana çizme
void UIManager::drawLeaderboardRows(SDL_Renderer *renderer, int startX, int startY, int panelWidth) {
    for (int i = 0; i < 5; i++) {
        if (highScoreTextures[i] == nullptr) {
            continue;
        }

        SDL_Rect rowRectangle = {startX + 26, startY + (i * 44), panelWidth - 52, 38};
        drawPanel(renderer, rowRectangle, {18, 22, 18, 128}, {255, 235, 160, 70});

        SDL_Rect textRectangle = {
            rowRectangle.x + 18,
            rowRectangle.y + (rowRectangle.h - highScoreRectangles[i].h) / 2,
            highScoreRectangles[i].w,
            highScoreRectangles[i].h
        };
        drawShadowText(renderer, highScoreTextures[i], &textRectangle);
    }
}

// Ana menüyü çizme
void UIManager::renderMenu(SDL_Renderer *renderer) {
    int mouseX = 0;
    int mouseY = 0;
    SDL_GetMouseState(&mouseX, &mouseY);
    SDL_Point mousePoint = {mouseX, mouseY};

    if (mainTitleTexture != nullptr) {
        SDL_RenderCopy(renderer, mainTitleTexture, NULL, &mainTitleRectangle);
    }

    drawButton(renderer, playButtonRectangle, {22, 28, 24, 132}, {0, 0, 0, 0},
               playButtonTexture, SDL_PointInRect(&mousePoint, &playButtonRectangle));

    drawButton(renderer, leaderboardButtonRectangle, {22, 28, 24, 132}, {0, 0, 0, 0},
               leaderboardButtonTexture, SDL_PointInRect(&mousePoint, &leaderboardButtonRectangle));

    drawButton(renderer, exitButtonRectangle, {22, 28, 24, 132}, {0, 0, 0, 0},
               exitButtonTexture, SDL_PointInRect(&mousePoint, &exitButtonRectangle));
}

// Oyun içindeki skor ve süre panellerini çizme
void UIManager::renderPlaying(SDL_Renderer *renderer) {
    SDL_Rect scorePanel = {scoreRectangle.x - 20, scoreRectangle.y - 12, scoreRectangle.w + 40, scoreRectangle.h + 24};
    SDL_Rect timerPanel = {timerRectangle.x - 20, timerRectangle.y - 12, timerRectangle.w + 40, timerRectangle.h + 24};

    if (scorePulseAmount > 0) {
        scorePanel.x -= scorePulseAmount / 2;
        scorePanel.y -= scorePulseAmount / 2;
        scorePanel.w += scorePulseAmount;
        scorePanel.h += scorePulseAmount;
        scorePulseAmount--;
    }

    SDL_Color timerBorder = {255, 255, 255, 110};
    if (currentTimeRemaining <= 10) {
        timerBorder = {255, 110, 110, 180};
    }

    drawPanel(renderer, scorePanel, {14, 18, 18, 150}, {255, 235, 160, 165});
    drawPanel(renderer, timerPanel, {14, 18, 18, 150}, timerBorder);

    if (scoreTexture != nullptr) {
        drawShadowText(renderer, scoreTexture, &scoreRectangle);
    }
    if (timerTexture != nullptr) {
        drawShadowText(renderer, timerTexture, &timerRectangle);
    }

    if (backIconTexture != nullptr) {
        int mouseX = 0;
        int mouseY = 0;
        SDL_GetMouseState(&mouseX, &mouseY);
        SDL_Point mousePoint = {mouseX, mouseY};
        
        SDL_Rect buttonPanel = {backIconRectangle.x - 12, backIconRectangle.y - 12, backIconRectangle.w + 24, backIconRectangle.h + 24};
        bool isHovered = SDL_PointInRect(&mousePoint, &buttonPanel);
        
        if (isHovered) {
            buttonPanel.x -= 2;
            buttonPanel.y -= 2;
            buttonPanel.w += 4;
            buttonPanel.h += 4;
        }
        
        drawPanel(renderer, buttonPanel, {14, 18, 18, 150}, {255, 255, 255, 100});
        drawShadowText(renderer, backIconTexture, &backIconRectangle);
    }
}

// Ayrı yüksek skor ekranını çizme
void UIManager::renderHighScores(SDL_Renderer *renderer) {
    SDL_Rect boardPanel = {(1280 / 2) - 250, 120, 500, 360};
    drawPanel(renderer, boardPanel, {14, 18, 18, 170}, {255, 235, 160, 85});

    drawCenteredAt(renderer, leaderBoardTitleTexture, leaderBoardTitleRectangle, boardPanel.y + 24);

    drawLeaderboardRows(renderer, boardPanel.x, boardPanel.y + 78, boardPanel.w);

    int mouseX = 0;
    int mouseY = 0;
    SDL_GetMouseState(&mouseX, &mouseY);
    SDL_Point mousePoint = {mouseX, mouseY};

    drawButton(renderer, backButtonRectangle, {18, 22, 18, 178}, {255, 235, 160, 190},
               backButtonTexture, SDL_PointInRect(&mousePoint, &backButtonRectangle));
}

// Süre bitince açılan sonuç ekranını çizme
void UIManager::renderGameOver(SDL_Renderer *renderer) {
    SDL_Rect resultCard = {(1280 / 2) - 300, 56, 600, 600};
    drawPanel(renderer, resultCard, {16, 18, 20, 176}, {255, 220, 140, 80});

    drawCenteredAt(renderer, resultsTitleTexture, resultsTitleRectangle, resultCard.y + 26);

    gameOverRectangle = {(1280 / 2) - (gameOverRectangle.w / 2), resultCard.y + 98, gameOverRectangle.w, gameOverRectangle.h};
    drawShadowText(renderer, gameOverTextTexture, &gameOverRectangle);

    if (scoreTexture != nullptr) {
        SDL_Rect finalScorePanel = {(1280 / 2) - (scoreRectangle.w / 2) - 20, resultCard.y + 162, scoreRectangle.w + 40, scoreRectangle.h + 22};
        drawPanel(renderer, finalScorePanel, {32, 18, 18, 142}, {255, 205, 120, 70});

        SDL_Rect finalScoreRectangle = {(1280 / 2) - (scoreRectangle.w / 2), resultCard.y + 174, scoreRectangle.w, scoreRectangle.h};
        drawShadowText(renderer, scoreTexture, &finalScoreRectangle);
    }

    drawCenteredAt(renderer, leaderBoardTitleTexture, leaderBoardTitleRectangle, resultCard.y + 248);

    for (int i = 0; i < 5; i++) {
        if (highScoreTextures[i] == nullptr) {
            continue;
        }

        SDL_Rect textRectangle = {
            resultCard.x + 72,
            resultCard.y + 306 + (i * 46),
            highScoreRectangles[i].w,
            highScoreRectangles[i].h
        };
        drawShadowText(renderer, highScoreTextures[i], &textRectangle);
    }

    int mouseX = 0;
    int mouseY = 0;
    SDL_GetMouseState(&mouseX, &mouseY);
    SDL_Point mousePoint = {mouseX, mouseY};

    backButtonRectangle = {(1280 / 2) - 155, resultCard.y + 526, 300, 54};
    drawButton(renderer, backButtonRectangle, {18, 22, 18, 160}, {255, 235, 160, 90},
               backButtonTexture, SDL_PointInRect(&mousePoint, &backButtonRectangle));
}

// Vuruş anında kısa süreli efekt başlatma
void UIManager::showHitEffect(int x, int y) {
    activateEffect(floatingTexts, 6, x, y);
    activateEffect(hitSparks, 6, x, y);
}

// Süresi dolan efektleri kapatma
void UIManager::updateHitEffects() {
    Uint32 now = SDL_GetTicks();

    for (int i = 0; i < 6; i++) {
        if (floatingTexts[i].active && now - floatingTexts[i].startTime > 380) {
            floatingTexts[i].active = false;
        }
        if (hitSparks[i].active && now - hitSparks[i].startTime > 140) {
            hitSparks[i].active = false;
        }
    }
}

// Yeni oyun başlarken eski efektleri temizleme
void UIManager::clearHitEffects() {
    for (int i = 0; i < 6; i++) {
        floatingTexts[i].active = false;
        hitSparks[i].active = false;
    }
}

// Vuruş efekti ve +10 yazısını çizme
void UIManager::renderEffects(SDL_Renderer *renderer) {
    Uint32 now = SDL_GetTicks();

    for (int i = 0; i < 6; i++) {
        if (!hitSparks[i].active) {
            continue;
        }

        int passedTime = now - hitSparks[i].startTime;
        int radius = 6 + (passedTime * 12 / 140);
        int alphaValue = 220 - (passedTime * 220 / 140);
        if (alphaValue < 0) {
            alphaValue = 0;
        }

        Uint8 alpha = alphaValue;
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
        SDL_SetRenderDrawColor(renderer, 255, 235, 160, alpha);

        SDL_RenderDrawLine(renderer, hitSparks[i].x - radius, hitSparks[i].y, hitSparks[i].x + radius, hitSparks[i].y);
        SDL_RenderDrawLine(renderer, hitSparks[i].x, hitSparks[i].y - radius, hitSparks[i].x, hitSparks[i].y + radius);
        SDL_RenderDrawLine(renderer, hitSparks[i].x - radius + 2, hitSparks[i].y - radius + 2, hitSparks[i].x + radius - 2, hitSparks[i].y + radius - 2);
        SDL_RenderDrawLine(renderer, hitSparks[i].x - radius + 2, hitSparks[i].y + radius - 2, hitSparks[i].x + radius - 2, hitSparks[i].y - radius + 2);
    }

    if (floatingScoreTexture == nullptr) {
        return;
    }

    for (int i = 0; i < 6; i++) {
        if (!floatingTexts[i].active) {
            continue;
        }

        int passedTime = now - floatingTexts[i].startTime;
        int moveUp = passedTime * 32 / 380;
        int alphaValue = 255 - (passedTime * 255 / 380);
        if (alphaValue < 0) {
            alphaValue = 0;
        }

        SDL_Rect drawRectangle = {
            floatingTexts[i].x - (floatingScoreRectangle.w / 2),
            floatingTexts[i].y - moveUp,
            floatingScoreRectangle.w,
            floatingScoreRectangle.h
        };

        Uint8 alpha = alphaValue;
        drawShadowText(renderer, floatingScoreTexture, &drawRectangle, alpha);
    }
}

// Texture ve font hafızalarını temizleme
void UIManager::clean() {
    mainTitleTexture = safeDestroyTexture(mainTitleTexture);
    playButtonTexture = safeDestroyTexture(playButtonTexture);
    leaderboardButtonTexture = safeDestroyTexture(leaderboardButtonTexture);
    exitButtonTexture = safeDestroyTexture(exitButtonTexture);
    backButtonTexture = safeDestroyTexture(backButtonTexture);
    backIconTexture = safeDestroyTexture(backIconTexture);
    scoreTexture = safeDestroyTexture(scoreTexture);
    timerTexture = safeDestroyTexture(timerTexture);
    gameOverTextTexture = safeDestroyTexture(gameOverTextTexture);
    resultsTitleTexture = safeDestroyTexture(resultsTitleTexture);
    floatingScoreTexture = safeDestroyTexture(floatingScoreTexture);
    leaderBoardTitleTexture = safeDestroyTexture(leaderBoardTitleTexture);

    for (int i = 0; i < 5; i++) {
        highScoreTextures[i] = safeDestroyTexture(highScoreTextures[i]);
    }

    if (font != nullptr) {
        TTF_CloseFont(font);
        font = nullptr;
    }

    TTF_Quit();
}
