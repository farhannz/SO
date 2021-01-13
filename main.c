#include "core.h"

int yTest = 0;
struct player p1,p2;
char scan;
char tetromino[7][17];

int main(int argc, char *argv[])
{
//=====================================================================
    clear();
    printf("=================================================\n");
    printf("         Selamat Datang di Tetris Battle\n");
    printf("                  Oleh Kelompok 4\n");
    printf("=================================================\n");
    printf("Cara Bermain :\n");
    printf("1. Untuk memulai bermain kamu bisa ketik 'Y',\n");
    printf("   sedangkan untuk keluar dari permainan kamu bisa\n");
    printf("   ketik 'T'\n");
    printf("2. Jika kamu ingin keluar ketika permainan sedang\n");
    printf("   berjalan, kamu hanya perlu klik tombol 'Q'\n");
    printf("   pada keyboard.\n");
    printf("=================================================\n");
    printf("   Apakah kamu ingin bermain? Y/T\n");
    scan = inputKey();
    scan = (scan >= 97 && scan <=127) ? scan - 97 : scan - 65;
    createEmpty(&keyBuffer);
//=====================================================================

    if(scan == KEY_Y){
        srand(time(NULL));
        // Backup tampilan dari termios
        struct termios old;
        tcgetattr(STDIN_FILENO,&old);
        // Membuat thread
        pthread_t tid0;
        pthread_t tid1;
        pthread_t tid2;
        pthread_t tid3;

//============================PLAYER===================================
        // Player 1
        initPlayer(&p1,1,1,pW/2, 0,100,0);
        // Player 2
        initPlayer(&p2,2,2,pW/2,0,100,0);


        // Variable untuk input event dan argument
        struct inputEvent ev;//, ev2;
        struct eventArg evArg;//, evArg2;
        // Memasukkan argumen untuk eventHandler
        evArg.evCodes = EV_KEY;
        evArg.event = &ev;
        // Game Loop
        int gameLoop = 1;
        // clock_t start, end;
        // start = clock();
        struct timespec start,end;
        clock_gettime(CLOCK_MONOTONIC,&start);
        int maxFps = 60;
        double period = 1000.0/(double)maxFps;
        int frames = 0;
        // Membuat thread untuk mendapatkan input dari user dengan memanggil fungsi GetImputFromUser dengan argumen
        // struct eventArg
        struct winScreen screen, playField1, playField2;
        createScreen(H,W, &screen);
        createScreen(pH,pW, &playField1);
        createScreen(pH,pW, &playField2);
        struct updateArg arg,arg2;
        // User Input Loop();
        pthread_create(&tid0,NULL,GetInputFromUser,(void *) &evArg);
        // pthread_create(&tid1,NULL,GetInputFromUser,(void *) &evArg2);

//=====================================================================
        arg.event = &ev;
        arg.screen = &screen;
        arg.p1 = &p1;
        arg.p2 = &p2;
        // char tetromino[7][17];
        strcpy(tetromino[0],"..X...X...X...X."); // Tetronimos 4x4
        strcpy(tetromino[1],"..X..XX...X.....");
        strcpy(tetromino[2],".....XX..XX.....");
        strcpy(tetromino[3],"..X..XX..X......");
        strcpy(tetromino[4],".X...XX...X.....");
        strcpy(tetromino[5],".X...X...XX.....");
        strcpy(tetromino[6],"..X...X..XX.....");
        int speed = 0;
        int gameOver = 0;
//============================GAME LOOP=================================
        while(gameLoop)
        {
            //Game timing================
            clock_gettime(CLOCK_MONOTONIC,&end);
            u_int64_t delta = (end.tv_sec - start.tv_sec) * 1000 + (end.tv_nsec - start.tv_nsec)/1e6;
            if(delta >= period)
            {
                clear();
                // system("clear");
                // gotoxy(0,0);
                
                // printf("Frametime : %ld ms FPS : %g\n",delta,(double)frames/delta);
                // printf("TEST USER INPUT\n");
                // printf("P1 : %c P2:%c\n",p1.code + 65, p2.code+65);
                printf("===================== Tetris Battle =======================\n");
                if(!isEmpty(keyBuffer)){
                    inputan = dequeue(&keyBuffer);
                    ev.code = (inputan >= 97 && inputan <=127) ? inputan - 97 : inputan - 65;
                }
                else{
                    inputan = KEY_NONE;
                }
                
                // UPDATE(&arg);
                pthread_create(&tid1,NULL,UPDATEP1,&arg);
                pthread_create(&tid2,NULL,UPDATEP2,&arg);
                pthread_join(tid1,NULL);
                pthread_join(tid2,NULL);
                // Render screen ke terminal
                struct drawArg drawArg;
                drawArg.p1 = &p1;
                drawArg.p2 = &p2;
                drawArg.screen = &screen;
                drawArg.tetromino = tetromino;
                pthread_create(&tid3, NULL,DRAW,&drawArg);
                pthread_join(tid3,NULL);
                printf("=====================Player 1=======================\n");
                printf("  W = Rotasi || S = Bawah || A = Kiri || D = Kanan\n");
                printf("=====================Player 2=======================\n");
                printf("  I = Rotasi || K = Bawah || J = Kiri || L = Kanan\n");
                printf("====================================================\n");
                // DRAW(&p1,&p2,&screen,tetromino);

                start = end;
                speed++;
                frames = 0;      
                gameOver =  p1.gameOver || p2.gameOver;
                if(inputan == KEY_Q || gameOver)
                {
                    gameLoop = 0;
                    break;
                }
            }
            ++frames;
        }
        clear();
        // printf("Program is done\n%d\n",evArg.event->code);
        char pemenang = (p1.gameOver) ? '2' : '1';
        char bufferScore[50];
        sprintf(bufferScore,"Scores : %09d", (p1.gameOver) ? p2.score : p1.score);
        if(gameOver)
        {
            printf("=====================GAME OVER=======================\n");
            printf("=                                                   =\n");
            printf("=                    PEMENANG :                     =\n");
            printf("=                       P%c                          =\n",pemenang);
            printf("=                %s                 =\n",bufferScore);
            printf("=====================================================\n");
            printf("\nPencet Q, untuk keluar dari game!\n");
        }
        pthread_join(tid0,NULL);
        clear();
        printf("Anda telah keluar dari permainan!\n");
        tcsetattr(STDIN_FILENO,TCSANOW,&old);
        exit(0);
    }
    else
    {
        clear();
        printf("Terima kasih sudah mencoba!\n");
    }
    return 0;
}
