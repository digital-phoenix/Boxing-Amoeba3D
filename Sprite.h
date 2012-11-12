#ifndef SPRITE_H_
#define SPRITE_H_

#include<iostream>
#include <string>
#include<windows.h>
#include<gl/gl.h>     
#include<gl/glut.h> 

#define AMOEBA_TYPE 1
#define AI_TYPE 2
#define OBSTACLE_TYPE 3

class Sprite
{
	public:
		
		virtual void draw() = 0;
		virtual void collision(Sprite*) = 0;
		virtual void update() = 0;
		virtual int getIdentifier() = 0;		
		
};

#endif