#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>

#define WIDTH 640
#define HEIGHT 480

SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;

int playerX = WIDTH / 2;
int playerY = HEIGHT - 50;
int bulletX, bulletY;
int enemyX, enemyY;
int score = 0;
int gameOver = 0;
int playerSize = 25;

// Düşmanı ekrana çizmek için kullanılan fonksiyon
void drawEnemy(int x, int y, int w, int h, SDL_Color color) {
    SDL_Rect rect = {x, y, w, h};
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    SDL_RenderFillRect(renderer, &rect);
}

// Oyuncuyu ekrana çizmek için kullanılan fonksiyon
void drawPlayer(SDL_Renderer* renderer, int x1, int y1, int x2, int y2, int x3, int y3, SDL_Color color) {
    SDL_RenderDrawLine(renderer, x1, y1, x2, y2);
    SDL_RenderDrawLine(renderer, x2, y2, x3, y3);
    SDL_RenderDrawLine(renderer, x3, y3, x1, y1);
}

int main() {
    // SDL başlatılıyor
    SDL_Init(SDL_INIT_VIDEO);
    // Pencere oluşturuluyor
    window = SDL_CreateWindow("Space Shooter", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIDTH, HEIGHT, SDL_WINDOW_SHOWN);
    // Renderer oluşturuluyor
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    SDL_Event event;
    // Oyun döngüsü
    while (!gameOver) {
        // Eventleri kontrol etme
        while (SDL_PollEvent(&event)) {
            // Çıkış kontrolü
            if (event.type == SDL_QUIT) {
                gameOver = 1;
            }
            // Tuş kontrolü
            if (event.type == SDL_KEYDOWN) {
                // Sol tuşa basılırsa ve oyuncu ekranın solunda değilse
                if (event.key.keysym.sym == SDLK_LEFT && playerX > 0) {
                    playerX -= 20;
                }
                // Sağ tuşa basılırsa ve oyuncu ekranın sağında değilse
                if (event.key.keysym.sym == SDLK_RIGHT && playerX < WIDTH - 50) {
                    playerX += 20;
                }
                // Boşluk tuşuna basılırsa ve mermi ekrandan çıkmışsa
                if (event.key.keysym.sym == SDLK_SPACE && bulletY <= 0) {
                    bulletX = playerX -3;
                    bulletY = playerY - 20;
                }
            }
        }

        // Mermiyi ve düşmanı hareket ettirme
        bulletY -= 5;
        if (bulletY < 0) {
            bulletY = -1;
        }
        enemyY += 3;
        if (enemyY > HEIGHT) {
            enemyY = -50;
            enemyX = rand() % (WIDTH - 50);
        }

        // Mermi düşmana çarparsa
        if (bulletX >= enemyX && bulletX <= enemyX + 50 && bulletY >= enemyY && bulletY <= enemyY + 50) {
            score++;
            bulletY = -1;
            enemyY = -50;
            enemyX = rand() % (WIDTH - 50);
        }

        // Oyuncu düşmana çarparsa
        if (playerX >= enemyX && playerX <= enemyX + 50 && playerY >= enemyY && playerY <= enemyY + 50) {
            gameOver = 1;
        }

        // Ekranı temizle
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        // Oyuncu ve düşmanı ekrana çiz
        SDL_Color whiteColor = {255, 255, 255, 255};
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
        SDL_Color red = {255, 0, 0};
        drawPlayer(renderer, playerX, playerY - playerSize, playerX - playerSize, playerY + playerSize, playerX + playerSize, playerY + playerSize, red);
        drawEnemy(bulletX, bulletY, 5, 20, whiteColor); // Mermiyi ekrana çiz
        drawEnemy(enemyX, enemyY, 50, 50, whiteColor); // Düşmanı ekrana çiz
        SDL_RenderPresent(renderer); // Ekranı güncelle
        SDL_Delay(10); // 10ms beklet
    }

    // Oyun bitti, renderer ve pencere temizleniyor
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    // Skor ekrana yazdırılıyor
    printf("Oyun bitti skorunuz : %d\n", score);

    return 0;
}

