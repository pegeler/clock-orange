// A Clockwork Orange
// 31 Jan 2012
// revised 14 Sep 2019

#include <time.h>
#include <SDL/SDL.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>

// Define colors
#define BLACK 0x000000
#define WHITE 0xffffff
#define ORANGE 0xff8800

// Declare image array.
unsigned int *img;
const unsigned int img_width=600, img_height=600;

void draw_pixel(int color,
                int x,
                int y)
{
  if(x >= 0 && x < img_width && y >= 0 && y < img_height)
    img[img_width*y+x] = color;
}

void draw_line(int color,
               int x1,
               int y1,
               int x2,
               int y2)
{
  int direction, slope, x_len, y_len, b;

  if(x1 == x2 && y1 == y2) {
    draw_pixel(x1, y1, color);
    return;
  }

  x_len = abs(x2 - x1);
  y_len = abs(y2 - y1);

  if(x_len >= y_len) {
    direction = x2 > x1 ? 1 : -1;
    slope = ((y2 - y1) << 16) / x_len;
    b = y1 << 16;

    for(int a=0; a <= x_len; a++)
    {
      draw_pixel(color, x1, b >> 16);
      b  += slope;
      x1 += direction;
    }
  } else {
    direction = y2 > y1 ? 1 : -1;
    slope = ((x2 - x1) << 16) / y_len;
    b = x1 << 16;

    for(int a=0; a <= y_len; a++)
    {
      draw_pixel(color, b >> 16, y1);
      b  += slope;
      y1 += direction;
    }
  }

}

void clear_screen(int color)
{
  for(int i=0; i <= img_height*img_width; i++)
    img[i]=color;
}

int main(void)
{
  SDL_Surface *surface;
  SDL_Event event;
  time_t t;
  struct tm *tm;
  int current_sec=0;
  const double radius=0.45;
  double
    sec_x=0, sec_y=0,
    min_x=0, min_y=0,
    hour_x=0, hour_y=0,
    x0=0,y0=0,
    x1=0,y1=0;

  // Initialize SDL Video
  if(SDL_Init(SDL_INIT_VIDEO)==-1) exit(1);
  atexit(SDL_Quit);
  surface = SDL_SetVideoMode(img_width,
                             img_height,
                             32,
                             SDL_SWSURFACE|SDL_NOFRAME);
  if(surface == NULL) exit(1);
  img = (unsigned int *) surface->pixels;

  // Set background color
  clear_screen(ORANGE);

  // Draw outline of circle
  #define N_POINTS 2160
  for (int i=0; i <= N_POINTS; i++)
  {
    /* This leaves bare spots.
    for (int i=-270; i <= 270; i++)
    x0 = i + img_width / 2;
    y0 = sqrt(pow(270,2) - pow(i, 2)) + img_height / 2;
    draw_pixel(BLACK, x0, y0);
    */

    draw_pixel(BLACK,
               cos(i*2*M_PI/N_POINTS) * img_width * radius + img_width / 2,
               sin(i*2*M_PI/N_POINTS) * img_height * radius + img_height / 2);


  }

  // Draw hash marks
  for (int i=0; i < 60; i++)
  {
    double theta, tick;
    tick = i % 5 ? 0.43 : 0.41;
    theta = i*2*M_PI/60;

    x0 = cos(theta) * img_width  * tick + img_width    / 2;
    y0 = sin(theta) * img_height * tick + img_height   / 2;
    x1 = cos(theta) * img_width  * radius + img_width  / 2;
    y1 = sin(theta) * img_height * radius + img_height / 2;

    draw_line(BLACK,x0,y0,x1,y1);
  }

  // Loop to refresh time and draw hands
  while (1) {

    // Housekeeping for exit criteria
    if(SDL_PollEvent(&event) && event.type==SDL_KEYDOWN)
      break;

    // Get time
    time(&t);
    tm = localtime(&t);

    // Ensures no more than 30 FPS
    if (tm->tm_sec == current_sec) {
      usleep(32000);
    continue;
    } else {
      current_sec = tm->tm_sec;
    }

    // Clear previously drawn hands
    draw_line(ORANGE,img_width/2,img_height/2,sec_x,sec_y);
    draw_line(ORANGE,img_width/2,img_height/2,min_x,min_y);
    draw_line(ORANGE,img_width/2,img_height/2,hour_x,hour_y);

    // Draw hands
    sec_x = cos(  2*M_PI*tm->tm_sec / 60 - M_PI / 2  ) *
            img_width * 0.4 + img_width / 2;
    sec_y = sin(  2*M_PI*tm->tm_sec / 60 - M_PI / 2  ) *
            img_height * 0.4 + img_height / 2;
    draw_line(WHITE,img_width/2,img_height/2,sec_x,sec_y);

    min_x = cos(  2*M_PI*(tm->tm_min
                          + (double) tm->tm_sec / 60)
                  / 60 - M_PI / 2  ) *
            img_width * 0.4 + img_width / 2;
    min_y = sin(  2*M_PI*(tm->tm_min
                          + (double) tm->tm_sec / 60)
                  / 60 - M_PI / 2  ) *
            img_height * 0.4 + img_height / 2;
    draw_line(BLACK,img_width/2,img_height/2,min_x,min_y);

    hour_x = cos(  2*M_PI*((tm->tm_hour % 12)
                            + (double) tm->tm_min / 60
                            + (double) tm->tm_sec / 3600)
                   / 12 - M_PI / 2  ) *
             img_width * 0.25 + img_width / 2;
    hour_y = sin(  2*M_PI*((tm->tm_hour % 12)
                            + (double) tm->tm_min / 60
                            + (double) tm->tm_sec / 3600)
                   / 12 - M_PI / 2  ) *
             img_height * 0.25 + img_height / 2;
    draw_line(BLACK,img_width/2,img_height/2,hour_x,hour_y);

    SDL_Flip(surface);

  }

 return 0;

}
