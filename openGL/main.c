#include <GL/glut.h> // OpenGL kütüphanesi
#include <SOIL/SOIL.h> // SOIL kütüphanesi, resim yükleme için
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define SHIP_SIZE 60 // Uzay gemisi boyutu
#define ENEMY_SIZE 35 // Düşman boyutu
#define BULLET_SIZE 7 // Mermi boyutu
#define SHIP_SPEED 5 // Uzay gemisi hızı
#define ENEMY_SPEED 3 // Düşman hızı
#define BULLET_SPEED 10 // Mermi hızı
#define MAX_BULLETS 7 // Maksimum mermi sayısı
#define FPS 60 // Oyun hızı

GLuint shipTexture; // Uzay gemisi resmi
GLuint enemyTexture; // Düşman resmi
GLuint bulletTexture; // Mermi resmi

int shipX, shipY; // Uzay gemisinin konumu
int enemyX, enemyY; // Düşmanın konumu
int bulletsX[MAX_BULLETS], bulletsY[MAX_BULLETS]; // Mermilerin konumu
int bulletFired[MAX_BULLETS]; // Mermi atılıp atılmadığını tutan dizi
int score = 0; // Oyuncunun skoru
int crashes = 0; // Çarpışma sayısı
int gameRunning = 1; // Oyunun devam edip etmediği
int keyLeft = 0, keyRight = 0, keySpace = 0; // Klavye girişlerini tutan değişkenler

void loadTextures() {
    // Resimleri yükle
    shipTexture = SOIL_load_OGL_texture("ship.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_INVERT_Y);
    enemyTexture = SOIL_load_OGL_texture("stone.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_INVERT_Y);
    bulletTexture = SOIL_load_OGL_texture("bullet.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_INVERT_Y);
}

void checkCollision() {
    // Uzay gemisi ile düşman arasındaki çarpışmayı kontrol et
    float distance = sqrt((shipX - enemyX) * (shipX - enemyX) + (shipY - enemyY) * (shipY - enemyY));
    if (distance < SHIP_SIZE + ENEMY_SIZE) {
        crashes++;
        if (crashes >= 3) {
            gameRunning = 0;
            printf("Oyun Bitti! Skorunuz : %d\n", score);
        }
    }

    // Mermiler ile düşman arasındaki çarpışmayı kontrol et
    for (int i = 0; i < MAX_BULLETS; i++) {
        if (bulletFired[i]) {
            distance = sqrt((bulletsX[i] - enemyX) * (bulletsX[i] - enemyX) + (bulletsY[i] - enemyY) * (bulletsY[i] - enemyY));
            if (distance < BULLET_SIZE + ENEMY_SIZE) {
                score++;
                enemyX = rand() % (glutGet(GLUT_WINDOW_WIDTH) - ENEMY_SIZE);
                enemyY = glutGet(GLUT_WINDOW_HEIGHT) - 50;
                bulletFired[i] = 0;
            }
        }
    }
}

void updateGame(int value) {
    if (gameRunning) {
        // Uzay gemisinin hareketini güncelle
        if (keyLeft) {
            shipX = fmax(shipX - SHIP_SPEED, 0); // Uzay gemisinin sol hareketini sınırla
        }
        if (keyRight) {
            shipX = fmin(shipX + SHIP_SPEED, glutGet(GLUT_WINDOW_WIDTH) - SHIP_SIZE); // Uzay gemisinin sağ hareketini sınırla
        }

        // Düşmanın hareketini güncelle
        enemyY -= ENEMY_SPEED;
        if (enemyY < 0) {
            enemyX = rand() % (glutGet(GLUT_WINDOW_WIDTH) - ENEMY_SIZE);
            enemyY = glutGet(GLUT_WINDOW_HEIGHT) - 50;
        }

        // Mermilerin pozisyonunu güncelle
        for (int i = 0; i < MAX_BULLETS; i++) {
            if (bulletFired[i]) {
                bulletsY[i] += BULLET_SPEED;
                if (bulletsY[i] > glutGet(GLUT_WINDOW_HEIGHT)) {
                    bulletFired[i] = 0;
                }
            }
        }

        // Çarpışmaları kontrol et
        checkCollision();

        glutPostRedisplay();
        glutTimerFunc(1000 / FPS, updateGame, 0);
    }
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT);

    glEnable(GL_TEXTURE_2D);

    // Uzay gemisini çiz
    glBindTexture(GL_TEXTURE_2D, shipTexture);
    glBegin(GL_QUADS);
    glTexCoord2f(0, 0); glVertex2f(shipX, shipY);
    glTexCoord2f(1, 0); glVertex2f(shipX + SHIP_SIZE, shipY);
    glTexCoord2f(1, 1); glVertex2f(shipX + SHIP_SIZE, shipY + SHIP_SIZE);
    glTexCoord2f(0, 1); glVertex2f(shipX, shipY + SHIP_SIZE);
    glEnd();

    // Düşmanı çiz
    glBindTexture(GL_TEXTURE_2D, enemyTexture);
    glBegin(GL_QUADS);
    glTexCoord2f(0, 0); glVertex2f(enemyX, enemyY);
    glTexCoord2f(1, 0); glVertex2f(enemyX + ENEMY_SIZE, enemyY);
    glTexCoord2f(1, 1); glVertex2f(enemyX + ENEMY_SIZE, enemyY + ENEMY_SIZE);
    glTexCoord2f(0, 1); glVertex2f(enemyX, enemyY + ENEMY_SIZE);
    glEnd();

    // Mermileri çiz
    glBindTexture(GL_TEXTURE_2D, bulletTexture);
    glBegin(GL_QUADS);
    for (int i = 0; i < MAX_BULLETS; i++) {
        if (bulletFired[i]) {
            glTexCoord2f(0, 0); glVertex2f(bulletsX[i], bulletsY[i]);
            glTexCoord2f(1, 0); glVertex2f(bulletsX[i] + BULLET_SIZE, bulletsY[i]);
            glTexCoord2f(1, 1); glVertex2f(bulletsX[i] + BULLET_SIZE, bulletsY[i] + BULLET_SIZE);
            glTexCoord2f(0, 1); glVertex2f(bulletsX[i], bulletsY[i] + BULLET_SIZE);
        }
    }
    glEnd();

    glDisable(GL_TEXTURE_2D);

    glutSwapBuffers();
}

void keyboardDown(unsigned char key, int x, int y) {
    switch (key) {
        case 'a':
            keyLeft = 1;
            break;
        case 'd':
            keyRight = 1;
            break;
        case ' ':
            for (int i = 0; i < MAX_BULLETS; i++) {
                if (!bulletFired[i]) {
                    bulletsX[i] = shipX + SHIP_SIZE / 2 - BULLET_SIZE / 2;
                    bulletsY[i] = shipY + SHIP_SIZE;
                    bulletFired[i] = 1;
                    break;
                }
            }
            break;
        case 27: // ESC tuşu
            gameRunning = 0;
            printf("Oyundan çıkmak için penceriyi kapatın.\n");
            break;
    }
}

void keyboardUp(unsigned char key, int x, int y) {
    switch (key) {
        case 'a':
            keyLeft = 0;
            break;
        case 'd':
            keyRight = 0;
            break;
    }
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(800, 600);
    glutCreateWindow("Uzay Savaşı Oyunu");
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, 800, 0, 600);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    loadTextures();

    shipX = 400 - SHIP_SIZE / 2;
    shipY = 50;
    enemyX = rand() % (glutGet(GLUT_WINDOW_WIDTH) - ENEMY_SIZE);
    enemyY = glutGet(GLUT_WINDOW_HEIGHT) - 50;

    for (int i = 0; i < MAX_BULLETS; i++) {
        bulletFired[i] = 0;
    }

    glutDisplayFunc(display);
    glutKeyboardFunc(keyboardDown);
    glutKeyboardUpFunc(keyboardUp);
    glutTimerFunc(0, updateGame, 0);
    glutMainLoop();

    return 0;
}
void display() {
    glClear(GL_COLOR_BUFFER_BIT); // Ekranı temizle

    glEnable(GL_TEXTURE_2D); // 2D texture'ları kullan

    // Uzay gemisini çiz
    glBindTexture(GL_TEXTURE_2D, shipTexture);
    glBegin(GL_QUADS);
    glTexCoord2f(0, 0); glVertex2f(shipX, shipY);
    glTexCoord2f(1, 0); glVertex2f(shipX + SHIP_SIZE, shipY);
    glTexCoord2f(1, 1); glVertex2f(shipX + SHIP_SIZE, shipY + SHIP_SIZE);
    glTexCoord2f(0, 1); glVertex2f(shipX, shipY + SHIP_SIZE);
    glEnd();

    // Düşmanı çiz
    glBindTexture(GL_TEXTURE_2D, enemyTexture);
    glBegin(GL_QUADS);
    glTexCoord2f(0, 0); glVertex2f(enemyX, enemyY);
    glTexCoord2f(1, 0); glVertex2f(enemyX + ENEMY_SIZE, enemyY);
    glTexCoord2f(1, 1); glVertex2f(enemyX + ENEMY_SIZE, enemyY + ENEMY_SIZE);
    glTexCoord2f(0, 1); glVertex2f(enemyX, enemyY + ENEMY_SIZE);
    glEnd();

    // Mermileri çiz
    glBindTexture(GL_TEXTURE_2D, bulletTexture);
    glBegin(GL_QUADS);
    for (int i = 0; i < MAX_BULLETS; i++) {
        if (bulletFired[i]) {
            glTexCoord2f(0, 0); glVertex2f(bulletsX[i], bulletsY[i]);
            glTexCoord2f(1, 0); glVertex2f(bulletsX[i] + BULLET_SIZE, bulletsY[i]);
            glTexCoord2f(1, 1); glVertex2f(bulletsX[i] + BULLET_SIZE, bulletsY[i] + BULLET_SIZE);
            glTexCoord2f(0, 1); glVertex2f(bulletsX[i], bulletsY[i] + BULLET_SIZE);
        }
    }
    glEnd();

    glDisable(GL_TEXTURE_2D); // Texture'ları kapat

    glutSwapBuffers(); // Ekranı yenile
}

void keyboardDown(unsigned char key, int x, int y) {
    switch (key) {
        case 'a':
            keyLeft = 1; // 'a' tuşu basılı tutuluyor
            break;
        case 'd':
            keyRight = 1; // 'd' tuşu basılı tutuluyor
            break;
        case ' ':
            for (int i = 0; i < MAX_BULLETS; i++) {
                if (!bulletFired[i]) {
                    bulletsX[i] = shipX + SHIP_SIZE / 2 - BULLET_SIZE / 2;
                    bulletsY[i] = shipY + SHIP_SIZE;
                    bulletFired[i] = 1; // Mermi ateşlendi
                    break;
                }
            }
            break;
        case 27: // ESC tuşu
            gameRunning = 0; // Oyunu durdur
            printf("Oyundan çıkmak için penceriyi kapatın.\n");
            break;
    }
}

void keyboardUp(unsigned char key, int x, int y) {
    switch (key) {
        case 'a':
            keyLeft = 0; // 'a' tuşu serbest bırakıldı
            break;
        case 'd':
            keyRight = 0; // 'd' tuşu serbest bırakıldı
            break;
    }
}
// Tüm fonksiyonların main'de çağırılması
int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(800, 600);
    glutCreateWindow("Uzay Savaşı Oyunu");
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, 800, 0, 600);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    loadTextures();

    shipX = 400 - SHIP_SIZE / 2;
    shipY = 50;
    enemyX = rand() % (glutGet(GLUT_WINDOW_WIDTH) - ENEMY_SIZE);
    enemyY = glutGet(GLUT_WINDOW_HEIGHT) - 50;

    for (int i = 0; i < MAX_BULLETS; i++) {
        bulletFired[i] = 0;
    }

    glutDisplayFunc(display);
    glutKeyboardFunc(keyboardDown);
    glutKeyboardUpFunc(keyboardUp);
    glutTimerFunc(0, updateGame, 0);
    glutMainLoop();

    return 0;
}
