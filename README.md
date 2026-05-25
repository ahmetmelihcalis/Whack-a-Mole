# Whack-a-Mole

## Kocaeli Üniversitesi Yazılım Mühendisliği - Programlama II Dersi Oyun Projesi

## Geliştirici

**Ahmet Melih Çalış**  
**Öğrenci No:** 240229027

---

## Proje Özeti

Bu proje, **C++** ve **SDL2** kullanılarak geliştirilmiş, zamana karşı oynanan bir **Whack-a-Mole** oyunudur.

Oyuncu, deliklerden rastgele çıkan köstebeklere fare ile tıklayarak puan toplar. Süre dolduğunda oyun sona erer ve sonuç ekranı ile en yüksek skorlar gösterilir.

---

## Özellikler

- Köstebek çıkış ve giriş animasyonları
- Rastgele delik seçimi
- Fare tıklama algılama ve çarpışma kontrolü
- Skor sayacı ve geri sayım süresi
- Ana menü, sonuç ekranı ve yüksek skorlar ekranı
- En yüksek 5 skoru kaydetme ve listeleme
- Vuruş efekti ve ses desteği

---

## Kullanılan Teknolojiler

### Programlama Dili

- **C++**

### Kütüphaneler

- **SDL2**
- **SDL2_image**
- **SDL2_ttf**
- **SDL2_mixer**

---

## Proje Yapısı

```text
Whack-a-Mole/
├── assets/
├── include/
│   ├── Game.h
│   ├── Mole.h
│   └── UIManager.h
├── src/
│   ├── Game.cpp
│   ├── Mole.cpp
│   ├── UIManager.cpp
│   └── main.cpp
├── CMakeLists.txt
└── README.md
```

## Oyun Akışı

1. Oyuncu ana menüden oyunu başlatır.
2. Köstebekler farklı deliklerden rastgele çıkar.
3. Oyuncu görünen köstebeklere tıklayarak puan kazanır.
4. Süre dolunca oyun biter.
5. Sonuç ekranında skor ve yüksek skorlar listesi gösterilir.

## Amaç

Bu projenin amacı, Programlama II dersi kapsamında C++ programlama dili ile nesne yönelimli programlama mantığını kullanarak SDL2 tabanlı etkileşimli bir oyun geliştirmektir.
