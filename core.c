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
  player->playField = (struct winScreen *)malloc(sizeof(struct winScreen));
  player->playField->width = pW;
  player->playField->height = pH;
  player->pemain = pemain;
  player->rotasi = 0;
  player->curBalok = 0;
  player->tetris = 0;
  player->speed = 0;
  player->playField->screen = (char*)malloc(pW*pH*sizeof(char));
  int i,j;
  for(i = 0;i< pH; ++i)
  {
    for(j = 0;j< pW; ++j)
    {
      if(j == 0 || j == pW - 1 || i == pH-1){
        player->playField->screen[getIndex(j, i, *player->playField)] = 9;
      }
      else{
        player->playField->screen[getIndex(j, i, *player->playField)] = 0;
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

pthread_mutex_t hitLock;

int isHit(int x, int y, int rotate,  struct player *player)
{
  pthread_mutex_init(&hitLock,NULL);
  pthread_mutex_lock(&hitLock);
  int rot;
  int ans = -99999;
  for(int i =0 ;i<4;++i){
    for(int j = 0;j<4;++j){
      rot = indexRotasi(rotate,j,i);
      int fi = getIndex((x+j),(y+i),*player->playField);
      if(x + j >= 0 && x + j < pW){
        if(y + i >= 0 && y + i < pH){
          if(tetromino[player->tetris][rot] != '.' && player->playField->screen[fi] == 0)
          {
            ans = (ans > fi) ? ans : fi;
          }
        }
      }
    }
  }
  for(int i =0 ;i<4;++i){
    for(int j = 0;j<4;++j){
      rot = indexRotasi(rotate,j,i);
      int fi = getIndex((x+j),(y+i),*player->playField);
      if(x + j >= 0 && x + j < pW){
        if(y + i >= 0 && y + i < pH){
          if(fi == ans){
            return 0;
          }
        }
      }
    }
  }
  pthread_mutex_unlock(&hitLock);
  return 1;
}
void clearTrail(int x , int y, struct player *player ){
  // printf("Clearing.....\n");
  for(int i =0;i<4;++i){
    for(int j = 0;j<4;++j){
      int idx = getIndex(x + j,y + i,*player->playField);
      if(x + j >= 0 && x + j < pW){
        if(y + i >= 0 && y + i < pH){
          if(player->playField->screen[idx] != 9 && player->playField->screen[idx] != 0)
          {
            if(player->playField->screen[idx] == player->curBalok+1)
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
        screen->screen[getIndex(coorX + j, coorY + i, *screen)] = " ABCDEFG=#"[player->playField->screen[i*pW + j]];//player.playField->screen[getIndex(j,i,*player.playField)];
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
  // for(int i =0 ;i<4;++i){
  //   for(int j = 0;j<4;++j){
  //     int rot = indexRotasi(player->rotasi,j,i);
  //     int fi = getIndex((player->posX+j+1),(player->posY+i),*player->playField);
  //     if(player->posX + j >= 0 && player->posX + j < pW){
  //       if(player->posY + i >= 0 && player->posY + i < pH){
  //         if(tetromino[player->tetris][rot] != '.' && player->playField->screen[fi] == 0)
  //         {
  //           // player->playField->screen[fi] = 7;
  //         }
  //       }
  //     }
  //   }
  // }
  // for(i = 0;i<pH;++i){
  //   for(j = 0;j<pW;++j){
  //     printf("%d",player->playField->screen[getIndex(j,i,*player->playField)]);
  //   }
  //   printf("\n");
  // }
  // for(i =0;i<4;++i){
  //   for(j = 0;j<4;++j){
  //     // printf("YOW %d %d\n",i,j);
  //     if( player->playField->screen[getIndex(player->posX+j,(player->posY + i - 1)%pH, *player->playField)] != 9 &&  player->playField->screen[getIndex(player->posX+j,(player->posY + i - 1)%pH, *player->playField)] != 0)
  //     {
  //       player->playField->screen[getIndex(player->posX+j,(player->posY + i - 1)%pH, *player->playField)] = 0;
  //     }
  //   }
  // }
  // clearTrail(player->posX,player->posY-1,player);
  for(i =0;i<4;++i){
    for(j = 0;j<4;++j){
      if(tetromino[player->tetris][indexRotasi(player->rotasi,j,i)] != '.')
        player->playField->screen[getIndex(player->posX + j,(player->posY + i)%pH, *player->playField)] = player->curBalok+1;
    }
  }
}
// void drawPlayerAttributes(struct player player, struct winScreen *screen, int coorX, int coorY)
// {
//     drawField(player, coorX,coorY,screen);
// }
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
void DRAW(struct player *p1, struct player *p2,struct winScreen *screen, char**tetromino)
{
  // drawPlayerAttributes(*p1,screen,2,3);
  // drawPlayerAttributes(*p2,screen,50,3);
  drawField(p1, 2,3,screen);
  drawField(p2, 50,3,screen);
  drawScore(*p1,20,5,screen);
  drawScore(*p2,20,8,screen);
  drawHealth(*p1,1,2,screen);
  drawHealth(*p2,49,2,screen);
  drawPieces(p1,screen);
  drawPieces(p2,screen);
  fprintf(stderr,"%s",screen->screen);
  fflush(stderr);
}
pthread_mutex_t p1Lock;
pthread_mutex_t p2Lock;
void *UPDATEP1(void *args)
{
  pthread_mutex_init(&p1Lock,NULL);
  struct updateArg *arg = args;
  pthread_mutex_lock(&p1Lock);
  switch(inputan){
      case KEY_W:
        arg->p1->code = inputan;
        if(isHit(arg->p1->posX, arg->p1->posY, arg->p1->rotasi+1,arg->p1) == 0)
        {
          arg->p1->rotasi+=1;
          clearTrail(arg->p1->posX, arg->p1->posY,arg->p1);
        }
        // arg->p2->rotasi += (!isHit(arg->p2->posX, arg->p2->posY, arg->p2->rotasi+1,arg->p2))? 0 : 1;
        break;
      case KEY_S:
        arg->p1->code = inputan;
        if(isHit(arg->p1->posX, arg->p1->posY+1, arg->p1->rotasi,arg->p1) == 0)
        {
          arg->p2->posY+=1;
          clearTrail(arg->p1->posX, arg->p1->posY-1,arg->p1);
        }
        // arg->p2->posY += (!isHit(arg->p2->posX, arg->p2->posY+1, arg->p2->rotasi,arg->p2))? 0 : 1;
        break;
      case KEY_A:
        arg->p1->code = inputan;
        if(isHit(arg->p1->posX-1, arg->p1->posY, arg->p1->rotasi,arg->p1) == 0)
        {
          arg->p1->posX-=1;
          clearTrail(arg->p1->posX+1, arg->p1->posY,arg->p1);
        }
        // arg->p2->posX -= (!isHit(arg->p2->posX-1, arg->p2->posY, arg->p2->rotasi,arg->p2))? 0 : 1;
        break;
      case KEY_D:
        arg->p1->code = inputan;
        if(isHit(arg->p1->posX+1, arg->p1->posY, arg->p1->rotasi,arg->p1) == 0)
        {
          arg->p1->posX +=1;
          clearTrail(arg->p1->posX-1, arg->p1->posY,arg->p1);
        }
        // arg->p2->posX += (!isHit(arg->p2->posX+1, arg->p2->posY, arg->p2->rotasi,arg->p2))? 0 : 1;
        break;
      default:
        if(arg->p1->speed >= 20){
          if(isHit(arg->p1->posX, arg->p1->posY + 1, arg->p1->rotasi,arg->p1) == 0)
          {
            arg->p1->speed = 0;
            arg->p1->posY++;
            clearTrail(arg->p1->posX, (arg->p1->posY-1),arg->p1);
          }
          else
          {
            for (int px = 0; px < 4; px++)
              for (int py = 0; py < 4; py++)
                if (tetromino[arg->p1->tetris][indexRotasi(arg->p1->rotasi,px, py)] != L'.')
                      arg->p1->playField->screen[(arg->p1->posY + py) * pW + (arg->p1->posX + px)] = arg->p1->curBalok+1;
            arg->p1->curBalok+=1;
            arg->p1->curBalok%=7;
            arg->p1->score += 25;
            arg->p1->posX = pW/2;
            arg->p1->posY = 0;
            arg->p1->rotasi  = 0;
            arg->p1->tetris = rand()%7;
          }
          // printf("awpodikpaowdka\n");
        }
        break;
    }
    
    arg->p1->posX %= pW;
    arg->p1->posY %= pH;
    arg->p1->speed++;
  // int px, py;
  // printf("%d\n",arg->p1->posY);
  // // printf("%d\n",isHit(*arg->p1));
  // for(py = 0;py < 4;++py)
  // {
  //     for(px = 0;px < 4;++px)
  //     {
  //         //getIndex(x,y);
  //         arg->p1->playField->screen[getIndex((px  + arg->p1->posX)%arg->p1->playField->width, (py+arg->p1->posY-1)%(arg->p1->playField->height-1),*(arg->p1->playField))] = '.';
  //         // screen.screen[getIndex(px  + p2.posX, (py+yTest)%(screen.height+5),screen)] = '.';
  //         // screen.screen[((py+yTest+2)%screen.height) * screen.width + (px + W/2)] = '.';
  //     }
  // }
  // if(arg->p1->speed>=20){
  //   arg->p1->speed = 0;
  //   arg->p1->posY = (arg->p1->posY + 1)% pH;
  // }
  // if(arg->p1->posY % (arg->p1->playField->height-2)==0 && arg->p1->posY != 0){
  //   arg->p1->tetris = rand()%7;
  // }
  // for(py = 0;py < 4;++py)
  // {
  //     for(px = 0;px < 4;++px)
  //     {
  //         if(tetromino[arg->p1->tetris][indexRotasi(arg->p1->rotasi,px,py)] != '.')
  //         {
  //             arg->p1->playField->screen[getIndex((px + arg->p1->posX)%arg->p1->playField->width, (py+arg->p1->posY)%(arg->p1->playField->height-1),*(arg->p1->playField))] = 'T';
  //             // screen.screen[((py+yTest+2)%screen.height) * screen.width + (px + W/2)] = 'T';
  //         }
  //     }
  // }
  // arg->p1->speed++;
  pthread_mutex_unlock(&p1Lock);
  pthread_exit(0);
}
void *UPDATEP2(void *args)
{
  struct updateArg *arg = args;
  pthread_mutex_init(&p2Lock,NULL);
  pthread_mutex_lock(&p2Lock);
  switch(inputan){
    case KEY_I:
      arg->p2->code = inputan;
      if(isHit(arg->p2->posX, arg->p2->posY, arg->p2->rotasi+1,arg->p2) == 0)
      {
        arg->p2->rotasi+=1;
        clearTrail(arg->p2->posX, arg->p2->posY,arg->p2);
      }
      // arg->p2->rotasi += (!isHit(arg->p2->posX, arg->p2->posY, arg->p2->rotasi+1,arg->p2))? 0 : 1;
      break;
    case KEY_K:
      arg->p2->code = inputan;
      if(isHit(arg->p2->posX, arg->p2->posY+1, arg->p2->rotasi,arg->p2) == 0)
      {
        arg->p2->posY+=1;
        clearTrail(arg->p2->posX, arg->p2->posY-1,arg->p2);
      }
      // arg->p2->posY += (!isHit(arg->p2->posX, arg->p2->posY+1, arg->p2->rotasi,arg->p2))? 0 : 1;
      break;
    case KEY_J:
      arg->p2->code = inputan;
      if(isHit(arg->p2->posX-1, arg->p2->posY, arg->p2->rotasi,arg->p2) == 0)
      {
        arg->p2->posX-=1;
        clearTrail(arg->p2->posX+1, arg->p2->posY,arg->p2);
      }
      // arg->p2->posX -= (!isHit(arg->p2->posX-1, arg->p2->posY, arg->p2->rotasi,arg->p2))? 0 : 1;
      break;
    case KEY_L:
      arg->p2->code = inputan;
      if(isHit(arg->p2->posX+1, arg->p2->posY, arg->p2->rotasi,arg->p2) == 0)
      {
        arg->p2->posX +=1;
        clearTrail(arg->p2->posX-1, arg->p2->posY,arg->p2);
      }
      // arg->p2->posX += (!isHit(arg->p2->posX+1, arg->p2->posY, arg->p2->rotasi,arg->p2))? 0 : 1;
      break;
    default:
      if(arg->p2->speed >= 20 && isHit(arg->p2->posX, arg->p2->posY + 1, arg->p2->rotasi,arg->p2) == 0){
        arg->p2->speed = 0;
        arg->p2->posY++;
        clearTrail(arg->p2->posX, (arg->p2->posY-1),arg->p2);
        // printf("awpodikpaowdka\n");
      }
      break;
  }
  
  arg->p2->posX %= pW;
  arg->p2->posY %= pH;
  arg->p2->speed++;
  pthread_mutex_unlock(&p2Lock);
  pthread_exit(0);
}

void* UPDATE(void *args)
{
  struct updateArg *arg = args;
  int p1X = arg->p1->posX % W;
  int p1Y = arg->p1->posY % H;
  if(p1X == arg->screen->width-1)
    arg->screen->screen[p1Y * W + p1X] = '\n';
  else
    arg->screen->screen[p1Y * W + p1X] = '.';
  int p2X = arg->p2->posX % W;
  int p2Y = arg->p2->posY % H;
  if(p2X == arg->screen->width-1)
    arg->screen->screen[p2Y * W + p2X] = '\n';
  else
    arg->screen->screen[p2Y * W + p2X] = '.';
  char key;
  if(!isEmpty(keyBuffer))
  {
    key = dequeue(&keyBuffer);
    switch(key)
    {
      case KEY_I:
        arg->p2->code = key;
        break;
      case KEY_K:
        arg->p2->code = key;
        break;
      case KEY_J:
        arg->p2->code = key;
        break;
      case KEY_L:
        arg->p2->code = key;
        break;
      default:
        arg->p1->code = key;
        break;
    }
  }
  else
  {
    key = (char)KEY_NONE;
  }
  if(key == KEY_W)
  {
    arg->p1->posY--;
  }
  if(key == KEY_A) 
  {
    arg->p1->posX--;
  }
  if(key == KEY_S)
  {
    arg->p1->posY++;
  }
  if(key == KEY_D)
  {
    arg->p1->posX++;
  }
  if(key == KEY_I)
  {
    arg->p2->posY--;
  }
  if(key == KEY_J) 
  {
    arg->p2->posX--;
  }
  if(key == KEY_K)
  {
    arg->p2->posY++;
  }
  if(key == KEY_L)
  {
    arg->p2->posX++;
  }

  if(arg->event->code == KEY_Q)
  {
    return NULL;
  }
  p1X = arg->p1->posX % W;
  p1Y = arg->p1->posY % H;
  if(p1X == arg->screen->width-1)
    arg->screen->screen[p1Y * W + p1X] = '\n';
  // else
  //   arg->screen->screen[p1Y * W + p1X] = '#';
  p2X = arg->p2->posX % W;
  p2Y = arg->p2->posY % H;
  if(p1X == arg->screen->width-1)
    arg->screen->screen[p2Y * W + p2X] = '\n';
  // else
  //   arg->screen->screen[p2Y * W + p2X] = '#';
  arg->event->code = KEY_NONE;
  // p2.code = KEY_NONE;
}
