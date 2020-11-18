#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <termios.h>
#include <pthread.h>
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

// Besar dari windows
#define W 50
#define H 15

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
    struct player *player;
};

struct winScreen {
    int width, height;
    char *screen;
};

struct player {
    int posX;
    int posY;
    int code;
};

void initTermios(void);
void resetTermios(void);
void EventHandler(int evCodes, struct inputEvent * event);
void *GetInputFromUser(void * args);
// void UPDATE(struct inputEvent *event, struct winScreen *screen, struct player *player);
void *UPDATE(void *args);
void DRAW(struct winScreen screen);
void createScreen(int height, int width, struct winScreen * screen);
char inputKey(void);      // Mengembalikan character dari keyboard input
char* getKeyState(struct inputEvent ev);
int keyHit();           // check apakah keyboard ditekan atau tidak
void drawField(int rows,int columns);