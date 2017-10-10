/*
A Clockwork Orange

31 Jan 2012

To compile:
   gcc ./[name].cpp -lSDL -lm -o [name].o
*/

#include <time.h>
#include <SDL/SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>


//Declare image array.
unsigned int *img,imgwidth=600,imgheight=600;

void putpixel(int c, int x, int y)
{
 //Next line is for clip check.
 if(x >= 0 && x < imgwidth && y >= 0 && y < imgheight) img[imgwidth*y+x] = c;
}

void line(int color,int x,int y,int x2,int y2) {
 int a,b,step,xlen,ylen,inc;
 if(x==x2 && y==y2) {putpixel(x,y,color);return;}
 xlen=x2-x; if(xlen < 0) xlen*=-1;
 ylen=y2-y; if(ylen < 0) ylen*=-1;
 if(xlen >= ylen) {
     inc=x2<x ? -1 : 1;
     step=((y2-y) << 16) / xlen;
     b=y << 16; //b fills in for y
      for(a=0;a<=xlen;a++) {
        putpixel(color,x,b >> 16);
        b+=step;
        x+=inc;
        }
      }    
    else {
      inc=y2<y ? -1 : 1;
      step=((x2-x) << 16) / ylen;
      b=x << 16;  //b fills in for x
      for(a=0;a<=ylen;a++) {
        putpixel(color,b >> 16,y);
        b+=step;
        y+=inc;
        }
      }    
}  
int clearscreen(int color)
{
int counter=0;
while(counter<=imgheight*imgwidth){
	img[counter]=color;
	counter++;
	}
}

int main(void)
{
 SDL_Surface *surface;
 SDL_Event event;
 time_t utime;
 int running=1;//sec=0,min=0,hour=0;
 int color=0x000000,accent=0xffffff,background=0xff8800;
 const double pi=3.141592653;
 double sec=0,min=0,hour=0,
 secx=0, secy=0, 
 minx=0, miny=0,
 hourx=0, houry=0,
 x=0,y=0,
 x1=0,y1=0,counter=0;
 
 //Introduction
 printf("A Clockwork Orange.\r\n");
 printf("31 Jan 2012\r\n");
 
 //sleep(1);
 
 //Check to see if SDL vid will initialize.
 if(SDL_Init(SDL_INIT_VIDEO)==-1) {
	printf("SDL failed to initialize\r\n");
	exit(1);
	}
 atexit(SDL_Quit);
 surface=SDL_SetVideoMode(imgwidth,imgheight,32,SDL_SWSURFACE);
 if(surface==NULL) exit(1);
 img=(unsigned int *) surface->pixels;

//Set background color
clearscreen(background);

//Draw outline of circle 
 while(counter<=2*pi && running) {
	x = cos(counter) * (imgwidth * 0.45) + imgwidth/2;
	y = sin(counter) * (imgheight * 0.45) + imgheight/2;
	putpixel(color,x,y);
	counter += 0.0005;
	if(SDL_PollEvent(&event)) if(event.type==SDL_KEYDOWN) running=0;
}

counter=0;

//Draw major hash marks
 while(counter<2*pi && running) {
	x = cos(counter) * (imgwidth * 0.41) + imgwidth/2;
	y = sin(counter) * (imgheight * 0.41) + imgheight/2;
	x1 = cos(counter) * (imgwidth * 0.45) + imgwidth/2;
	y1 = sin(counter) * (imgheight * 0.45) + imgheight/2;
	line(color,x,y,x1,y1);
	counter += 2*pi/12;
	if(SDL_PollEvent(&event)) if(event.type==SDL_KEYDOWN) running=0;
  }

counter=0;

//Draw minor hash marks
 while(counter<2*pi && running) {
	x = cos(counter) * (imgwidth * 0.43) + imgwidth/2;
	y = sin(counter) * (imgheight * 0.43) + imgheight/2;
	x1 = cos(counter) * (imgwidth * 0.45) + imgwidth/2;
	y1 = sin(counter) * (imgheight * 0.45) + imgheight/2;
	line(color,x,y,x1,y1);
	counter += 2*pi/60;
	if(SDL_PollEvent(&event)) if(event.type==SDL_KEYDOWN) running=0;
  }

counter=0;

//Draw hands.
 while(running && counter<=9000) {
   
   //Clear previously drawn lines.
	line(background,imgwidth/2,imgheight/2,secx,secy);
	line(background,imgwidth/2,imgheight/2,minx,miny);
	line(background,imgwidth/2,imgheight/2,hourx,houry);
  
	//Get Unix Time
	utime = time(NULL);
	//utime = 162090; //45h, 1m, 30s 9:01:30
	//utime=7290; // 2h, 1m, 30s
	utime -= 3600*5; //Convert from GMT to ET
	//Modulo each value based on number of seconds per rotation
	sec = utime%(60); 
	min = utime%(3600);
	hour = utime%(12*3600);
   
	//Draw lines
		//Value is divided by number of radians for a complete turn
		//then is offset by 90 degrees counterclockwise
	secx = cos((sec/(60/(2*pi)))-pi/2) * (imgwidth * 0.4) + imgwidth/2;
	secy = sin((sec/(60/(2*pi)))-pi/2) * (imgheight * 0.4) + imgheight/2;
	line(accent,imgwidth/2,imgheight/2,secx,secy);
   
	minx = cos((min/(3600/(2*pi))-pi/2)) * (imgwidth * 0.4) + imgwidth/2;
	miny = sin((min/(3600/(2*pi))-pi/2)) * (imgheight * 0.4) + imgheight/2;
	line(color,imgwidth/2,imgheight/2,minx,miny);
   
	hourx = cos((hour/(12*3600/(2*pi))-pi/2)) * (imgwidth * 0.25) + imgwidth/2;
	houry = sin((hour/(12*3600/(2*pi))-pi/2)) * (imgheight * 0.25) + imgheight/2;
	line(color,imgwidth/2,imgheight/2,hourx,houry);
   
	//Ensures no more than 30 FPS
	usleep(32000);  
   
   //fun with colors:
   //accent +=0x01090f;
    

	SDL_Flip(surface);
	
	//Housekeeping for exit criteria
	if(SDL_PollEvent(&event)) if(event.type==SDL_KEYDOWN) running=0;
	counter++;
 }
 return 0;
}