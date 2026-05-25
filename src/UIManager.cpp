#include "UIManager.h"
#include <iostream>
#include <string>

using namespace std;

// Yazıların her arka planda okunmasını sağlama
static void renderShadowed(SDL_Renderer *renderer, SDL_Texture *texture, SDL_Rect *rect, Uint8 alpha = 255) {
    if (texture == nullptr) return;

    SDL_Rect shadowRect = {rect->x + 3, rect->y + 3, rect->w, rect->h};
    SDL_SetTextureColorMod(texture, 0, 0, 0);

    Uint8 shadowAlpha = alpha;
    if (shadowAlpha > 150) shadowAlpha = 150;

    SDL_SetTextureAlphaMod(texture, shadowAlpha);
    SDL_RenderCopy(renderer, texture, NULL, &shadowRect);

    SDL_SetTextureColorMod(texture, 255, 255, 255);
    SDL_SetTextureAlphaMod(texture, alpha);
    SDL_RenderCopy(renderer, texture, NULL, rect);
}

// Panelleri tek tip bir görünümle çizme
static void drawPanel(SDL_Renderer *renderer, SDL_Rect rect, SDL_Color fillColor, SDL_Color borderColor) {
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

    SDL_Rect shadowRect = {rect.x + 4, rect.y + 5, rect.w, rect.h};
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 46);
    SDL_RenderFillRect(renderer, &shadowRect);

    SDL_SetRenderDrawColor(renderer, fillColor.r, fillColor.g, fillColor.b, fillColor.a);
    SDL_RenderFillRect(renderer, &rect);

    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 18);
    SDL_Rect topHighlight = {rect.x + 2, rect.y + 2, rect.w - 4, 4};
    SDL_RenderFillRect(renderer, &topHighlight);

    if (borderColor.a > 0) {
        SDL_SetRenderDrawColor(renderer, borderColor.r, borderColor.g, borderColor.b, borderColor.a);
        SDL_RenderDrawLine(renderer, rect.x + 2, rect.y + rect.h - 1, rect.x + rect.w - 3, rect.y + rect.h - 1);
    }
}

// Butonu panel ve yazısıyla birlikte çizme
static void renderButton(SDL_Renderer *renderer, SDL_Rect rect, SDL_Color fillColor, SDL_Color borderColor,
                         SDL_Texture *texture, bool isHovered) {
    if (isHovered) {
        rect.x -= 8;
        rect.y -= 4;
        rect.w += 16;
        rect.h += 8;
    }

    drawPanel(renderer, rect, fillColor, borderColor);

    int textWidth = 0;
    int textHeight = 0;
    SDL_QueryTexture(texture, nullptr, nullptr, &textWidth, &textHeight);

    SDL_Rect textRect = {
        rect.x + (rect.w - textWidth) / 2,
        rect.y + (rect.h - textHeight) / 2,
        textWidth,
        textHeight
    };
    renderShadowed(renderer, texture, &textRect);
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
        mainTitleRect.w = 460;
        mainTitleRect.h = (surface->h * mainTitleRect.w) / surface->w;
        mainTitleRect.x = (1280 / 2) - (mainTitleRect.w / 2);
        mainTitleRect.y = 70;
        SDL_FreeSurface(surface);
    }

    surface = TTF_RenderText_Blended(font, "PLAY", green);
    playButtonTexture = SDL_CreateTextureFromSurface(renderer, surface);
    playButtonRect = {(1280 / 2) - 170, 300, 340, 66};
    SDL_FreeSurface(surface);

    surface = TTF_RenderText_Blended(font, "HIGH SCORES", white);
    leaderboardButtonTexture = SDL_CreateTextureFromSurface(renderer, surface);
    leaderboardButtonRect = {(1280 / 2) - 170, 380, 340, 66};
    SDL_FreeSurface(surface);

    surface = TTF_RenderText_Blended(font, "EXIT", red);
    exitButtonTexture = SDL_CreateTextureFromSurface(renderer, surface);
    exitButtonRect = {(1280 / 2) - 170, 460, 340, 66};
    SDL_FreeSurface(surface);

    surface = TTF_RenderText_Blended(font, "BACK TO MENU", white);
    backButtonTexture = SDL_CreateTextureFromSurface(renderer, surface);
    backButtonRect = {(1280 / 2) - 170, 524, 340, 66};
    SDL_FreeSurface(surface);

    surface = TTF_RenderText_Blended(font, "ROUND RESULTS", yellow);
    resultsTitleTexture = SDL_CreateTextureFromSurface(renderer, surface);
    resultsTitleRectangle = {(1280 / 2) - (surface->w / 2), 138, surface->w, surface->h};
    SDL_FreeSurface(surface);

    surface = TTF_RenderText_Blended(font, "TIME IS UP", yellow);
    gameOverTextTexture = SDL_CreateTextureFromSurface(renderer, surface);
    gameOverRectangle = {(1280 / 2) - (surface->w / 2), 194, surface->w, surface->h};
    SDL_FreeSurface(surface);

    surface = TTF_RenderText_Blended(font, "+10", yellow);
    floatingScoreTexture = SDL_CreateTextureFromSurface(renderer, surface);
    floatingScoreRect = {0, 0, surface->w, surface->h};
    SDL_FreeSurface(surface);

    scoreRectangle.x = 1000;
    scoreRectangle.y = 30;
    timerRectangle.x = 50;
    timerRectangle.y = 30;

    return true;
}

// Skor yazısını güncelleme
void UIManager::updateScore(SDL_Renderer *renderer, int score) {
    if (scoreTexture != nullptr) {
        SDL_DestroyTexture(scoreTexture);
    }

    string scoreText = "Score: " + to_string(score);
    SDL_Surface *surface = TTF_RenderText_Blended(font, scoreText.c_str(), {255, 255, 255, 255});
    scoreRectangle.w = surface->w;
    scoreRectangle.h = surface->h;
    scoreTexture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);

    scorePulseAmount = 6;
}

// Süre yazısını güncelleme
void UIManager::updateTimer(SDL_Renderer *renderer, int timeRemaining) {
    if (timerTexture != nullptr) {
        SDL_DestroyTexture(timerTexture);
    }

    currentTimeRemaining = timeRemaining;

    SDL_Color timerColor = {255, 255, 255, 255};
    if (timeRemaining <= 10) {
        timerColor = {255, 110, 110, 255};
    }

    string timerText = "Time: " + to_string(timeRemaining);
    SDL_Surface *surface = TTF_RenderText_Blended(font, timerText.c_str(), timerColor);
    timerRectangle.w = surface->w;
    timerRectangle.h = surface->h;
    timerTexture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
}

// Skor tablosunu hafızada tek seferlik hazırlama
void UIManager::prepareLeaderboard(SDL_Renderer *renderer, int finalScore, const std::vector<int>& topScores) {
    if (leaderBoardTitleTexture != nullptr) {
        SDL_DestroyTexture(leaderBoardTitleTexture);
    }

    for (int i = 0; i < 5; i++) {
        if (highScoreTextures[i] != nullptr) {
            SDL_DestroyTexture(highScoreTextures[i]);
        }
        highScoreTextures[i] = nullptr;
    }

    SDL_Color yellow = {255, 255, 0, 255};
    SDL_Color white = {255, 255, 255, 255};

    SDL_Surface *titleSurf = TTF_RenderText_Blended(font, "TOP SCORES", yellow);
    leaderBoardTitleTexture = SDL_CreateTextureFromSurface(renderer, titleSurf);
    leaderBoardTitleRectangle = {(1280 / 2) - (titleSurf->w / 2), 100, titleSurf->w, titleSurf->h};
    SDL_FreeSurface(titleSurf);

    bool didMarkPlayerScore = false;

    for (size_t i = 0; i < topScores.size() && i < 5; i++) {
        string text = to_string(i + 1) + ". Rank: " + to_string(topScores[i]);

        if (topScores[i] == finalScore && finalScore > 0 && !didMarkPlayerScore) {
            text += " [YOU]";
            didMarkPlayerScore = true;
        }

        SDL_Surface *scoreSurf = TTF_RenderText_Blended(font, text.c_str(), white);
        highScoreTextures[i] = SDL_CreateTextureFromSurface(renderer, scoreSurf);
        highScoreRectangles[i] = {(1280 / 2) - (scoreSurf->w / 2), 170 + ((int)i * 50), scoreSurf->w, scoreSurf->h};
        SDL_FreeSurface(scoreSurf);
    }
}

// Skor satırlarını aynı düzenle ekrana çizme
void UIManager::renderLeaderboardRows(SDL_Renderer *renderer, int startX, int startY, int panelWidth) {
    for (int i = 0; i < 5; i++) {
        if (highScoreTextures[i] == nullptr) continue;

        SDL_Rect rowRect = {startX + 26, startY + (i * 44), panelWidth - 52, 38};
        drawPanel(renderer, rowRect, {18, 22, 18, 128}, {255, 235, 160, 70});

        SDL_Rect textRect = {
            rowRect.x + 18,
            rowRect.y + (rowRect.h - highScoreRectangles[i].h) / 2,
            highScoreRectangles[i].w,
            highScoreRectangles[i].h
        };
        renderShadowed(renderer, highScoreTextures[i], &textRect);
    }
}

// Ana menüyü çizme
void UIManager::renderMenu(SDL_Renderer *renderer) {
    int mouseX = 0;
    int mouseY = 0;
    SDL_GetMouseState(&mouseX, &mouseY);
    SDL_Point mousePoint = {mouseX, mouseY};

    if (mainTitleTexture != nullptr) {
        SDL_RenderCopy(renderer, mainTitleTexture, NULL, &mainTitleRect);
    }

    renderButton(renderer, playButtonRect, {22, 28, 24, 132}, {0, 0, 0, 0},
                 playButtonTexture, SDL_PointInRect(&mousePoint, &playButtonRect));

    renderButton(renderer, leaderboardButtonRect, {22, 28, 24, 132}, {0, 0, 0, 0},
                 leaderboardButtonTexture, SDL_PointInRect(&mousePoint, &leaderboardButtonRect));

    renderButton(renderer, exitButtonRect, {22, 28, 24, 132}, {0, 0, 0, 0},
                 exitButtonTexture, SDL_PointInRect(&mousePoint, &exitButtonRect));
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

    if (scoreTexture != nullptr) renderShadowed(renderer, scoreTexture, &scoreRectangle);
    if (timerTexture != nullptr) renderShadowed(renderer, timerTexture, &timerRectangle);
}

// Ayrı yüksek skor ekranını çizme
void UIManager::renderHighScores(SDL_Renderer *renderer) {
    SDL_Rect boardPanel = {(1280 / 2) - 250, 120, 500, 360};
    drawPanel(renderer, boardPanel, {14, 18, 18, 170}, {255, 235, 160, 85});

    if (leaderBoardTitleTexture != nullptr) {
        SDL_Rect titleRect = {
            (1280 / 2) - (leaderBoardTitleRectangle.w / 2),
            boardPanel.y + 24,
            leaderBoardTitleRectangle.w,
            leaderBoardTitleRectangle.h
        };
        renderShadowed(renderer, leaderBoardTitleTexture, &titleRect);
    }

    renderLeaderboardRows(renderer, boardPanel.x, boardPanel.y + 78, boardPanel.w);

    int mouseX = 0;
    int mouseY = 0;
    SDL_GetMouseState(&mouseX, &mouseY);
    SDL_Point mousePoint = {mouseX, mouseY};

    renderButton(renderer, backButtonRect, {18, 22, 18, 178}, {255, 235, 160, 190},
                 backButtonTexture, SDL_PointInRect(&mousePoint, &backButtonRect));
}

// Süre bitince açılan sonuç ekranını çizme
void UIManager::renderGameOver(SDL_Renderer *renderer) {
    SDL_Rect resultCard = {(1280 / 2) - 300, 56, 600, 600};
    drawPanel(renderer, resultCard, {16, 18, 20, 176}, {255, 220, 140, 80});

    if (resultsTitleTexture != nullptr) {
        SDL_Rect titleRect = {
            (1280 / 2) - (resultsTitleRectangle.w / 2),
            resultCard.y + 26,
            resultsTitleRectangle.w,
            resultsTitleRectangle.h
        };
        renderShadowed(renderer, resultsTitleTexture, &titleRect);
    }

    gameOverRectangle = {(1280 / 2) - (gameOverRectangle.w / 2), resultCard.y + 98, gameOverRectangle.w, gameOverRectangle.h};
    renderShadowed(renderer, gameOverTextTexture, &gameOverRectangle);

    if (scoreTexture != nullptr) {
        SDL_Rect finalScorePanel = {(1280 / 2) - (scoreRectangle.w / 2) - 20, resultCard.y + 162, scoreRectangle.w + 40, scoreRectangle.h + 22};
        drawPanel(renderer, finalScorePanel, {32, 18, 18, 142}, {255, 205, 120, 70});

        SDL_Rect finalScoreRectangle = {(1280 / 2) - (scoreRectangle.w / 2), resultCard.y + 174, scoreRectangle.w, scoreRectangle.h};
        renderShadowed(renderer, scoreTexture, &finalScoreRectangle);
    }

    if (leaderBoardTitleTexture != nullptr) {
        SDL_Rect titleRect = {
            (1280 / 2) - (leaderBoardTitleRectangle.w / 2),
            resultCard.y + 248,
            leaderBoardTitleRectangle.w,
            leaderBoardTitleRectangle.h
        };
        renderShadowed(renderer, leaderBoardTitleTexture, &titleRect);
    }

    for (int i = 0; i < 5; i++) {
        if (highScoreTextures[i] == nullptr) continue;

        SDL_Rect textRect = {
            resultCard.x + 72,
            resultCard.y + 306 + (i * 46),
            highScoreRectangles[i].w,
            highScoreRectangles[i].h
        };
        renderShadowed(renderer, highScoreTextures[i], &textRect);
    }

    int mouseX = 0;
    int mouseY = 0;
    SDL_GetMouseState(&mouseX, &mouseY);
    SDL_Point mousePoint = {mouseX, mouseY};

    backButtonRect = {(1280 / 2) - 155, resultCard.y + 526, 300, 54};
    renderButton(renderer, backButtonRect, {18, 22, 18, 160}, {255, 235, 160, 90},
                 backButtonTexture, SDL_PointInRect(&mousePoint, &backButtonRect));
}

// Vuruş anında kısa süreli efekt başlatma
void UIManager::addHitEffect(int x, int y) {
    for (int i = 0; i < 6; i++) {
        if (!floatingTexts[i].active) {
            floatingTexts[i].active = true;
            floatingTexts[i].x = x;
            floatingTexts[i].y = y;
            floatingTexts[i].startTime = SDL_GetTicks();
            break;
        }
    }

    for (int i = 0; i < 6; i++) {
        if (!hitSparks[i].active) {
            hitSparks[i].active = true;
            hitSparks[i].x = x;
            hitSparks[i].y = y;
            hitSparks[i].startTime = SDL_GetTicks();
            break;
        }
    }
}

// Süresi dolan efektleri kapatma
void UIManager::updateEffects() {
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
void UIManager::clearEffects() {
    for (int i = 0; i < 6; i++) {
        floatingTexts[i].active = false;
        hitSparks[i].active = false;
    }
}

// Vuruş efekti ve +10 yazısını çizme
void UIManager::renderEffects(SDL_Renderer *renderer) {
    Uint32 now = SDL_GetTicks();

    for (int i = 0; i < 6; i++) {
        if (!hitSparks[i].active) continue;

        int passedTime = now - hitSparks[i].startTime;
        int radius = 6 + (passedTime * 12 / 140);
        int alphaValue = 220 - (passedTime * 220 / 140);
        if (alphaValue < 0) alphaValue = 0;

        Uint8 alpha = (Uint8)alphaValue;
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
        SDL_SetRenderDrawColor(renderer, 255, 235, 160, alpha);

        SDL_RenderDrawLine(renderer, hitSparks[i].x - radius, hitSparks[i].y, hitSparks[i].x + radius, hitSparks[i].y);
        SDL_RenderDrawLine(renderer, hitSparks[i].x, hitSparks[i].y - radius, hitSparks[i].x, hitSparks[i].y + radius);
        SDL_RenderDrawLine(renderer, hitSparks[i].x - radius + 2, hitSparks[i].y - radius + 2, hitSparks[i].x + radius - 2, hitSparks[i].y + radius - 2);
        SDL_RenderDrawLine(renderer, hitSparks[i].x - radius + 2, hitSparks[i].y + radius - 2, hitSparks[i].x + radius - 2, hitSparks[i].y - radius + 2);
    }

    if (floatingScoreTexture == nullptr) return;

    for (int i = 0; i < 6; i++) {
        if (!floatingTexts[i].active) continue;

        int passedTime = now - floatingTexts[i].startTime;
        int moveUp = passedTime * 32 / 380;
        int alphaValue = 255 - (passedTime * 255 / 380);
        if (alphaValue < 0) alphaValue = 0;

        SDL_Rect drawRect = {
            floatingTexts[i].x - (floatingScoreRect.w / 2),
            floatingTexts[i].y - moveUp,
            floatingScoreRect.w,
            floatingScoreRect.h
        };

        Uint8 alpha = (Uint8)alphaValue;
        renderShadowed(renderer, floatingScoreTexture, &drawRect, alpha);
    }
}

// Texture ve font hafızalarını temizleme
void UIManager::clean() {
    SDL_DestroyTexture(mainTitleTexture);
    SDL_DestroyTexture(playButtonTexture);
    SDL_DestroyTexture(leaderboardButtonTexture);
    SDL_DestroyTexture(exitButtonTexture);
    SDL_DestroyTexture(backButtonTexture);
    SDL_DestroyTexture(scoreTexture);
    SDL_DestroyTexture(timerTexture);
    SDL_DestroyTexture(gameOverTextTexture);
    SDL_DestroyTexture(resultsTitleTexture);
    SDL_DestroyTexture(floatingScoreTexture);

    if (leaderBoardTitleTexture != nullptr) {
        SDL_DestroyTexture(leaderBoardTitleTexture);
    }

    for (int i = 0; i < 5; i++) {
        if (highScoreTextures[i] != nullptr) {
            SDL_DestroyTexture(highScoreTextures[i]);
        }
    }

    if (font != nullptr) {
        TTF_CloseFont(font);
    }

    TTF_Quit();
}
