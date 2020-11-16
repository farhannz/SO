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
void * GetInputFromUser(void *args)
{
  struct eventArg *evArg = args;
  // Memanggil event handler untuk Input User
  while(1)
  {
    pthread_mutex_lock(&mutex);
    EventHandler(evArg->evCodes,evArg->event);
    pthread_mutex_unlock(&mutex);
  }
}

void createScreen(int height, int width, struct winScreen *screen)
{
  screen->width = width;
  screen->height = height;
  screen->screen = (char*)malloc(screen->width * screen->height * sizeof(char));
  int i;
  for(i = 0;i<screen->width * screen->height;++i) screen->screen[i] = '-';
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
void drawField(int rows, int columns)
{
  char screen[rows*columns];
  int i , j;
  for(i = 0;i < rows;++i)
  {
    for(j = 0;j< columns;++j)
    {
      if(i == rows-1 || j == 0 || j == columns-1)
        screen[i * columns + j] = '#';
        // printf("#");
      else
        // printf(".");
        screen[i * columns + j] = '.';
    }
  }
  for(i = 0;i < rows;++i)
  {
    for(j = 0;j< columns;++j)
    {
      printf("%c",screen[i*columns + j]);
    }
    printf("\n");
  }
}
void DRAW(struct winScreen screen)
{
  // system("clear");
  
  int i ,j;
  for(i = 0; i<screen.height;++i)
  {
    for(j = 0; j<screen.width;++j)
    {
      printf("%c",screen.screen[i * screen.width + j]);
    }
    printf("\n");
  }
  fflush(stdout);
}
void UPDATE(struct inputEvent *event,struct winScreen *screen)
{
  int currentX = posX % W;
  int currentY = posY % H;
  screen->screen[currentY * W + currentX] = '-';
  if(event->code == KEY_W)
  {
    posY--;
  }
  if(event->code == KEY_A)
  {
    posX--;
  }
  if(event->code == KEY_S)
  {
    posY++;
  }
  if(event->code == KEY_D)
  {
    posX++;
  }
  if(event->code == KEY_Q)
  {
    return;
  }
  currentX = posX % W;
  currentY = posY % H;
  screen->screen[currentY * W + currentX] = '#';
  event->code = KEY_NONE;
}