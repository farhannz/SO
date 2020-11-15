#include "core.h"

int main(int argc, char *argv[])
{
    // Backup tampilan dari termios
    struct termios old;
    tcgetattr(STDIN_FILENO,&old);
    // Membuat thread
    pthread_t tid0;
    // Variable untuk input event dan argument
    struct inputEvent ev;
    struct eventArg evArg;
    // Memasukkan argumen untuk eventHandler
    evArg.evCodes = EV_KEY;
    evArg.event = &ev;
    // User Input Loop();
    // Game Loop
    int gameLoop = 1;
    clock_t start, end;
    start = clock();
    int maxFps = 60;
    double period = 1.0/maxFps;
    int frames = 0;
    // Membuat thread untuk mendapatkan input dari user dengan memanggil fungsi GetImputFromUser dengan argumen
    // struct eventArg
    pthread_create(&tid0,NULL,GetInputFromUser,(void *) &evArg);
    while(gameLoop && evArg.event->code != KEY_Q)
    {
        //Game timing================

        // UPDATE();
        end = clock();
        double delta = (end - start) / CLOCKS_PER_SEC;
        if(delta >= period)
        {
            system("clear");
            printf("FPS : %0.2lf\n",frames/delta);
            DRAW();
            frames = 0;
            start = end;
        }
        ++frames;
    }
    // pthread_join(tid0,NULL);
    printf("Program is done\n%d\n",evArg.event->code);
    tcsetattr(STDIN_FILENO,TCSANOW,&old);
    return 0;
}