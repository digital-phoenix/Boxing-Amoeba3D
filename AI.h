#include"Sprite.h"
#include"Amoeba.h"
#include"Metaball2DGroup.h"
#include <time.h>
#include <string>

class AI : Amoeba
{
	/*
		Metaball2DGroup balls;
		std::string identifier;
		double velX, velY;
		double px, py;
		int radAngle;
		double radius;
		bool normal;


		
		Metaball2DGroup *attackArm;
		bool attackActive;
		time_t attackArmTimer;

		int attackSpacing1;
		int attackSpacing2;
		int attackSpacing3;

		float lslope;

		Metaball2DGroup *defendArm;

		bool defendActive;
		bool defendSegActive;
		bool defendSeg2Active;
		time_t defendArmTimer;

		int defendSpacing1;
		int defendSpacing2;
		int defendSpacing3;

		float rslope;

		double attackFistPx;
		double attackFistPy;
		double attackFistRadius;

		double defendFistPx;
		double defendFistPy;
		double defendFistRadius;

		bool isCollision;
		bool isBody;
		bool isDefend; 
		bool isAttack;
		bool isWall;

		double colPx;
		double colPy;
		double colAngle;
*/
		Amoeba *player;

		public:

		AI(double,double, double, double, Amoeba*, bool);
	
		Amoeba *getPlayer(){
			return player;
		}

		int getIdentifier(){
			return AI_TYPE;
		}

		void setPlayer(Amoeba *player){
			this->player = player;
		}
		
	
		
		void update();

		void extendAttackArm();

		void extendDefendArm();		
	
};