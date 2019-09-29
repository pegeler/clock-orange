// A Clockwork Orange
// Paul Egeler
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

struct Point
{
  int x, y;
};

struct Colors
{
  int bg, fg, ac;
};

void draw_pixel(int color, struct Point p)
{
  if ( p.x >= 0 && p.x < img_width && p.y >= 0 && p.y < img_height )
    img[img_width * p.y + p.x] = color;
}

void draw_line(int color, struct Point p1, struct Point p2)
{
  int direction, slope, x_len, y_len, b;

  x_len = abs(p2.x - p1.x);
  y_len = abs(p2.y - p1.y);

  if (p1.x == p2.x) {                                    // Vertical line
    if (p1.y == p2.y) { draw_pixel(color, p1); return; } // Single pixel

    direction = p2.y > p1.y ? 1 : -1;
    for (int a=0; a <= y_len; a++)
    {
      draw_pixel(color, p1);
      p1.y += direction;
    }

  } else if (p1.y == p2.y) {                             // Horizontal line
    direction = p2.x > p1.x ? 1 : -1;

    for (int a=0; a <= x_len; a++)
    {
      draw_pixel(color, p1);
      p1.x += direction;
    }

  } else if (x_len >= y_len) {                           // Diagonal, x longer
    direction = p2.x > p1.x ? 1 : -1;
    slope = ((p2.y - p1.y) << 16) / x_len;
    b = p1.y << 16;

    for (int a=0; a <= x_len; a++)
    {
      p1.y = b >> 16;
      draw_pixel(color, p1);
      b += slope;
      p1.x += direction;
    }

  } else {                                               // Diagonal, y longer
    direction = p2.y > p1.y ? 1 : -1;
    slope = ((p2.x - p1.x) << 16) / y_len;
    b = p1.x << 16;

    for (int a=0; a <= y_len; a++)
    {
      p1.x = b >> 16;
      draw_pixel(color, p1);
      b += slope;
      p1.y += direction;
    }
  }

}

void clear_screen(int color)
{
  for(int i=0; i < img_height*img_width; i++)
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
  struct Point
    sec    = {           0,            0 },
    min    = {           0,            0 },
    hour   = {           0,            0 };
  const struct Point
    origin = { img_width/2, img_height/2 };
  struct Colors
    colors = {      ORANGE,        BLACK,        WHITE };

  // Initialize SDL Video
  if (SDL_Init(SDL_INIT_VIDEO)==-1) exit(1);
  atexit(SDL_Quit);
  surface = SDL_SetVideoMode(img_width,
                             img_height,
                             32,
                             SDL_SWSURFACE);
  SDL_WM_SetCaption("A Clockwork Orange", NULL);
  if (surface == NULL) exit(1);
  img = (unsigned int *) surface->pixels;

  // Loop to refresh time and draw hands
  int redraw = 1;
  while (1) {

    // Housekeeping for exit criteria
    while (SDL_PollEvent(&event))
    {
      if (event.type==SDL_QUIT) exit(0);

      if (event.type==SDL_KEYDOWN)
        switch (event.key.keysym.sym)
        {
          // Exit criteria
          case SDLK_SPACE:
            exit(0);
            break;

          case SDLK_ESCAPE:
            exit(0);
            break;

          // All permutations of colors
          // https://github.com/pegeler/heaps
          case SDLK_1:
            colors.bg = ORANGE;
            colors.fg = BLACK;
            colors.ac = WHITE;
            redraw = 1;
            break;

          case SDLK_2:
            colors.bg = BLACK;
            colors.fg = ORANGE;
            colors.ac = WHITE;
            redraw = 1;
            break;

          case SDLK_3:
            colors.bg = WHITE;
            colors.fg = ORANGE;
            colors.ac = BLACK;
            redraw = 1;
            break;

          case SDLK_4:
            colors.bg = ORANGE;
            colors.fg = WHITE;
            colors.ac = BLACK;
            redraw = 1;
            break;

          case SDLK_5:
            colors.bg = BLACK;
            colors.fg = WHITE;
            colors.ac = ORANGE;
            redraw = 1;
            break;

          case SDLK_6:
            colors.bg = WHITE;
            colors.fg = BLACK;
            colors.ac = ORANGE;
            redraw = 1;
            break;
        }
    }

    if (redraw) {

      // Set background color
      clear_screen(colors.bg);

      // Draw outline of circle
      #define N_POINTS 2160
      for (int i=0; i <= N_POINTS; i++)
      {
        struct Point p;

        p.x = cos(i*2*M_PI/N_POINTS) * img_width  * radius + origin.x;
        p.y = sin(i*2*M_PI/N_POINTS) * img_height * radius + origin.y;

        draw_pixel(colors.fg, p);
      }

      // Draw hash marks
      for (int i=0; i < 60; i++)
      {
        double theta, tick;
        tick = i % 5 ? 0.43 : 0.41;
        theta = i*2*M_PI/60;

        struct Point outer, inner;
        inner.x = cos(theta) * img_width  * tick   + origin.x;
        inner.y = sin(theta) * img_height * tick   + origin.y;
        outer.x = cos(theta) * img_width  * radius + origin.x;
        outer.y = sin(theta) * img_height * radius + origin.y;

        draw_line(colors.fg, inner, outer);
      }

      redraw = 0;

    } else {

      // Just draw over current hands with bg color
      draw_line(colors.bg, origin, sec);
      draw_line(colors.bg, origin, min);
      draw_line(colors.bg, origin, hour);

    }

    // Get time
    time(&t);
    tm = localtime(&t);

    // Ensures no more than 30 FPS
    if (tm->tm_sec == current_sec) {
      usleep(3e5);
      continue;
    } else {
      current_sec = tm->tm_sec;
    }

    // Draw hands
    sec.x = cos(  2*M_PI*tm->tm_sec / 60 - M_PI / 2  ) *
            img_width * 0.4 + origin.x;
    sec.y = sin(  2*M_PI*tm->tm_sec / 60 - M_PI / 2  ) *
            img_height * 0.4 + origin.y;
    draw_line(colors.ac, origin, sec);

    min.x = cos(  2*M_PI*(tm->tm_min
                          + (double) tm->tm_sec / 60)
                  / 60 - M_PI / 2  ) *
            img_width * 0.4 + origin.x;
    min.y = sin(  2*M_PI*(tm->tm_min
                          + (double) tm->tm_sec / 60)
                  / 60 - M_PI / 2  ) *
            img_height * 0.4 + origin.y;
    draw_line(colors.fg, origin, min);

    hour.x = cos(  2*M_PI*((tm->tm_hour % 12)
                            + (double) tm->tm_min / 60
                            + (double) tm->tm_sec / 3600)
                   / 12 - M_PI / 2  ) *
             img_width * 0.25 + origin.x;
    hour.y = sin(  2*M_PI*((tm->tm_hour % 12)
                            + (double) tm->tm_min / 60
                            + (double) tm->tm_sec / 3600)
                   / 12 - M_PI / 2  ) *
             img_height * 0.25 + origin.y;
    draw_line(colors.fg, origin, hour);

    SDL_Flip(surface);

  }

  return 0;

}
