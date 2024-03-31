#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <unistd.h>

#ifdef _WIN32
#include <conio.h>
#else
#include <termios.h>
#include <fcntl.h>
#endif

#define WIDTH 40
#define HEIGHT 20

// Oyuncunun konumunu ve skorunu 
int playerX, playerY;
int bulletX, bulletY;
int enemyX, enemyY;
int score = 0;

// Oyunun bitip bitmediğini kontrol eden değişken
bool gameOver = false;

// Oyun alanındaki çarpışma ve kaçırma sayıları
int crashes = 0;
int passes = 0;

// Windows ve diğer işletim sistemleri için klavye girişi iiçin ayar
#ifdef _WIN32
int kbhit(void)
{
    return _kbhit();
}

int getch(void)
{
    return _getch();
}
#else
int kbhit(void)
{
    struct termios oldt, newt;
    int ch;
    int oldf;

    // Terminal ayarlarını 
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);

    // Girişi oku
    ch = getchar();

    // Terminal ayarlarını geri yükle
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    fcntl(STDIN_FILENO, F_SETFL, oldf);

    if(ch != EOF)
    {
        ungetc(ch, stdin);
        return 1;
    }

    return 0;
}

int getch(void)
{
    int ch;
    struct termios oldt;
    struct termios newt;

    // Terminal ayarlarını değiştir
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    ch = getchar();
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);

    return ch;
}
#endif

// Oyunu başlatmak için
void Setup()
{
    // Oyuncuyu ortaya, mermiyi ve düşmanı rastgele konumlara yerleştir
    playerX = WIDTH / 2;
    playerY = HEIGHT - 1;
    bulletX = -1;
    bulletY = -1;
    enemyX = rand() % WIDTH;
    enemyY = 0;
    score = 0;
    crashes = 0;
    passes = 0;
}

// Oyun alanını çizen fonksiyon
void Draw()
{
    system("clear");
    // Üst çizgi
    for (int i = 0; i < WIDTH + 2; i++)
        printf("-");
    printf("\n");

    // Oyun alanı
    for (int y = 0; y < HEIGHT; y++)
    {
        for (int x = 0; x < WIDTH; x++)
        {
            if (x == 0 || x == WIDTH - 1)
                printf("|");
            else if (x == playerX && y == playerY)
                printf("^");
            else if (x == bulletX && y == bulletY)
                printf("|");
            else if (x == enemyX && y == enemyY)
                printf("-+-");
            else
                printf(" ");
        }
        printf("\n");
    }

    // Alt çizgi
    for (int i = 0; i < WIDTH + 2; i++)
        printf("-");
    printf("\n");

    // Skor ve çarpışma/kaçırma sayısı
    printf("Score: %d\n", score);
    printf("Crashes: %d\n", crashes);
    printf("Passes: %d\n", passes);
}

// Kullanıcı girişlerini işleyen fonksiyon
void Input()
{
    if (kbhit())
    {
        switch (getch())
        {
            case 'a':
                if (playerX > 1)
                    playerX--;
                break;
            case 'd':
                if (playerX < WIDTH - 2)
                    playerX++;
                break;
            case ' ':
                if (bulletY == -1)
                {
                    bulletX = playerX;
                    bulletY = playerY - 1;
                }
                break;
            case 'q':
                gameOver = true;
                break;
        }
    }
}

// Oyun mantığını işleyen fonksiyon
void Logic()
{
    // Düşmanın hareketi
    enemyY++;
    // Düşmanın alt sınırı geçmesi durumunda
    if (enemyY == HEIGHT)
    {
        enemyX = rand() % WIDTH; // Düşmanı rastgele bir yere yerleştir
        enemyY = 0; // Yeni düşmanın yüksekliğini sıfırla
        passes++; // Geçilen düşman sayısını artır
        // Eğer belirli bir sayıda düşman geçilirse oyunu bitir
        if (passes == 6)
        {
            gameOver = true;
            return;
        }
    }

    // Mermi hareketi ve çarpışma kontrolü
    if (bulletY > -1)
    {
        bulletY--;
        if (bulletY == 0)
        {
            bulletY = -1;
        }
        else if (bulletX == enemyX && bulletY == enemyY)
        {
            score++; // Skoru artır
            enemyX = rand() % WIDTH; // Yeni düşman oluştur
            enemyY = 0; // Yeni düşmanın yüksekliğini sıfırla
            bulletY = -1; // Mermiyi sil
        }
    }

    // Çarpışma kontrolü
    if (playerX == enemyX && playerY == enemyY)
    {
        crashes++; // Çarpışma sayısını artır
        // Eğer belirli bir sayıda çarpışma gerçekleşirse oyunu bitir
        if (crashes == 3)
        {
            gameOver = true;
            return;
        }
    }
}

// Ana fonksiyon
int main()
{
    srand(time(NULL)); // Rastgele sayı üreteci başlat

    Setup(); // Oyunu başlat

    // Oyun döngüsü
    while (!gameOver)
    {
        Draw(); // Oyun alanını çiz
        Input(); // Kullanıcı girişlerini işle
        Logic(); // Oyun mantığını işle
        usleep(100000); // Oyun hızını kontrol etmek için gecikme ekle
    }

    // Oyun bittiğinde skoru ekrana yazdır
    printf("Game Over!\n");
    printf("Your score is: %d\n", score);

    return 0;
}

