#include "Game.h"
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <fstream>   // Dosya okuma/yazma için eklendi
#include <algorithm> // Skorları sıralamak için eklendi

using namespace std;

void Game::init(const char *title, int xposition, int yposition, int width, int height, bool fullscreen) {
    int flags = 0;
    if (fullscreen) flags = SDL_WINDOW_FULLSCREEN; // Tam ekran kontrolü

    // SDL'i başlatma ve pencereyi oluşturma
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        isRunning = false;
        return;
    }

    // Resimleri küçültüp büyütürken kaliteyi artırma
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1");

    window = SDL_CreateWindow(title, xposition, yposition, width, height, flags);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    // UI sistemini başlatma
    ui.init(renderer, "../assets/PixelOperator8-Bold.ttf", 26);

    SDL_Surface *surface = IMG_Load("../assets/menu_bg.png");
    menuBgTexture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);

    surface = IMG_Load("../assets/game_bg.png");
    gameBgTexture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);

    // Ses sistemini başlatma
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        cout << "Failed to initialize audio. Error: " << Mix_GetError() << endl;
    }

    // Vuruş sesini hafızaya yükleme
    hitSound = Mix_LoadWAV("../assets/hit.wav");
    if (hitSound == nullptr) {
        cout << "Failed to load hit.wav. Error: " << Mix_GetError() << endl;
    }

    // Oyun açılırken kayıtlı en yüksek skorları dosyadan çekme
    loadHighScores();

    // İlk skor ve süre metinlerini hazırlama
    ui.updateScore(renderer, score);
    ui.updateTimer(renderer, timeRemaining);

    // 3x3 ızgara
    int startX = 340;
    int startY = 45;
    int spacingX = 190;
    int spacingY = 165;

    for (int i = 0; i < 9; i++) {
        int row = i / 3;
        int col = i % 3;
        moles[i].init(renderer, startX + (col * spacingX), startY + (row * spacingY));
    }

    srand(static_cast<unsigned int>(time(nullptr))); // Rastgele sayı üretecini başlatma
    isRunning = true; // Oyun döngüsünü aktif etme
}

// txt dosyasından skorları okuma
void Game::loadHighScores() {
    highScores.clear();

    ifstream file("highscores.txt");
    int tempScore;

    while (file >> tempScore) {
        highScores.push_back(tempScore);
    }

    file.close();

    while (highScores.size() < 5) {
        highScores.push_back(0);
    }

    // Skorları büyükten küçüğe sıralama
    sort(highScores.begin(), highScores.end(), greater<int>());
}

// Yeni yapılan skoru listeye ekleme, sıralama ve dosyaya kaydetme
void Game::checkAndSaveHighScore(int currentScore) {
    highScores.push_back(currentScore);

    // Büyükten küçüğe sıralama
    sort(highScores.begin(), highScores.end(), greater<int>());

    while (highScores.size() > 5) {
        highScores.pop_back();
    }

    // Güncel en yüksek 5 skoru dosyaya kalıcı olarak kaydetme
    ofstream file("highscores.txt");
    for (int s : highScores) {
        file << s << "\n";
    }
    file.close();
}

// Kullanıcı girdilerini yakalama
void Game::handleEvents() {
    SDL_Event event;

    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            isRunning = false;
        }

        if (event.type != SDL_MOUSEBUTTONDOWN || event.button.button != SDL_BUTTON_LEFT) {
            continue;
        }

        int mx = event.button.x;
        int my = event.button.y;
        SDL_Point p = {mx, my};

        // DURUM 1: Ana Menü
        if (currentState == MENU) {
            SDL_Rect playBtn = ui.getPlayButtonRect();
            SDL_Rect lbBtn = ui.getLeaderboardButtonRect();
            SDL_Rect exitBtn = ui.getExitButtonRect();

            if (SDL_PointInRect(&p, &playBtn)) {
                score = 0;
                timeRemaining = timeLimit;
                gameStartTime = SDL_GetTicks();
                ui.clearEffects();
                ui.updateScore(renderer, score);
                ui.updateTimer(renderer, timeRemaining);
                currentState = PLAYING;
            } else if (SDL_PointInRect(&p, &lbBtn)) {
                ui.prepareLeaderboard(renderer, 0, highScores);
                currentState = HIGHSCORES;
            } else if (SDL_PointInRect(&p, &exitBtn)) {
                isRunning = false;
            }
        }

        // DURUM 2: Skor Tablosu
        else if (currentState == HIGHSCORES) {
            SDL_Rect backBtn = ui.getBackButtonRect();
            if (SDL_PointInRect(&p, &backBtn)) {
                currentState = MENU;
            }
        }

        // DURUM 3: Oyun Oynanıyorsa
        else if (currentState == PLAYING) {
            for (int i = 0; i < 9; i++) {
                if (moles[i].handleInput(mx, my)) {
                    // Köstebeğe başarıyla vurulduğunda sesi çalma
                    if (hitSound != nullptr) {
                        Mix_PlayChannel(-1, hitSound, 0);
                    }

                    score += 10;
                    ui.addHitEffect(mx, my);
                    ui.updateScore(renderer, score); // Skor yazısını ui güncelliyor
                }
            }
        }

        // DURUM 4: Oyun Bittiyse
        else if (currentState == GAMEOVER) {
            SDL_Rect goBtn = ui.getGameOverRect();
            SDL_Rect backBtn = ui.getBackButtonRect();

            if (SDL_PointInRect(&p, &goBtn) || SDL_PointInRect(&p, &backBtn)) {
                currentState = MENU;
            }
        }
    }
}

// Oyun mantığını, zamanı ve animasyonları güncelleme
void Game::update() {
    if (currentState != PLAYING) return;

    // Zaman kontrolü
    int elapsedSeconds = (SDL_GetTicks() - gameStartTime) / 1000;
    int newTimeRemaining = timeLimit - elapsedSeconds;

    if (newTimeRemaining != timeRemaining) {
        timeRemaining = newTimeRemaining;
        ui.updateTimer(renderer, timeRemaining); // Süre yazısını ui güncelliyor
    }

    if (timeRemaining <= 0) {
        currentState = GAMEOVER;

        // Skorları kontrol etme, kaydetme ve ekrana çizilmesi için UI'ı hazırlama
        checkAndSaveHighScore(score);
        ui.prepareLeaderboard(renderer, score, highScores);

        for (int i = 0; i < 9; i++) {
            moles[i].hide();
        }
    }

    for (int i = 0; i < 9; i++) {
        moles[i].update();
    }

    ui.updateEffects();

    if (SDL_GetTicks() - lastSpawnTime > spawnInterval) {
        int randomIndex = rand() % 9;
        if (!moles[randomIndex].isShowing()) {
            moles[randomIndex].popUp();
        }
        lastSpawnTime = SDL_GetTicks();
    }
}

// Görselleri ekrana çizme
void Game::render() {
    if (currentState == MENU || currentState == HIGHSCORES) {
        // DURUM 1: Menü veya Skor Ekranı
        SDL_SetRenderDrawColor(renderer, 34, 139, 34, 255);
        SDL_RenderClear(renderer);

        if (menuBgTexture != nullptr) {
            SDL_RenderCopy(renderer, menuBgTexture, NULL, NULL);
        }

        if (currentState == MENU) {
            ui.renderMenu(renderer);
        } else {
            ui.renderHighScores(renderer);
        }
    } else if (currentState == PLAYING) {
        // DURUM 2: Oyun Esnası
        SDL_SetRenderDrawColor(renderer, 34, 139, 34, 255);
        SDL_RenderClear(renderer);

        if (gameBgTexture != nullptr) {
            SDL_RenderCopy(renderer, gameBgTexture, NULL, NULL);
        }

        for (int i = 0; i < 9; i++) {
            moles[i].render(renderer);
        }

        ui.renderPlaying(renderer); // Skor ve süre yazılarını ui çiziyor
        ui.renderEffects(renderer);
    } else if (currentState == GAMEOVER) {
        // DURUM 3: Oyun Bitişi
        SDL_SetRenderDrawColor(renderer, 50, 0, 0, 255);
        SDL_RenderClear(renderer);

        if (menuBgTexture != nullptr) {
            SDL_RenderCopy(renderer, menuBgTexture, NULL, NULL);
        }

        ui.renderGameOver(renderer); // Oyun bitti ekranını ui çiziyor
    }

    SDL_RenderPresent(renderer); // Çizimleri ekrana yansıtma
}

// Oyun kapatıldığında hafıza temizleme
void Game::clean() {
    ui.clean(); // Arayüz hafızasını UIManager temizliyor

    for (int i = 0; i < 9; i++) {
        moles[i].clean();
    }

    // Ses dosyasını ve Mixer sistemini hafızadan temizleme
    if (hitSound != nullptr) {
        Mix_FreeChunk(hitSound);
    }
    Mix_CloseAudio();

    // Arkaplan hafızasını temizleme
    if (menuBgTexture != nullptr) {
        SDL_DestroyTexture(menuBgTexture);
    }
    if (gameBgTexture != nullptr) {
        SDL_DestroyTexture(gameBgTexture);
    }

    SDL_DestroyWindow(window);
    SDL_DestroyRenderer(renderer);
    SDL_Quit();
}

// Oyunun çalışma durumunu döndürme
bool Game::running() {
    return isRunning;
}
