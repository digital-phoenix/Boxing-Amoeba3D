#ifndef GRAPHIC_STATE_H_
#define GRAPHIC_STATE_H_

#include<iostream>
#include<windows.h>
#include<gl/gl.h>     
#include<gl/glut.h>   

extern int screenLeft;
extern  int screenRight;
extern  int screenTop;
extern  int screenBottom;

typedef struct{
 double x,y;
}point;
	
typedef struct{
	point points[3];
}triangle;	

#endif