#include "core.h"

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
    EventHandler(evArg->evCodes,evArg->event);
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
      event->val  = 0;
      int i ;
    }
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
void DRAW()
{
  // system("clear");
  drawField(12,8);
}
void UPDATE()
{
  printf("2+2\n");
}