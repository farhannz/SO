#include "core.h"

// int posX = (W/2) % W;
// int posY = (H/2) % H;
// struct player p1,p2;
int yTest = 0;
int main(int argc, char *argv[])
{
    // Backup tampilan dari termios
    struct termios old;
    tcgetattr(STDIN_FILENO,&old);
    // Membuat thread
    pthread_t tid0;
    pthread_t tid1;
    pthread_t tid2;
    //player
    // Player 1
    p1.code = 1;
    p1.posX = (W/4) %W;
    p1.posY = (H/2) %H;
    // Player 2
    p2.code = 2;
    p2.posX = (W/6) % W;
    p2.posY = (H/2) % H;
    // Variable untuk input event dan argument
    struct inputEvent ev, ev2;
    struct eventArg evArg, evArg2;
    // Memasukkan argumen untuk eventHandler
    // Player1
    evArg.evCodes = EV_KEY;
    evArg.event = &ev;
    // Player 2
    evArg2.evCodes = EV_KEY;
    evArg2.event = &ev2;
    // User Input Loop();
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
    struct winScreen screen;
    createScreen(H,W, &screen);
    pthread_create(&tid0,NULL,GetInputFromUser,(void *) &evArg);
    // pthread_create(&tid1,NULL,GetInputFromUser,(void *) &evArg2);

    struct updateArg arg,arg2;
    // player 1
    arg.event = &ev;
    arg.screen = &screen;
    arg.player = &p1;
    // Player 2
    arg2.event = &ev2;
    arg2.screen = &screen;
    arg2.player = &p2;
    char testPiece[]="..X...X...X...X.";
    int speed = 0;
    while(gameLoop && evArg.event->code != KEY_Q)
    {
        //Game timing================
        clock_gettime(CLOCK_MONOTONIC,&end);
        // UPDATE();
        // end = clock();
        // double delta = (end - start) / CLOCKS_PER_SEC;
        // if(delta >= period)
    // {
        u_int64_t delta = (end.tv_sec - start.tv_sec) * 1000 + (end.tv_nsec - start.tv_nsec)/1e6;
       //  clear();
        if(delta >= period)
        {
            system("clear");
            gotoxy(0,0);
            // printf("FPS : %ld %d %g\n",delta, frames, (double)frames/delta);
            printf("TEST USER INPUT\n");
            printf("P1 : %c P2:%c\n",ev.code + 65, p2.code+65);
            UPDATE(&arg);
            // pthread_create(&tid2,NULL,UPDATE,(void*)&arg);
            int px, py;
            if(speed >=20)
            {
                speed =0;
                yTest++;
            }
            for(py = 0;py < 4;++py)
            {
                for(px = 0;px < 4;++px)
                {
                    if(testPiece[py*4 + px] !='.')
                    {
                        screen.screen[((yTest + py + 2))%screen.height*screen.width + px + W/2] = 'T';
                    }
                }
            }
            for(px = 0;px < 4;++px)
            {
                screen.screen[((yTest +1))%screen.height*screen.width +  px + W/2] = '-';
            }
            DRAW(screen);
            // frames = 0;
            start = end;
            speed++;
        }
            frames = 0;
            // start = end;
        // }
        ++frames;
    }
    // pthread_join(tid0,NULL);
    printf("Program is done\n%d\n",evArg.event->code);
    tcsetattr(STDIN_FILENO,TCSANOW,&old);
    return 0;
}
