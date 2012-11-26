#include"Amoeba.h"

Amoeba::Amoeba():balls(0.0,0.0,1.0){}

Amoeba::Amoeba(double px, double py,double pz, double radius, double scale, bool normal) : balls(0.0,1.0,1.0)
{
	velX = 0;
	velY = 0;
	velZ = 0;
	this->player = NULL;
	this->px = px;
	this->py = py;
	this->pz = pz;
	this->radius = radius;
	this->normal = normal;
	this->scale = scale;
	needToResize = false;
	isHit  = false;
	isHitTimer = 0;

	attackArm = NULL;

	attackActive = false;
	attackArmTimer = 0;

	attackSpacing1 = 0;
	attackSpacing2 = 0;
	attackSpacing3 = 0;


	leftMx = 0;
	leftMy = 0;
	lslope = 0;

	defendArm = NULL;

	defendActive = false;
	defendSegActive = false;
	defendSeg2Active = false;
	defendArmTimer = 0;
	defendWaitTimer = 0;

	defendSpacing1 = 0;
	defendSpacing2 = 0;
	defendSpacing3 = 0;

	rightMx = 0;
	rightMy = 0;
	rslope = 0;


	isCollision = false;
	colPx = 0;
	colPy = 0;
	colPz = 0;
	colAngle = 0;

	canMoveUp = true;
	canMoveDown = true;
	canMoveLeft = true;
	canMoveRight = true;

	isBody = false;
	isDefend = false;
	isAttack = false;
	isWall = false;

	attackFistPx = 0;
	attackFistPy = 0;
	attackFistPz = 0;
	attackFistRadius = 0;

	defendFistPx = 0;
	defendFistPy = 0;
	attackFistPz = 0;
	defendFistRadius = 0;

	balls.addMetaball(new Metaball3D(px,py,pz,radius));//body of amoeba
}

bool Amoeba::AmoebaCollision( Amoeba* other){
	//(y2 - y1)² + (x2 - x1)²

	isCollision = false;
	isBody = false;
	isAttack = false;
	isDefend = false;
	isWall = false;

	double attackData[4];
	double distance;
	other->getAttackData(attackData);

	/*Wall Collision Test*/
	if(px-radius < 0 || px+radius > screenRight)
	{
		if( px + radius > screenRight){
			balls.shiftGroup(screenRight - px - radius ,0, 0);
			px += screenRight - px - radius;
		} else if(px - radius < 0)
		{
			balls.shiftGroup(-px +radius, 0, 0);
			px += -px + radius;		
		}
		velX = -velX;		
	}

	if(pz - radius < 0 || pz + radius > screenTop)
	{
		if(py + radius > screenTop){
			balls.shiftGroup(0, 0,screenTop - pz - radius);
			pz += screenTop - pz - radius;
		} else if(pz - radius < 0)
		{
			balls.shiftGroup(0,0, -pz +radius);
			pz += -pz + radius;		
		}

		velZ = -velZ;			
	}	 
				
	if(attackData[0] != 0 && attackData[3] == scale*3)
	{

		distance = sqrt(((pz - attackData[2] ) * (pz - attackData[2]) + ((px - attackData[0]) * (px - attackData[0])))) ;

		colPx = attackData[0];
		colPz = attackData[2];

		if(distance < radius + attackData[3] + 5)
		{
			retractAttackArm();
			retractDefendArm();

			balls.decreaseRadius(10);
			if( radius > 10)
			{
				radius -= 10;
			}
			else
			{
				radius = 5;
			}

			if(px < colPx)
			{
				velX = -10;
			}
			else
			{
				velX = 10;
			}

			if(pz < colPz)
			{
				velZ = -10;
			}
			else
			{
				velZ = 10;
			}

			isAttack = true;
			return true;
		}
	}
	

	/*Defend Collision Test*/
	double DefendData[4]; 

	other->getDefendData(DefendData);
			
	if(DefendData[0] != 0 && DefendData[3] == scale*12)
	{
				
		distance = sqrt(((pz - DefendData[2] ) * (pz - DefendData[2]) +  ((px - DefendData[0]) * (px - DefendData[0])))) ; 
			
		colPx = DefendData[0];
		colPz = DefendData[2];

		if(distance < radius + DefendData[3] + 5)
		{

			if(px < colPx)
			{
				velX = -10;
			}
			else
			{
				velX = 10;
			}

			if(pz < colPz)
			{
				velZ = -10;
			}
			else
			{
				velZ = 10;
			}
			isDefend = true;
			isCollision = true;

			return true;
		}
	}

	/*Body Collision Test*/
	distance = sqrt(  ((pz - other->getPz()) * (pz - other->getPz()) ) +  ((px - other->getPx() ) * (px - other->getPx() ))) ; 

	if(distance < radius + other->getRadius())
	{
		velX = 0;
		velY = 0;
		velZ = 0;
		canMoveLeft = !( px > other->getPx()); 
		canMoveRight = !( px < other->getPx()); 
		canMoveDown = !( pz > other->getPz()); 
		canMoveUp = !( pz < other->getPz()); 
		isBody = true;
		isCollision = true;
		return true;
	}

	return isCollision;
}

void Amoeba::collision(Sprite* obj)
{ 
	if( obj->getIdentifier() == AI_TYPE || obj->getIdentifier() == AMOEBA_TYPE)
	{
		 AmoebaCollision( (Amoeba*) obj);
	}
	if( obj->getIdentifier() == OBSTACLE_TYPE){
		Obstacle *ob = (Obstacle*)obj;
		std::pair<double, double>positions[2];
		double radiuses[2];
		ob->getPositions( positions);
		ob->getRadiuses(radiuses);
		
		for( int i = 0; i<2; i++){
			if( (px - positions[i].first) * (px - positions[i].first) + (pz - positions[i].second) * (pz - positions[i].second) <= radius * radius + radiuses[i] * radiuses[i] - 5){
				isCollision = true;
				retractAttackArm();
				retractDefendArm();
				balls.decreaseRadius(10);
				if( radius > 10)
				{
					radius -= 10;
				}
				else
				{
					radius = 5;
				}
				if(px < positions[i].first)
				{
					velX = -10;
				}
				else
				{
					velX = 10;
				}

				if(pz < positions[i].second)
				{
					velZ = -10;
				}
				else
				{
					velZ = 10;
				}

			}
		}
	}

};

void Amoeba::update()
{

	if( radius <= 5){
		radius = 50;
		balls.setRadius(50);
		balls.shiftGroup( 250 - px, 0, 250 - pz);
		px = 250;
		pz = 250;
		velX = 0;
		velZ = 0;
	}

	if( velX > 0.1){
		velX -= 0.1;
	} else if( velX < -0.1){
		velX+= 0.1;
	}else{
		velX = 0;
	}

	if( velZ > 0.1){
		velZ -= 0.1;
	} else if( velZ < -0.1){
		velZ+= 0.1;
	}else{
		velZ = 0;
	}

	px += velX;
	pz += velZ;

	if(!isCollision)
	{
		canMoveUp = true;
		canMoveDown = true;
		canMoveLeft = true;
		canMoveRight = true;
	}

	if(time(NULL) - isHitTimer > 3)
	{
		isHit = false;
	}
			
	retractArm();
	balls.shiftGroup(velX, velY, velZ);

}

void Amoeba::extendDefendArm()
{
	if(rslope == 0)
	{

		rslope = ( ( player->getPz() - pz) / (player->getPx() - px) );
		double angle = atan(rslope);

		defendSpacing1 = radius + radius/2;
		defendSpacing2 = defendSpacing1 + radius/2;
		defendSpacing3 = defendSpacing2 + radius/2;

		if(rightMx < px)
		{
			defendSpacing1 = (-1)*defendSpacing1;
			defendSpacing2 = (-1)*defendSpacing2;
			defendSpacing3 = (-1)*defendSpacing3;
		}
		
		

		if(!defendActive && (defendWaitTimer - time(NULL) <= 0 ))
		{
				defendActive = true;
				defendArmTimer = time(NULL);
				defendWaitTimer = time(NULL) + 7;
				defendArm = new Metaball3DGroup(0.0, 1.0, 0.0);
				
				defendArm->addMetaball(new Metaball3D(px + cos(angle)* defendSpacing1, 0, pz + sin(angle) *defendSpacing1, scale*3.0));
				defendArm->addMetaball(new Metaball3D(px + cos(angle)* defendSpacing2, 0, pz + sin(angle) *defendSpacing2,scale*3.0));
				defendArm->addMetaball(new Metaball3D(px + cos(angle)* defendSpacing3, 0, pz + sin(angle)*defendSpacing3,scale*12.0));
				balls.addSubgroup(defendArm);

				defendFistPx = px + cos(angle)*defendSpacing3;
				defendFistPz = pz + sin(angle)*defendSpacing3;
				defendFistRadius = scale*12;
		}
		else
		{
			rslope = 0;
		}
	}


}

void Amoeba::extendAttackArm()
{
	if(lslope == 0)
	{

		lslope = ( ( player->getPz() - pz) / (player->getPx() - px) );
		double angle = atan(lslope);


		attackSpacing1 = radius + radius/2;
		attackSpacing2 = attackSpacing1 + radius/2;
		attackSpacing3 = attackSpacing2 + radius/2;


		if(leftMx < px)
		{

			attackSpacing1 = (-1)*attackSpacing1;
			attackSpacing2 = (-1)*attackSpacing2;
			attackSpacing3 = (-1)*attackSpacing3;
		}


		if(!attackActive)
		{
			attackActive = true;
			attackArmTimer = time(NULL);

			attackArm = new Metaball3DGroup(1.0, 0.0, 0.0);
			attackArm->addMetaball(new Metaball3D(px + cos(angle)*attackSpacing1, 0, pz + sin(angle)*attackSpacing1, scale*3.0));
			attackArm->addMetaball(new Metaball3D(px + cos(angle) *attackSpacing2, 0, pz + sin(angle)*attackSpacing2,scale*3.0));
			attackArm->addMetaball(new Metaball3D(px + cos(angle)* attackSpacing3, 0, pz + sin(angle)*attackSpacing3,scale*3.0));
			balls.addSubgroup(attackArm);

			attackFistPx = px + cos(angle)*attackSpacing3;
			attackFistPy = pz + sin(angle)*attackSpacing3;
			attackFistRadius = scale*3;

		}	
	}
}

void Amoeba::retractAttackArm(){
	attackFistPx = 0;
	attackFistPz = 0;
	balls.popSubgroup();
	lslope = 0;
	attackArm = NULL;
	attackActive = false;

}

void Amoeba::retractDefendArm(){
	defendFistPx  = 0;
	defendFistPz = 0;
	balls.popSubgroup();
	rslope = 0;
	defendArm = NULL;
	defendActive = false;
}

void Amoeba::retractArm()
{
	if(attackActive && time(NULL) - attackArmTimer > 0.25)
	{
		retractAttackArm();
	}

	if(defendActive && time(NULL) - defendArmTimer > 0.25)
	{
		retractDefendArm();
	}

}

void Amoeba:: setupTarget(Amoeba* target)
{
	player = target;
}