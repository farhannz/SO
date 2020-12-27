#include "core.h"
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;


void initTermios(void)
{
    // Mengambil dari standard input (STDIN);
    tcgetattr(STDIN_FILENO,&old);
    current = old;
    // Set bahwa local mode dari terminal itu tanpa buffering dan tidak echo ( inputan tidak ditampilkan di terminal)
    current.c_lflag &= ~ICANON;
    current.c_lflag &= ~ECHO;
    // SET STDIN
    tcsetattr(STDIN_FILENO,TCSANOW, &current);
}

void resetTermios(void)
{
    // Reset terminal
    tcsetattr(STDIN_FILENO,TCSANOW,&old);
}

char inputKey(void)
{
  //Inisialisasi termios
  initTermios();
  // Mengammbil inputan keyboard
  char c= getchar();
  // Reset terminal
  resetTermios();
  return c;
}
void initPlayer(struct player* player,int pemain,int code,int posX,int posY,double health,int score)
{
  // Inisialisasi nilai awal dari player
  player->code = code;
  player->posX = posX;
  player->posY = posY;
  player->health = health;
  player->score = score;
  player->playField = (struct playField *)malloc(sizeof(struct playField));
  player->playField->width = pW;
  player->playField->height = pH;
  player->pemain = pemain;
  player->rotasi = 0;
  player->curBalok = 0;
  player->tetris = 0;
  player->speed = 0;
  player->gameOver = 0;
  player->playField->screen = (int*)malloc(pW*pH*sizeof(int));
  int i,j;
  for(i = 0;i< pH; ++i)
  {
    for(j = 0;j< pW; ++j)
    {
      if(j == 0 || j == pW - 1 || i == pH-1){
        player->playField->screen[ i * player->playField->width + j] = 9;
        // player->playField->screen[getIndex(j, i, *player->playField)] = 9;
      }
      else{
        player->playField->screen[ i * player->playField->width + j] = 0;
        // player->playField->screen[getIndex(j, i, *player->playField)] = 0;
      }
    }
  }
}
void * GetInputFromUser(void *args)
{
  struct eventArg *evArg = args;
  // Memanggil event handler untuk Input User
  // pthread_mutex_lock(&mutex);
  while(1 && evArg->event->code != KEY_Q)
  {
    EventHandler(evArg->evCodes,evArg->event);
  }
  // pthread_mutex_unlock(&mutex);
  // int x;
  // pthread_exit(&x);
}
int getIndex(int x, int y, struct winScreen screen)
{
  return y * screen.width + x;
}
void createScreen(int height, int width, struct winScreen *screen)
{
  screen->width = width;
  screen->height = height;
  screen->screen = (char*)malloc(screen->width * screen->height * sizeof(char));
  int i,j;
  // for(i = 0;i<screen->width * screen->height;++i) screen->screen[i] = '.';
  for(i = 0;i<screen->height;++i)
  {
    for(j = 0;j<screen->width;++j)
    {
      if(j == screen->width-1)
      {
        screen->screen[i * screen->width + j] = '\n';
      }
      else
      {
        screen->screen[i * screen->width + j] = '.';
      }
    }
  }
}


void EventHandler(int evCodes, struct inputEvent *event)
{
  if(evCodes == EV_KEY)
  {
    char key;
    if(keyHit())
    {
      key = inputKey();
      event->type = EV_KEY;
      event->code = (key >= 97 && key <=127) ? key - 97 : key - 65;
      enqueue(event->code,&keyBuffer);
      event->val  = 0;
    }
    // event->code = KEY_NONE;
  }
}

int keyHit()
{
  // Mengambil inputan dari keyboard
  char key = inputKey();
  // Jika ada inputan dari keyboard
  if(key != EOF)
  {
    // Mengembalikan karakter yang diterima ke dalam STDIN
    ungetc(key,stdin);
    return 1;
  }
  return 0;
}

int indexRotasi(int rotasi, int x , int y){
  int rot = 0;
  switch(rotasi % 4){
    case 0: // 0 Derajat
      rot = y * 4 + x;
      break;
    case 1: // 90 Derajat
      rot = 12 + y - (4 * x);
      break;
    case 2: // 180 derajat
      rot = 15 -(y* 4) - x;
      break;
    case 3: // 270 derajat
      rot = 3 - y + (4 * x);
      break;
  }
  return rot;
}

pthread_mutex_t hitLock, hitLock2;
int isHit(int x, int y, int rotate,  struct player *player)
{
  pthread_mutex_init(&hitLock,NULL);
  pthread_mutex_lock(&hitLock);
  int rot;
  int ans = -99999;
  for(int i =0;i<4;++i)
  {
    for(int j = 0;j<4;++j)
    {
      int rot = indexRotasi(player->rotasi,j,i);
      int tempX = player->posX + x + j;
      int tempY = player->posY + y + i;
      int idx = tempY * player->playField->width + tempX;
      if(tetromino[player->tetris][rot] !='.')
      {
        if(tempX <=0 || tempX >= pW-1 || tempY >= pH-1 || (player->playField->screen[idx] != 0 && player->playField->screen[idx] != player->curBalok+1))
          return 1;
      }
    }
  }
  pthread_mutex_unlock(&hitLock);
  return 0;
}
int isHitRotasi(int x, int y, int rotate,  struct player *player)
{
  pthread_mutex_init(&hitLock2,NULL);
  pthread_mutex_lock(&hitLock2);
  int rot;
  int ans = -99999;
  for(int i =0;i<4;++i)
  {
    for(int j = 0;j<4;++j)
    {
      int rot = indexRotasi(player->rotasi,j,i);
      int tempX = player->posX + x + j;
      int tempY = player->posY + y + i;
      int idx = tempY * player->playField->width + tempX;
      if(tetromino[player->tetris][rot] !='.')
      {
        if(tempX <=0 || tempX >= pW-1 || tempY >= pH-1 || (player->playField->screen[idx] != 0 ))
          return 1;
        // if(player->playField->screen[idx] != 0)
        //   return 1;
      }
    }
  }
  pthread_mutex_unlock(&hitLock2);
  return 0;
}
void clearTrail(int x , int y, struct player *player ){
  // printf("Clearing.....\n");
  for(int i =0;i<4;++i){
    for(int j = 0;j<4;++j){
      int tempX = x + j;
      int tempY = y + i;
      int rot = indexRotasi(player->rotasi,j,i);
      int idx = tempY * player->playField->width + tempX;
      if(x + j >= 0 && x + j < pW){
        if(y + i >= 0 && y + i < pH){
          if(player->playField->screen[idx] != 9 && player->playField->screen[idx] != 0)
          {
            if(tetromino[player->tetris][rot] !='.' && player->playField->screen[idx] == player->curBalok+1)
            {
              player->playField->screen[idx] = 0;
            }
          }
        } 
      }
    }
  }
}
void drawField(struct player *player, int coorX, int coorY,struct winScreen *screen)
{
  int i,j;
  for(i = 0;i< pH; ++i)
  {
    for(j = 0;j< pW; ++j)
    {
      if(player->playField->screen[i*pW + j] == 0)
      {
        screen->screen[getIndex(coorX + j, coorY + i, *screen)] = ' ';
      }
      else if(player->playField->screen[i*pW + j] == 9)
      {
        screen->screen[getIndex(coorX + j, coorY + i, *screen)] = '#';
      }
      else if(player->playField->screen[i*pW + j] == 8)
      {
        screen->screen[getIndex(coorX + j, coorY + i, *screen)] = '=';
      }
      else
      {
        screen->screen[getIndex(coorX + j, coorY + i, *screen)] = "ABCDEFG"[(player->playField->screen[i*pW + j] % 7)];//player.playField->screen[getIndex(j,i,*player.playField)];
      }
    }
  }
}
void drawScore(struct player player,int coorX,int coorY,struct winScreen *screen)
{
  char buffer[50];
  sprintf(buffer,"P%d Scores : %09d",player.pemain, player.score);
  for(int i = 0;i<strlen(buffer);++i){
    screen->screen[getIndex(coorX+i,coorY,*screen)] = buffer[i];
  }
}
void drawPieces(struct player *player, struct winScreen *screen){
  int i,j;
  for(i =0;i<4;++i){
    for(j = 0;j<4;++j){
      if(tetromino[player->tetris][indexRotasi(player->rotasi,j,i)] != '.')
      {
        int tempX = player->posX + j;
        int tempY = (player->posY + i)%pH;
        int idx = tempY * player->playField->width + tempX;
        player->playField->screen[idx] = player->curBalok+1;
      }
    }
  }
}

void drawHealth(struct player player,int coorX,int coorY,struct winScreen *screen)
{
  char buffer[50];
  char pla[20],heal[10];
  sprintf(pla,"Player %d",player.pemain);
  sprintf(heal,"Health");
  sprintf(buffer,"|=====%g=====|",player.health);
  for(int i = 0;i<strlen(pla);++i)
  {
    screen->screen[getIndex(coorX+i+(strlen(pla)/2),coorY-2,*screen)] = pla[i];
  }
  for(int i = 0;i<strlen(heal);++i)
  {
    screen->screen[getIndex(coorX+i+(strlen(heal)/2)+2,coorY-1,*screen)] = heal[i];
  }
  for(int i = 0;i<strlen(buffer);++i)
  {
    screen->screen[getIndex(coorX+i,coorY,*screen)] = buffer[i];
  }
}
void *DRAW(void* args)
{
  struct drawArg *arg = args;
  drawField(arg->p1, 2,3,arg->screen);
  drawField(arg->p2, 50,3,arg->screen);
  drawScore(*arg->p1,20,5,arg->screen);
  drawScore(*arg->p2,20,8,arg->screen);
  drawHealth(*arg->p1,1,2,arg->screen);
  drawHealth(*arg->p2,49,2,arg->screen);
  drawPieces(arg->p1,arg->screen);
  drawPieces(arg->p2,arg->screen);
  fprintf(stderr,"%s",arg->screen->screen);
  // fflush(stderr);
  pthread_exit(NULL);
}
pthread_mutex_t p1Lock;
pthread_mutex_t p2Lock;
void *UPDATEP1(void *args)
{
  pthread_mutex_init(&p1Lock,NULL);
  struct updateArg *arg = args;
  queue garisPenuh;
  createEmpty(&garisPenuh);
  pthread_mutex_lock(&p1Lock);
  switch(inputan){
    case KEY_W:
      arg->p1->code = inputan;
      //isHit(x,y,rotasi,player)
      int kick = 0, kick2 = 0;
      if(isHitRotasi(0,0,arg->p1->rotasi+1,arg->p1))
      {
        if(arg->p1->posX >= 0 && arg->p1->posX < pW/2)
        {
          kick = 1;
        }
        else if(arg->p1->posX >=pW/2 && arg->p1->posX <= pW-1)
        {
          kick = -1;
        }
        clearTrail(arg->p1->posX, arg->p1->posY,arg->p1);
      }
      if(isHitRotasi(kick,0,arg->p1->rotasi+1,arg->p1) == 0)
      {
        arg->p1->posX += kick;
        arg->p1->rotasi +=1;
      }
      break;
    case KEY_S:
      arg->p1->code = inputan;
      if(isHit(0,1, arg->p1->rotasi,arg->p1) == 0)
      {
        arg->p1->posY+=1;
        clearTrail(arg->p1->posX, arg->p1->posY-1,arg->p1);
      }
      // arg->p2->posY += (!isHit(arg->p2->posX, arg->p2->posY+1, arg->p2->rotasi,arg->p2))? 0 : 1;
      break;
    case KEY_A:
      arg->p1->code = inputan;
      if(isHit(-1,0, arg->p1->rotasi,arg->p1) == 0)
      {
        arg->p1->posX-=1;
        clearTrail(arg->p1->posX+1, arg->p1->posY,arg->p1);
      }
      // arg->p2->posX -= (!isHit(arg->p2->posX-1, arg->p2->posY, arg->p2->rotasi,arg->p2))? 0 : 1;
      break;
    case KEY_D:
      arg->p1->code = inputan;
      if(isHit(1, 0, arg->p1->rotasi,arg->p1) == 0)
      {
        arg->p1->posX +=1;
        clearTrail(arg->p1->posX-1, arg->p1->posY,arg->p1);
      }
      // arg->p2->posX += (!isHit(arg->p2->posX+1, arg->p2->posY, arg->p2->rotasi,arg->p2))? 0 : 1;
      break;
    default:
      if(arg->p1->speed >= 20){
        if(isHit(0, 1, arg->p1->rotasi,arg->p1) == 0)
        {
          arg->p1->speed = 0;
          arg->p1->posY++;
          clearTrail(arg->p1->posX, (arg->p1->posY-1),arg->p1);
        }
        else
        {
          for (int px = 0; px < 4; px++)
            for (int py = 0; py < 4; py++)
              if (tetromino[arg->p1->tetris][indexRotasi(arg->p1->rotasi,px, py)] != '.')
                    arg->p1->playField->screen[(arg->p1->posY + py) * pW + (arg->p1->posX + px)] = arg->p1->curBalok+1;
                    
          int penuh = 1;
          for(int i = 0;i<4;++i)
          {
            if(arg->p1->posY + i < pH - 1)
            {
              penuh = 1;
              for(int j = 1;j<pW-1;++j)
              {
                penuh &= (arg->p1->playField->screen[(arg->p1->posY+i)*pW+j] != 0);
              }
              if(penuh)
              {
                for(int j = 1;j<pW-1;++j)
                {
                  arg->p1->playField->screen[(arg->p1->posY+i)*pW+j] = 8;
                }
                enqueue(arg->p1->posY + i, &garisPenuh);
              }
            }
          }
          arg->p1->speed = 0;
          if(arg->p1->curBalok + 1 == 7)
          {
            arg->p1->curBalok +=4;
          }
          else
          {
            arg->p1->curBalok+=1;
          }
          arg->p1->score += 25;
          if(penuh)
          {
            arg->p1->score += 1 << garisPenuh.size * 100;
            arg->p2->health -= (1 << garisPenuh.size/ 100 * arg->p1->score/ 100) * garisPenuh.size;
          }
          arg->p1->posX = pW/2;
          arg->p1->posY = 0;
          arg->p1->rotasi  = 0;
          arg->p1->tetris = rand()%7;
        }
        // printf("awpodikpaowdka\n");
      }
      break;
  }
  if(garisPenuh.size){
    for(int i = 0 ; i < garisPenuh.size;++i)
    {
     int v = dequeue(&garisPenuh);
     for(int j = 1; j < pW -1 ; ++j)
     {
       for(int py = v; py > 0; py--)
       { 
        arg->p1->playField->screen[py * pW + j] = arg->p1->playField->screen[(py-1) * pW + j];
       }
       arg->p1->playField->screen[j] = 0;
     }
   } 
  }
  arg->p1->posX %= pW;
  arg->p1->posY %= pH;
  arg->p1->speed++;
  pthread_mutex_unlock(&p1Lock);
  pthread_exit(NULL);
}
void *UPDATEP2(void *args)
{
  pthread_mutex_init(&p2Lock,NULL);
  struct updateArg *arg = args;
  queue garisPenuh;
  pthread_mutex_lock(&p2Lock);
  switch(inputan){
    case KEY_I:
      arg->p2->code = inputan;
      //isHit(x,y,rotasi,player)
      int kick = 0, kick2 = 0;
      if(isHitRotasi(0,0,arg->p2->rotasi+1,arg->p2))
      {
        if(arg->p2->posX >= 0 && arg->p2->posX < pW/2)
        {
          kick = 1;
        }
        else if(arg->p2->posX >=pW/2 && arg->p2->posX <= pW-1)
        {
          kick = -1;
        }
        clearTrail(arg->p2->posX, arg->p2->posY,arg->p2);
      }
      if(isHitRotasi(kick,0,arg->p2->rotasi+1,arg->p2) == 0)
      {
        arg->p2->posX += kick;
        arg->p2->rotasi +=1;
      }
      break;
    case KEY_K:
      arg->p2->code = inputan;
      if(isHit(0,1, arg->p2->rotasi,arg->p2) == 0)
      {
        arg->p2->posY+=1;
        clearTrail(arg->p2->posX, arg->p2->posY-1,arg->p2);
      }
      // arg->p2->posY += (!isHit(arg->p2->posX, arg->p2->posY+1, arg->p2->rotasi,arg->p2))? 0 : 1;
      break;
    case KEY_J:
      arg->p2->code = inputan;
      if(isHit(-1,0, arg->p2->rotasi,arg->p2) == 0)
      {
        arg->p2->posX-=1;
        clearTrail(arg->p2->posX+1, arg->p2->posY,arg->p2);
      }
      // arg->p2->posX -= (!isHit(arg->p2->posX-1, arg->p2->posY, arg->p2->rotasi,arg->p2))? 0 : 1;
      break;
    case KEY_L:
      arg->p2->code = inputan;
      if(isHit(1, 0, arg->p2->rotasi,arg->p2) == 0)
      {
        arg->p2->posX +=1;
        clearTrail(arg->p2->posX-1, arg->p2->posY,arg->p2);
      }
      // arg->p2->posX += (!isHit(arg->p2->posX+1, arg->p2->posY, arg->p2->rotasi,arg->p2))? 0 : 1;
      break;
    default:
      if(arg->p2->speed >= 20){
        if(isHit(0, 1, arg->p2->rotasi,arg->p2) == 0)
        {
          arg->p2->speed = 0;
          arg->p2->posY++;
          clearTrail(arg->p2->posX, (arg->p2->posY-1),arg->p2);
        }
        else
        {
          // Tumpuk balok tetris
          for (int px = 0; px < 4; px++)
            for (int py = 0; py < 4; py++)
              if (tetromino[arg->p2->tetris][indexRotasi(arg->p2->rotasi,px, py)] != '.')
                    arg->p2->playField->screen[(arg->p2->posY + py) * pW + (arg->p2->posX + px)] = arg->p2->curBalok+1;
          // Check lines

          int penuh = 1;
          
          createEmpty(&garisPenuh);
          for(int i = 0;i<4;++i)
          {
            if(arg->p2->posY + i < pH - 1)
            {
              penuh = 1;
              for(int j = 1;j<pW-1;++j)
              {
                penuh &= (arg->p2->playField->screen[(arg->p2->posY+i)*pW+j] != 0);
              }
              if(penuh)
              {
                for(int j = 1;j<pW-1;++j)
                {
                  arg->p2->playField->screen[(arg->p2->posY+i)*pW+j] = 8;
                }
                enqueue(arg->p2->posY + i, &garisPenuh);
              }
            }
          }
          arg->p2->speed = 0;
          if(arg->p2->curBalok + 1 == 7)
          {
            arg->p2->curBalok +=4;
          }
          else
          {
            arg->p2->curBalok+=1;
          }
          arg->p2->score += 25;
          if(penuh)
          {
            arg->p2->score += 1 << garisPenuh.size * 100;
            arg->p1->health -= (1 << garisPenuh.size/ 100 * arg->p2->score/ 100) * 2 * garisPenuh.size;
          }
          arg->p2->posX = pW/2;
          arg->p2->posY = 0;
          arg->p2->rotasi  = 0;
          arg->p2->tetris = rand()%7;
        }
        // printf("awpodikpaowdka\n");
      }
      break;
  }
  if(garisPenuh.size){
   for(int i = 0 ; i < garisPenuh.size;++i)
   {
     int v = dequeue(&garisPenuh);
     for(int j = 1; j < pW -1 ; ++j)
     {
       for(int py = v; py > 0; py--)
       { 
        arg->p2->playField->screen[py * pW + j] = arg->p2->playField->screen[(py-1) * pW + j];
       }
       arg->p2->playField->screen[j] = 0;
     }
   } 
  }
  arg->p2->posX %= pW;
  arg->p2->posY %= pH;
  arg->p2->speed++;
  pthread_mutex_unlock(&p2Lock);
  pthread_exit(NULL);
}