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
    SDL_Texture *texture = nullptr; 
    SDL_Rect moleRectangle; 
    
    bool isUp = false; 
    // Zamanlayıcı Değişkenleri
    Uint32 popUpTime = 0;         // Çıktığı anki saati tutacak kısım
    Uint32 stayUpDuration = 1000; // Kaç milisaniye dışarıda kalacağını tutan kısım
};

#endif