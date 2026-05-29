# Whack-a-Mole

## Kocaeli Üniversitesi Yazılım Mühendisliği - Programlama II Dersi Oyun Projesi

## Geliştirici

**Ahmet Melih Çalış**  
**Öğrenci No:** 240229027

## Proje Özeti

Bu proje, **C++** ve **SDL2** kullanılarak geliştirilmiş, zamana karşı oynanan bir **Whack-a-Mole** oyunudur.

Oyuncu, deliklerden rastgele çıkan köstebeklere fare ile tıklayarak puan toplar. Süre dolduğunda oyun sona erer ve sonuç ekranı ile en yüksek skorlar gösterilir.

## Özellikler

- Köstebek çıkış ve giriş animasyonları
- Rastgele delik seçimi
- Fare tıklama algılama ve çarpışma kontrolü
- Skor sayacı ve geri sayım süresi
- Ana menü, sonuç ekranı ve yüksek skorlar ekranı
- En yüksek 5 skoru kaydetme ve listeleme
- Vuruş efekti ve ses desteği

## Kullanılan Teknolojiler

### Programlama Dili

- **C++**

### Kütüphaneler

- **SDL2**
- **SDL2_image**
- **SDL2_ttf**
- **SDL2_mixer**

## Proje Yapısı

```text
Whack-a-Mole/
├── assets/              # Oyun görselleri, ses dosyaları ve fontlar
├── docs/                # Proje raporu ve README görselleri
├── include/             # Header dosyaları
│   ├── Game.h
│   ├── Mole.h
│   └── UIManager.h
├── src/                 # Kaynak kod dosyaları
│   ├── Game.cpp
│   ├── Mole.cpp
│   ├── UIManager.cpp
│   └── main.cpp
├── CMakeLists.txt       # CMake derleme yapılandırması
└── README.md            # Proje açıklaması ve kullanım bilgileri
```

## Yazılım ve Fonksiyon Mimarisi

Proje, sorumlulukları birbirinden ayıran üç temel sınıf üzerine kurulmuştur:

- **Game**: Oyun döngüsünü, durum geçişlerini, süre yönetimini, skor kaydını ve genel akışı yönetir.
- **Mole**: Köstebeklerin görünürlük durumunu, animasyonlarını, tıklanma kontrolünü ve çizimini yönetir.
- **UIManager**: Menüleri, skor ve süre yazılarını, sonuç ekranlarını ve görsel efektleri yönetir.

### Sınıflar Arası İlişki

---

```mermaid
classDiagram
    class Game {
        +init()
        +handleEvents()
        +update()
        +render()
        +clean()
        +running()
        -loadHighScores()
        -saveHighScore()
    }

    class Mole {
        +init()
        +popUp()
        +hide()
        +checkClick()
        +update()
        +render()
        +isVisible()
        +clean()
    }

    class UIManager {
        +init()
        +updateScore()
        +updateTimer()
        +updateLeaderboard()
        +renderMenu()
        +renderPlaying()
        +renderGameOver()
        +renderHighScores()
        +renderEffects()
        +showHitEffect()
        +updateHitEffects()
        +clearHitEffects()
        +clean()
    }

    Game --> UIManager : kullanır
    Game --> Mole : 9 köstebeği yönetir
```

> **Not:** Bu diyagramda yalnızca projede tanımlanan `Game`, `Mole` ve `UIManager` sınıfları arasındaki ilişki gösterilmiştir. SDL2 ve alt kütüphanelere ait hazır fonksiyonlar bu sınıf diyagramına dahil edilmemiştir.

## Oyun Akışı

1. Oyuncu ana menüden oyunu başlatır.
2. Köstebekler farklı deliklerden rastgele çıkar.
3. Oyuncu görünen köstebeklere tıklayarak puan kazanır.
4. Süre dolunca oyun biter.
5. Sonuç ekranında skor ve yüksek skorlar listesi gösterilir.

### Oyun Akış Diyagramı

---

```mermaid
flowchart TD
    A[Program Başlar] --> B["Başlatma - init()"]
    B --> C{"Oyun Çalışıyor mu? - running()"}
    C -->|Evet| D["Olayları İşle - handleEvents()"]
    D --> E["Güncelle - update()"]
    E --> F["Ekrana Çiz - render()"]
    F --> G["Görüntü Akışını Dengelemek İçin Kısa Süre Bekle - FPS Ayarı"]
    G --> C
    C -->|Hayır| H["Temizle ve Çıkış - clean()"]
```

## Oyun Görselleri

### Ana Menü

![Ana Menü](docs/screenshots/main-page.png)

### Oyun Ekranı

![Oyun Ekranı](docs/screenshots/game-page.png)

### Oyun Sonu Ekranı

![Oyun Sonu Ekranı](docs/screenshots/end-game.png)

### Yüksek Skorlar Ekranı

![Yüksek Skorlar Ekranı](docs/screenshots/high-score.png)

## Proje Demo Videosu

Oyunun temel özelliklerini ve genel işleyişini görmek için aşağıdaki videoyu izleyebilirsiniz:

https://github.com/user-attachments/assets/f02d2488-d5d2-47e6-889e-c248b5a948e3

## Proje Raporu

Projenin detaylı raporuna aşağıdaki bağlantıdan ulaşabilirsiniz:

[Whack-a-Mole Proje Raporu](docs/Whack-a-Mole-Proje-Raporu.pdf)

## Kurulum ve Derleme Rehberi

Proje, **CMake** kullanılarak derlenmek üzere yapılandırılmıştır. Kodu derlemeden önce sisteminizde `C++ Derleyici (GCC/MinGW)`, `CMake` (minimum sürüm 3.16) ve `SDL2` kütüphanelerinin (SDL2, SDL2_image, SDL2_ttf, SDL2_mixer) kurulu olması gerekmektedir.

> **ÖNEMLİ NOT:** Projeyi CMake ile yapılandırmadan önce `build` adında bir derleme dizini oluşturmayı unutmayınız.

### Windows / MSYS2 Bağımlılık Kurulumu

Windows üzerinde MSYS2 UCRT64 ortamı kullanıyorsanız gerekli derleyici, CMake ve SDL2 kütüphanelerini aşağıdaki komutla kurabilirsiniz:

```bash
pacman -S --needed mingw-w64-ucrt-x86_64-gcc mingw-w64-ucrt-x86_64-cmake mingw-w64-ucrt-x86_64-make mingw-w64-ucrt-x86_64-SDL2 mingw-w64-ucrt-x86_64-SDL2_image mingw-w64-ucrt-x86_64-SDL2_ttf mingw-w64-ucrt-x86_64-SDL2_mixer
```

### Çalıştırma Adımları (Terminal / Komut Satırı)

1. Repoyu klonlayın ve proje klasörüne gidin:
```bash
git clone https://github.com/ahmetmelihcalis/Whack-a-Mole.git
cd Whack-a-Mole
```

2. Derleme dizini oluşturun ve içine girin:
```bash
mkdir build
cd build
```

3. CMake ile projeyi yapılandırın:
```bash
cmake -G "MinGW Makefiles" ..
```

Farklı bir CMake oluşturucusu kullanıyorsanız aşağıdaki komut da tercih edilebilir:
```bash
cmake ..
```

4. Projeyi derleyin:
```bash
cmake --build .
```

5. Oyunu çalıştırın:
```bash
# Windows (MSYS2/MinGW) için:
./WhackAMole.exe

# Linux/macOS için:
./WhackAMole
```

## Not

Projenin geliştirme süreci, yazılım mimarisi, kullanılan algoritmalar ve diğer teknik detaylar proje raporunda daha kapsamlı şekilde açıklanmıştır.
