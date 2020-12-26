#ifndef CORE_H
#define CORE_H

#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <termios.h>
#include <pthread.h>
#include <string.h>
#include <sys/resource.h>
#include "queue.h"
// #include <sys/ioctl.h>
// #include <string.h>
// #include <wchar.h>
// List dari evCodes yang merupakan event codes yang akan dipakai
#define EV_KEY 0
#define clear() printf("\033[H\033[J")
#define gotoxy(x,y) printf("\033[%d;%dH", (y), (x))


// List dari keyboard codes input
#define KEY_A 0
#define KEY_B 1
#define KEY_C 2
#define KEY_D 3
#define KEY_E 4
#define KEY_F 5
#define KEY_G 6
#define KEY_H 7
#define KEY_I 8
#define KEY_J 9
#define KEY_K 10
#define KEY_L 11
#define KEY_M 12
#define KEY_N 13
#define KEY_O 14
#define KEY_P 15
#define KEY_Q 16
#define KEY_R 17
#define KEY_S 18
#define KEY_T 19
#define KEY_U 20
#define KEY_V 21
#define KEY_W 22
#define KEY_X 23
#define KEY_Y 24
#define KEY_Z 25
#define KEY_NONE -99999

//Keyboard state apakah pressed, repeat
#define KEY_PRESSED 0
#define KEY_REPEATED 1

// Besar dari screen
#define W 100
#define H 15
// Besar dari field tetris
#define pW 13
#define pH 12
pthread_mutex_t mutex;

int posX;
int posY;
// Struct
static struct termios old, current;
struct inputEvent
{
    int type;           // 0 = KEYBOARD INPUT 
    int code;           // Nilai dari keyboard yang diinput
    int val;            // 0 = Tekan / Pressed, 1 = Lepas / Released, 2 = Tahan / Repeat
};

struct eventArg {
    int evCodes;
    struct inputEvent *event;
};

struct updateArg {
    struct inputEvent *event;
    struct winScreen *screen;
    struct player *p1,*p2;
};

struct winScreen {
    int width, height;
    char *screen;
};

struct player {
    int posX;
    int posY;
    int pemain;
    int code;
    int rotasi;
    double health;
    int score;
    int curBalok;
    int tetris;
    int speed;
    struct winScreen *playField;
};
//Input buffer
queue keyBuffer;
//Key yang akan diproses
char inputan;
char tetromino[7][17];
//
void initPlayer(struct player* player,int pemain, int code,int posX,int posY,double health,int score);
void initTermios(void);
void resetTermios(void);
void EventHandler(int evCodes, struct inputEvent * event);
void *GetInputFromUser(void * args);
// void UPDATE(struct inputEvent *event, struct winScreen *screen, struct player *player);
void *UPDATE(void *args);
void *UPDATEP1(void *args);
void *UPDATEP2(void *args);
void DRAW(struct player *p1, struct player *p2,struct winScreen *screen, char **tetromino);
void drawPieces(struct player *player, struct winScreen *screen);
void drawPlayerAttributes(struct player, struct winScreen *screen, int coorX, int coorY);
void drawField(struct player *player, int coorX,int coorY, struct winScreen *screen);
void drawScore(struct player player,int coorX,int coorY,struct winScreen *screen);
void drawHealth(struct player player,int coorX,int coorY,struct winScreen *screen);
void createScreen(int height, int width, struct winScreen * screen);
char inputKey(void);      // Mengembalikan character dari keyboard input
char* getKeyState(struct inputEvent ev);
void clearTrail(int x , int y, struct player *player);
int isHit(int x, int y, int rotasi, struct player *player);             // Hit hitbox
int indexRotasi(int rotasi, int x, int y);      // Index hasil rotasi
int keyHit();           // check apakah keyboard ditekan atau tidak
int getIndex(int x,int y, struct winScreen screen);
#endif // !CORE_H
