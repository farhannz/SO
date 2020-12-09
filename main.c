#include "core.h"
// int posX = (W/2) % W;
// int posY = (H/2) % H;
// struct player p1,p2;
int yTest = 0;
struct player p1,p2;
int main(int argc, char *argv[])
{
    srand(time(NULL));
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
    p2.posX = (W - W/4) % W;
    p2.posY = (H/2) % H;
    // Variable untuk input event dan argument
    struct inputEvent ev;//, ev2;
    struct eventArg evArg;//, evArg2;
    // Memasukkan argumen untuk eventHandler
    // Player1
    evArg.evCodes = EV_KEY;
    evArg.event = &ev;
    // Player 2
    // evArg2.evCodes = EV_KEY;
    // evArg2.event = &ev2;
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
    createEmpty(&keyBuffer);
    struct updateArg arg,arg2;
    pthread_create(&tid0,NULL,GetInputFromUser,(void *) &evArg);
    // pthread_create(&tid1,NULL,GetInputFromUser,(void *) &evArg2);
    // player 1
    arg.event = &ev;
    arg.screen = &screen;
    arg.p1 = &p1;
    arg.p2 = &p2;
    // Player 2
    // arg2.event = &ev2;
    // arg2.screen = &screen;
    // arg2.player = &p2;
    // char testPiece[]="..X...X...X...X.";
    // char testPiece[16] = ".X...XX...X.....";
    char tetromino[7][17];
    strcpy(tetromino[0],"..X...X...X...X."); // Tetronimos 4x4
	strcpy(tetromino[1],"..X..XX...X.....");
	strcpy(tetromino[2],".....XX..XX.....");
	strcpy(tetromino[3],"..X..XX..X......");
	strcpy(tetromino[4],".X...XX...X.....");
	strcpy(tetromino[5],".X...X...XX.....");
	strcpy(tetromino[6],"..X...X..XX.....");
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
           
            // clear();
            system("clear");
            gotoxy(0,0);
            printf("Frametime : %ld ms FPS : %g\n",delta,(double)frames/delta);
            printf("TEST USER INPUT\n");
            printf("P1 : %c P2:%c\n",p1.code + 65, p2.code+65);
            UPDATE(&arg);
            // pthread_create(&tid2,NULL,UPDATE,(void*)&arg);
            // pthread_join(tid2,NULL);
            // Test unit tetris dan kecepatan turunnya
            int px, py;
            for(py = 0;py < 4;++py)
            {
                for(px = 0;px < 4;++px)
                {
                    //getIndex(x,y);
                    screen.screen[getIndex(px  + p1.posX, (py+yTest)%(screen.height+5),screen)] = '-';
                    screen.screen[getIndex(px  + p2.posX, (py+yTest)%(screen.height+5),screen)] = '-';
                    // screen.screen[((py+yTest+2)%screen.height) * screen.width + (px + W/2)] = '-';
                }
            }
            
            int currentPiece,currentPiece2;
            printf("%d\n",yTest % screen.height);

            if(yTest %16 == 0 && yTest != 0)
            {
                currentPiece = rand()%7;
                currentPiece2 = rand()%7;
            }
            if(speed >=20)
            {
                speed =0;
                yTest++;
                yTest %= screen.height + 5;
            }
            // int currentPiece = rand()%5;
            for(py = 0;py < 4;++py)
            {
                for(px = 0;px < 4;++px)
                {
                    if(tetromino[currentPiece][(py)*4+px] != '.')
                    {
                        screen.screen[getIndex(px + p1.posX, (py+yTest)%(screen.height+5),screen)] = 'T';
                        // screen.screen[((py+yTest+2)%screen.height) * screen.width + (px + W/2)] = 'T';
                    }
                    if(tetromino[currentPiece2][(py)*4+px] != '.')
                    {
                        screen.screen[getIndex(px + p2.posX, (py+yTest)%(screen.height+5),screen)] = 'G';
                        // screen.screen[((py+yTest+2)%screen.height) * screen.width + (px + W/2)] = 'T';
                    }
                }
            }
            // Render screen ke terminal
            DRAW(screen);
            // frames = 0;
            start = end;
            speed++;
            frames = 0;
        }
            // start = end;
        // }
        ++frames;
    }
    // pthread_join(tid0,NULL);
    printf("Program is done\n%d\n",evArg.event->code);
    tcsetattr(STDIN_FILENO,TCSANOW,&old);
    return 0;
}
