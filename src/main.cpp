#include <iostream>
#include <SDL2/SDL.h>

using namespace std;

int main(int argc, char *argv[])
{
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        return -1;
    }

    SDL_Window *window = SDL_CreateWindow(
        "Whack-a-Mole",
        SDL_WINDOWPOS_CENTERED, // X ekseninde ortalama
        SDL_WINDOWPOS_CENTERED, // Y ekseninde ortalama
        800,                    // Genişlik
        600,                    // Yükseklik
        SDL_WINDOW_SHOWN        // Ekranda gösterme
    );

    if (window == nullptr)
    {
        SDL_Quit();
        return -1;
    }

    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == nullptr)
    {
        SDL_DestroyWindow(window);
        SDL_Quit();
        return -1;
    }

    bool isRunning = true;
    SDL_Event event;

    while (isRunning)
    {
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
            {
                isRunning = false;
            }
        }

        SDL_SetRenderDrawColor(renderer, 34, 139, 34, 255);
        
        SDL_RenderClear(renderer);
        
        SDL_RenderPresent(renderer);

        SDL_Delay(16); // 60 FPS sabitleme
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}