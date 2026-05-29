#include "Mole.h"
#include <iostream>

using namespace std;

// Köstebeğin texture'larını ve sahnedeki yerleşimini hazırlama
void Mole::init(SDL_Renderer *renderer, int startX, int startY) {
    int holeWidth = 184;
    int holeHeight = 154;
    int holeOffsetX = -9;
    int holeOffsetY = 78;
    int moleHeight = 160;
    int moleOffsetY = -34;
    int textureWidth = 0;
    int textureHeight = 0;

    SDL_Surface *surface = IMG_Load("../assets/mole.png");
    if (surface != nullptr) {
        textureWidth = surface->w;
        textureHeight = surface->h;
        texture = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_FreeSurface(surface);
    } else {
        cout << "Failed to load mole.png. Error: " << IMG_GetError() << endl;
    }

    surface = IMG_Load("../assets/whacked-mole.png");
    if (surface != nullptr) {
        whackedTexture = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_FreeSurface(surface);
    } else {
        cout << "Failed to load whacked-mole.png. Error: " << IMG_GetError() << endl;
    }

    surface = IMG_Load("../assets/hole.png");
    if (surface != nullptr) {
        holeTexture = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_FreeSurface(surface);
    } else {
        cout << "Failed to load hole.png. Error: " << IMG_GetError() << endl;
    }

    surface = IMG_Load("../assets/hole_front.png");
    if (surface != nullptr) {
        holeFrontTexture = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_FreeSurface(surface);
    }

    holeRectangle = {startX + holeOffsetX, startY + holeOffsetY, holeWidth, holeHeight};

    moleRectangle.h = moleHeight;
    if (textureHeight > 0) {
        moleRectangle.w = (textureWidth * moleRectangle.h) / textureHeight;
    } else {
        moleRectangle.w = 106;
    }
    moleRectangle.x = holeRectangle.x + (holeRectangle.w - moleRectangle.w) / 2;
    moleRectangle.y = holeRectangle.y + moleOffsetY;

    hitRectangle = {
        moleRectangle.x + 4,
        moleRectangle.y + 6,
        moleRectangle.w - 8,
        100
    };

    state = HIDDEN;
    isHit = false;
    stateStartTime = 0;
}

void Mole::popUp() {
    if (state == HIDDEN) {
        state = RISING;
        isHit = false;
        stateStartTime = SDL_GetTicks();
    }
}

// Tıklandığında veya süresi dolduğunda düşüş animasyonuna geçirme
void Mole::hide() {
    if (state != HIDDEN) {
        state = FALLING;
        stateStartTime = SDL_GetTicks();
    }
}

bool Mole::isVisible() {
    return state != HIDDEN;
}

bool Mole::checkClick(int mouseX, int mouseY) {
    if (state == RISING || state == VISIBLE) {
        SDL_Point mousePoint = {mouseX, mouseY};
        if (SDL_PointInRect(&mousePoint, &hitRectangle)) {
            isHit = true;
            hide();
            return true;
        }
    }
    return false;
}

void Mole::update() {
    Uint32 now = SDL_GetTicks();
    Uint32 elapsed = now - stateStartTime;

    // Durumlar arası geçişi zamana göre yapma
    if (state == RISING && elapsed >= riseDuration) {
        state = VISIBLE;
        stateStartTime = now;
    } else if (state == VISIBLE && elapsed >= stayUpDuration) {
        isHit = false;
        state = FALLING;
        stateStartTime = now;
    } else if (state == FALLING && elapsed >= fallDuration) {
        state = HIDDEN;
    }
}

// Delik - köstebek - toprak katmanı sırası ile çizim
void Mole::render(SDL_Renderer *renderer) {
    float progress = 1;

    // Animasyon başladığından beri geçen süre
    Uint32 passedTime = SDL_GetTicks() - stateStartTime;
    if (state == RISING) {
        progress = (float)passedTime / riseDuration;
        if (progress > 1) {
            progress = 1;
        }
    } else if (state == FALLING) {
        progress = 1 - (float)passedTime / fallDuration;
        if (progress < 0) {
            progress = 0;
        }
    }

    if (holeTexture != nullptr) {
        SDL_RenderCopy(renderer, holeTexture, nullptr, &holeRectangle);
    } else {
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
        SDL_SetRenderDrawColor(renderer, 38, 24, 18, 220);
        SDL_RenderFillRect(renderer, &holeRectangle);
    }

    if (state == HIDDEN) return;

    SDL_Texture *activeTexture = isHit && whackedTexture != nullptr ? whackedTexture : texture;
    if (activeTexture == nullptr) return;

    // Köstebeğin başlangıç konumu
    int hiddenY = holeRectangle.y + 80;
    int visibleY = holeRectangle.y - 10;
    int moveDistance = hiddenY - visibleY;
    int currentY = hiddenY - (int)(moveDistance * progress);
    SDL_Rect destination = {
        moleRectangle.x,
        currentY,
        moleRectangle.w,
        moleRectangle.h
    };

    // Vurulunca sarsılma efekti verme
    if (isHit && passedTime < 90) {
        int shake = 0;
        if ((passedTime / 20) % 2 == 0) {
            shake = -3;
        } else {
            shake = 3;
        }

        destination.x += shake;
        destination.y += 2;
        destination.w -= 4;
        destination.h -= 4;
    }

    // Alt gövdenin taşmaması için çizim alanını sınırlama
    SDL_Rect clipRectangle = {
        holeRectangle.x - 8,
        holeRectangle.y - 96,
        holeRectangle.w + 16,
        182
    };

    SDL_RenderSetClipRect(renderer, &clipRectangle);
    SDL_RenderCopy(renderer, activeTexture, nullptr, &destination);
    SDL_RenderSetClipRect(renderer, nullptr);

    if (holeFrontTexture != nullptr) {
        SDL_Rect frontRectangle = holeRectangle;

        if (state == RISING) {
            if (progress < 0.35f) {
                frontRectangle.y += 4;
                frontRectangle.h -= 4;
            } else if (progress < 0.7f) {
                frontRectangle.y += 2;
                frontRectangle.h -= 2;
            }
        }

        SDL_RenderCopy(renderer, holeFrontTexture, nullptr, &frontRectangle);
    }
}

// Texture hafızalarını temizleme
void Mole::clean() {
    if (texture != nullptr) {
        SDL_DestroyTexture(texture);
        texture = nullptr;
    }
    if (whackedTexture != nullptr) {
        SDL_DestroyTexture(whackedTexture);
        whackedTexture = nullptr;
    }
    if (holeTexture != nullptr) {
        SDL_DestroyTexture(holeTexture);
        holeTexture = nullptr;
    }
    if (holeFrontTexture != nullptr) {
        SDL_DestroyTexture(holeFrontTexture);
        holeFrontTexture = nullptr;
    }
}
