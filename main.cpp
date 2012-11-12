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

	player = new Amoeba(51,51, 50,1, true);
	ai = new AI(450,450 , 50,1, player, true);
	sprites.push_back( (Sprite*) (player) );
	sprites.push_back( (Sprite*) (  ai  ) );
	sprites.push_back( (Sprite*)(new Obstacle()) );
	glShadeModel(GL_SMOOTH);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glEnable ( GL_COLOR_MATERIAL );
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

	glClear(GL_COLOR_BUFFER_BIT);	
	glLoadIdentity();	

	for( std::list<Sprite*>::iterator it = sprites.begin(); it != sprites.end(); it++)
	{

		for( std::list<Sprite*>::iterator it2 = sprites.begin(); it2 != sprites.end(); it2++)
		{
			
			if(it != it2)
			{
				(*it)->collision(*it2);
			}
		}

		(*it)->draw();
		(*it)->update();
	}

	glutSwapBuffers ( );
	glutPostRedisplay();
}

void reshape ( int w, int h )
{
	glViewport( 0, 0, w , h );
	glMatrixMode( GL_PROJECTION );  
	glLoadIdentity();   
	
	gluOrtho2D(screenLeft, screenRight, screenBottom, screenTop);
	glMatrixMode( GL_MODELVIEW );  
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
	glutInitDisplayMode( GLUT_RGB | GLUT_DOUBLE );
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