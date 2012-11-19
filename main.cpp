#include <windows.h>
#include <gl/gl.h>     
#include <gl/glut.h>   
#include <list>
#include<time.h>
#include "Sprite.h"
#include "Amoeba.h"
#include "AI.h"
#include "GraphicState.h"
#include "Obstacle.h"

std::list<Sprite*> sprites;
Amoeba *player;
AI *ai;

int screenLeft = 0;
int screenRight = 500;
int screenTop = 500;
int screenBottom = 0;
clock_t currentTime;
clock_t lastTime = clock();
int FPS = 0;

void init ( GLvoid )   
{
	player = new Amoeba(0, 0, 50,1, true);
	//ai = new AI(60,60 , 50,1, player, true);
	sprites.push_back( (Sprite*) (player) );
	//sprites.push_back( (Sprite*) (  ai  ) );
	//sprites.push_back( (Sprite*)(new Obstacle()) );
	glShadeModel(GL_SMOOTH);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glEnable ( GL_COLOR_MATERIAL );
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	glEnable(GL_LIGHTING);

	GLfloat position [] = { 0.0f, 0.0f, -20.0f, 1.0f };
	GLfloat ambient [] = { 0.0f, 0.3f, 0.3f, 1.0f };
	GLfloat diffuse [] = { 0.0f, 1.0f, 0.0f, 0.5f };

	glLightfv(GL_LIGHT0, GL_POSITION, position);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);

	glEnable(GL_LIGHT0);

	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
}

void display ( void )   
{

	FPS++;
	currentTime = clock();
	if( currentTime - lastTime >= CLOCKS_PER_SEC){
		//printf("FPS = %d\n", FPS);
		lastTime = currentTime;
		FPS = 0;
	}

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	

	glColor4f(0.0,0.0,1.0,1.0);

	glLoadIdentity();									// Reset The Current Modelview Matrix
	glTranslatef(0.0f,0.0f,-10.0f);						// Move Left 1.5 Units And Into The Screen 6.0

	/*	glBegin(GL_TRIANGLES);

	glVertex3d(0.0,0.0,-10.0);
	glVertex3d(5.0,0.0,-10.0);
	glVertex3d(0.0,5.0,-10.0);
	glEnd();*/

	for( std::list<Sprite*>::iterator it = sprites.begin(); it != sprites.end(); it++)
	{

		for( std::list<Sprite*>::iterator it2 = sprites.begin(); it2 != sprites.end(); it2++)
		{
			
			if(it != it2)
			{
				//(*it)->collision(*it2);
			}
		}

		(*it)->draw();
		//(*it)->update();
	}

	glutSwapBuffers ( );
	glutPostRedisplay();
}

void reshape ( int w, int h )
{
	if (h==0)										
	{
		h=1;										
	}

	glViewport(0,0,w,h);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	gluPerspective(45.0f,(GLfloat)w/(GLfloat)h,0.1f,100.0f);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}


void mouse(int btn, int state, int x, int y)
{
    if(btn==GLUT_LEFT_BUTTON && state==GLUT_DOWN)
    {
		player->setLeftMousePos(x,screenTop -y);
		player->extendAttackArm();
    }

    if(btn==GLUT_RIGHT_BUTTON && state==GLUT_DOWN)
    {
		player->setRightMousePos(x, screenTop - y);
		player->extendDefendArm();
    }
}

void keyboard ( unsigned char key, int x, int y )
{
	switch ( key ) 
	{
		case(27):
			exit(0);
			break;
		
		case('e'):
			player->extendAttackArm();
			break;

		case('r'):
			player->retractArm();
			break;

		case('q'):
			player->incAngle();
			break;

		case('w'):
			player->decAngle();
		break;

		case(' '):
			player->setVelocity(0,0);
			break;

		default:
			break;
	}
}

void arrow_keys ( int a_keys, int x, int y )
{

	bool moves[4];
	player->getAvailableMoves(moves);

	switch ( a_keys ) {
		case GLUT_KEY_UP:
			if( moves[0] )
				player->setVely( 5.0f);
			break;
		case GLUT_KEY_DOWN:
			if( moves[1] )
				player->setVely( -5.0f);
			break;
		case GLUT_KEY_LEFT:
			if( moves[2] )
				player->setVelx(-5.0f);
			break;
		case GLUT_KEY_RIGHT:
			if( moves[3] )
				player->setVelx(5.0f);
			break;
		default:
			break;
	}

}

int main ( int argc, char** argv )
{
	glutInit( &argc, argv );
	init();
	glutInitDisplayMode( GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize( 500, 500 ); 
	glutCreateWindow( "Amoeba Boxing" );
	//glutGameModeString("800x600:16@60");
	//glutEnterGameMode();
	glutDisplayFunc( display );
	glutReshapeFunc( reshape );
	glutMouseFunc(mouse);
	glutKeyboardFunc( keyboard );
	glutSpecialFunc( arrow_keys );
	glutMainLoop();

	return 0;
}