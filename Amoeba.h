#ifndef AMOEBA_H_
#define AMOEBA_H_

#include"Sprite.h"
#include"Metaball3DGroup.h"
#include"Obstacle.h"
#include <string>
#include <time.h>

class Amoeba : public Sprite  {
	protected:

		/*Characteristics*/
		Metaball3DGroup balls;
		bool needToResize;
		bool isHit;
		time_t isHitTimer;
		bool morphBall;
		clock_t morphBallTimer;

		double velX, velY, velZ;
		double px, py,pz;
		double radius;
		double scale;
		bool normal;
		Amoeba *player;


		/*Arm management variables*/
		Metaball3DGroup *attackArm;
		bool attackActive;
		time_t attackArmTimer;

		int attackSpacing1;
		int attackSpacing2;
		int attackSpacing3;

	    int leftMx, leftMy;
		float lslope;

		Metaball3DGroup *defendArm;

		bool defendActive;
		bool defendSegActive;
		bool defendSeg2Active;
		time_t defendArmTimer;
		time_t defendWaitTimer;

		int defendSpacing1;
		int defendSpacing2;
		int defendSpacing3;

		int rightMx, rightMy;
		float rslope;

		double attackFistPx;
		double attackFistPy;
		double attackFistPz;
		double attackFistRadius;

		double defendFistPx;
		double defendFistPy;
		double defendFistPz;
		double defendFistRadius;


		/*Collision gathering variables*/
		bool isCollision;
		bool isBody;
		bool isDefend;
		bool isAttack;
		bool isWall;

		double colPx;
		double colPy;
		double colPz;
		double colAngle;

		bool canMoveUp;
		bool canMoveDown;
		bool canMoveLeft;
		bool canMoveRight;
		

	public:
		Amoeba();
		Amoeba(double,double,double, double, double, bool);
		
		int getIdentifier(){
			return AMOEBA_TYPE;
		}

		int draw(double vertices[25000][4], double normals[25000][4]){
			return balls.draw(vertices, normals);
		};

		bool getResize()
		{
			return needToResize;
		}

		double getScale()
		{
			return scale;
		}

		bool AmoebaCollision( Amoeba* other);

		void getAvailableMoves( bool moves[4])
		{
			moves[0] = canMoveUp;
			moves[1] = canMoveDown;
			moves[2] = canMoveLeft;
			moves[3] = canMoveRight;
		}

		void getAttackData(double attack[4])
		{
			attack[0] = attackFistPx;
			attack[1] = attackFistPy;
			attack[2] = attackFistPz;
			attack[3] = attackFistRadius;
		}

		void getDefendData(double defend[4])
		{
			defend[0] =  defendFistPx;
			defend[1] = defendFistPy;
			defend[2] = defendFistPz;
			defend[3] = defendFistRadius;
		}

		void update();


		void collision(Sprite *obj);

		void setVelx( double x){
			velX = x;
		}

		void setVely( double y){
			velY = y;
		}

		void setVelz( double z){
			velZ = z;
		}
		void setVelocity( double x, double y, double z){
			setVelx(x);
			setVely(y);
			setVelz(z);
		}

		double getRadius()
		{
			return radius;
		}

		double getPx()
		{
			return px;
		}

		double getPy()
		{
			return py;
		}

		double getPz()
		{
			return pz;
		}

		
		/*void setRightMousePos(GLsizei x, GLsizei y)
		{
			if(!defendActive)
			{
				rightMx = x;
				rightMy = y;
			}
		}

		void setLeftMousePos(GLsizei x, GLsizei y)
		{
			if(!attackActive)
			{
				leftMx = x;
				leftMy = y;
			}
		}

		void incAngle()
		{
			//radAngle+=20;
		}

		void decAngle()
		{
			//radAngle-=20;
		}
		*/
		/*
		void morph()
		{
			if(!morphBall)
			{
				
				morphBallTimer = clock();

				if(velX != 0 || velY != 0)
				{
					morphBall = true;
					double morphRadius = radius/5;

					if(velX > 0 && velY > 0)//Up-Right
					{
						balls.addMetaball(new Metaball2D(px + radius + velX, py + radius + velY, morphRadius));
					}
					else if(velX > 0 && velY < 0)//Down-Right
					{
						balls.addMetaball(new Metaball2D(px + radius + velX, py - radius + velY, morphRadius));
					}
					else if(velX < 0 && velY > 0)//Up-Left
					{
						balls.addMetaball(new Metaball2D(px - radius + velX, py + radius + velY, morphRadius));
					}
					else if(velX < 0 && velY < 0)//Down-Left
					{
						balls.addMetaball(new Metaball2D(px - radius + velX, py - radius + velY, morphRadius));
					}
					else if(velX == 0 && velY > 0)//Up
					{
						balls.addMetaball(new Metaball2D(px, py + radius + velY, morphRadius));
					}
					else if(velX == 0 && velY < 0)//Bottom
					{
						balls.addMetaball(new Metaball2D(px, py - radius + velY, morphRadius));
					}
					else if(velX > 0 && velY == 0)//Right
					{
						balls.addMetaball(new Metaball2D(px + radius + velX, py, morphRadius));
					}
					else//Left
					{
						balls.addMetaball(new Metaball2D(px - radius + velX, py, morphRadius));
					}
				}
				
			}
			
			if(morphBall && clock() - morphBallTimer > 400)
			{
				morphBallTimer = 0;
				morphBall = false;
				balls.popMetaball();
			}

		}
		*/

		void extendAttackArm();

		void extendDefendArm();		

		void retractDefendArm();
		void retractAttackArm();

		void retractArm();
		void setupTarget(Amoeba*);
};
#endif