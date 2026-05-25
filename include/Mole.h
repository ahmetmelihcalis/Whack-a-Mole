#ifndef MOLE_H
#define MOLE_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

class Mole {
public:
    void init(SDL_Renderer *renderer, int startX, int startY);
    void update();
    void render(SDL_Renderer *renderer);

    void clean();

    void popUp();
    void hide();
    bool isShowing();
    bool handleInput(int mouseX, int mouseY);

private:
    // Köstebeğin ekrandaki animasyon durumları
    enum AnimationState {
        HIDDEN,
        RISING,
        VISIBLE,
        FALLING
    };

    SDL_Texture *texture = nullptr;
    SDL_Texture *whackedTexture = nullptr;
    SDL_Texture *holeTexture = nullptr;
    SDL_Texture *holeFrontTexture = nullptr;

    // Çizim ve tıklama kontrolü için kullanılan alanlar
    SDL_Rect moleRectangle;
    SDL_Rect hitRectangle;
    SDL_Rect holeRectangle;

    // Animasyon zamanlamasını yöneten değişkenler
    AnimationState state;
    bool isHit;
    Uint32 stateStartTime;

    // Animasyon süreleri
    const Uint32 riseDuration = 180;
    const Uint32 stayUpDuration = 850;
    const Uint32 fallDuration = 180;
};

#endif
