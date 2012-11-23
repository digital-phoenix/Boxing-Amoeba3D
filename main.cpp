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
#include "CCamera.h"

#define CAMERASPEED	0.05f// The Camera Speed

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

//angle of rotation
GLfloat angle = 0.0;
 
//diffuse light color variables
GLfloat dlr = 1.0;
GLfloat dlg = 1.0;
GLfloat dlb = 1.0;
 
//ambient light color variables
GLfloat alr = 0.2;
GLfloat alg = 0.2;
GLfloat alb = 0.2;

//specular light color variables
GLfloat slr = 0.0;
GLfloat slg = 0.0;
GLfloat slb = 0.0;
 
//light position variables
GLfloat lx = 0.0;
GLfloat ly = 0.0;
GLfloat lz = 1.0;
GLfloat lw = 0.0;


CCamera camera;//Camera

int numTex = 0;

struct texData
{
	/*Image Related*/

		// Data read from the header of the BMP file
	unsigned char header[54]; // Each BMP file begins by a 54-bytes header
	unsigned int dataPos;     // Position in the file where the actual data begins
	unsigned int width, height;
	unsigned int imageSize;   // = width*height*3
	// Actual RGB data
	unsigned char * data;

	GLuint textureID;

}tex[6];

void Draw_Grid(float x, float y)
{															
	x = x/2;
	y = y/2;

	for(float i =  x; i <= y; i += 5)
	{
		glBegin(GL_LINES);
			glColor3ub(150, 190, 150);						
			glVertex3f(x, 0, i);					
			glVertex3f(y, 0, i);
			glVertex3f(i, 0,x);							
			glVertex3f(i, 0, y);
		glEnd();
	}
}

void Draw_Skybox(float x, float y, float z, float width, float height, float length)
{
	// Center the Skybox around the given x,y,z position
	x = x - width  / 2;
	y = y - height / 2;
	z = z - length / 2;


	glEnable(GL_TEXTURE_2D);


	// Draw Front side
	glBindTexture(GL_TEXTURE_2D, tex[0].textureID);
	glBegin(GL_QUADS);	
		glNormal3d(0.0, 0.0, 1.0);
		glTexCoord2f(1.0f, 0.0f); glVertex3f(x,		  y,		z+length);
		glTexCoord2f(1.0f, 1.0f); glVertex3f(x,		  y+height, z+length);
		glTexCoord2f(0.0f, 1.0f); glVertex3f(x+width, y+height, z+length); 
		glTexCoord2f(0.0f, 0.0f); glVertex3f(x+width, y,		z+length);
	glEnd();

	// Draw Back side
	glBindTexture(GL_TEXTURE_2D, tex[1].textureID);
	glBegin(GL_QUADS);		
		glNormal3d(0.0, 0.0, -1.0);
		glTexCoord2f(1.0f, 0.0f); glVertex3f(x+width, y,		z);
		glTexCoord2f(1.0f, 1.0f); glVertex3f(x+width, y+height, z); 
		glTexCoord2f(0.0f, 1.0f); glVertex3f(x,		  y+height,	z);
		glTexCoord2f(0.0f, 0.0f); glVertex3f(x,		  y,		z);
	glEnd();

	// Draw Left side
	glBindTexture(GL_TEXTURE_2D, tex[2].textureID);
	glBegin(GL_QUADS);		
		glNormal3d(1.0, 0.0, 0.0);
		glTexCoord2f(1.0f, 1.0f); glVertex3f(x,		  y+height,	z);	
		glTexCoord2f(0.0f, 1.0f); glVertex3f(x,		  y+height,	z+length); 
		glTexCoord2f(0.0f, 0.0f); glVertex3f(x,		  y,		z+length);
		glTexCoord2f(1.0f, 0.0f); glVertex3f(x,		  y,		z);		
	glEnd();

	// Draw Right side
	glBindTexture(GL_TEXTURE_2D, tex[3].textureID);
	glBegin(GL_QUADS);		
		glNormal3d(-1.0, 0.0, 0.0);
		glTexCoord2f(0.0f, 0.0f); glVertex3f(x+width, y,		z);
		glTexCoord2f(1.0f, 0.0f); glVertex3f(x+width, y,		z+length);
		glTexCoord2f(1.0f, 1.0f); glVertex3f(x+width, y+height,	z+length); 
		glTexCoord2f(0.0f, 1.0f); glVertex3f(x+width, y+height,	z);
	glEnd();

	// Draw Up side
	glBindTexture(GL_TEXTURE_2D, tex[4].textureID);
	glBegin(GL_QUADS);		
		glNormal3d(0.0, -1.0, 0.0);
		glTexCoord2f(0.0f, 0.0f); glVertex3f(x+width, y+height, z);
		glTexCoord2f(1.0f, 0.0f); glVertex3f(x+width, y+height, z+length); 
		glTexCoord2f(1.0f, 1.0f); glVertex3f(x,		  y+height,	z+length);
		glTexCoord2f(0.0f, 1.0f); glVertex3f(x,		  y+height,	z);
	glEnd();

	// Draw Down side
	glBindTexture(GL_TEXTURE_2D, tex[5].textureID);
	glBegin(GL_QUADS);		
		glNormal3d(0.0, 1.0, 0.0);
		glTexCoord2f(0.0f, 0.0f); glVertex3f(x,		  y,		z);
		glTexCoord2f(1.0f, 0.0f); glVertex3f(x,		  y,		z+length);
		glTexCoord2f(1.0f, 1.0f); glVertex3f(x+width, y,		z+length); 
		glTexCoord2f(0.0f, 1.0f); glVertex3f(x+width, y,		z);
	glEnd();

} 
void Lighting()
{
  
	GLfloat fullambient[4] = {1.0,1.0,1.0,1.0};
	GLfloat position[4] = {lx,ly,lz,lw};
	GLfloat ambient[4]  = {alr,alg,alb,1.0};
	GLfloat diffuse[4]  = {dlr,dlg,dlb,1.0};
	GLfloat specular[4] = {slr,slg,slb,1.0};

   /* Turn off all the lights */
   glDisable(GL_LIGHT0);
   glDisable(GL_LIGHT1);
   glDisable(GL_LIGHT2);
   glDisable(GL_LIGHT3);
   glDisable(GL_LIGHT4);
   glDisable(GL_LIGHT5);
   glDisable(GL_LIGHT6);
   glDisable(GL_LIGHT7);
   glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER,GL_TRUE);
   glLightModeli(GL_LIGHT_MODEL_TWO_SIDE,GL_FALSE);

   /* Turn on the appropriate lights */
   glLightModelfv(GL_LIGHT_MODEL_AMBIENT,fullambient);
   glLightfv(GL_LIGHT0,GL_POSITION,position);
   glLightfv(GL_LIGHT0,GL_AMBIENT,ambient);
   glLightfv(GL_LIGHT0,GL_DIFFUSE,diffuse);
   glLightfv(GL_LIGHT0,GL_SPECULAR,specular);
   glEnable(GL_LIGHT0);

   /* Sort out the shading algorithm */
   glShadeModel(GL_SMOOTH);

   /* Turn lighting on */
   glEnable(GL_LIGHTING);

}

GLuint loadBMP_custom(const char * imagepath)
{/*So far only load 24 bit bitmaps work.*/


	// Open the file
	FILE *file = fopen(imagepath,"rb");
	if (!file)
	{
		printf("Image could not be opened\n"); 
		return 0;
	}

	if ( fread(tex[numTex].header, 1, 54, file)!=54 )
	{ // If not 54 bytes read : problem
		printf("Not a correct BMP file\n");
	
		return 0;
	}

	if ( tex[numTex].header[0]!='B' || tex[numTex].header[1]!='M' )
	{
		printf("Not a correct BMP file\n");
		return 0;
	}

	// Read ints from the byte array
	tex[numTex].dataPos    = *(int*)&(tex[numTex].header[0x0A]);
	tex[numTex].imageSize  = *(int*)&(tex[numTex].header[0x22]);
	tex[numTex].width      = *(int*)&(tex[numTex].header[0x12]);
	tex[numTex].height     = *(int*)&(tex[numTex].header[0x16]);

    // Some BMP files are misformatted, guess missing information
	if (tex[numTex].imageSize==0) 
	{
		tex[numTex].imageSize=tex[numTex].width*tex[numTex].height*3;// 3 : one byte for each Red, Green and Blue component
		puts("Missing imageSize");
		printf("%d x %d\n", tex[numTex].width, tex[numTex].height);
	}
			
	if (tex[numTex].dataPos==0)  
	{
		tex[numTex].dataPos=54; // The BMP header is done that way
		puts("Missing data pos\n");
	}

		// Create a buffer
	tex[numTex].data = new unsigned char [tex[numTex].imageSize];
 

	// Read the actual data from the file into the buffer
	fread(tex[numTex].data,1,tex[numTex].imageSize,file);
 
	//Everything is in memory now, the file can be closed
	fclose(file);

	
	glGenTextures(1, &(tex[numTex].textureID) );
 

	glBindTexture(GL_TEXTURE_2D, tex[numTex].textureID);

	
	glTexImage2D(GL_TEXTURE_2D, 0,GL_RGB, tex[numTex].width, tex[numTex].height, 0, GL_BGR_EXT, GL_UNSIGNED_BYTE, tex[numTex].data);
	

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	
	

	numTex++;
} 

void init ( GLvoid )   
{
	player = new Amoeba(0, 0, 25,1, true);
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
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	glColorMaterial(GL_FRONT_AND_BACK,GL_AMBIENT_AND_DIFFUSE);
	glEnable(GL_COLOR_MATERIAL);

	glEnable(GL_TEXTURE_2D);
	

	camera.Position_Camera(0, 2.5f, 5,	0, 2.5f, 0,   0, 1, 0);

}

void display ( void )   
{
	static double amoebaVertices[5][24000][4];
	static double AmoebaNormals[5][24000][4];
	int nVertices[10];
	/*
	FPS++;
	currentTime = clock();
	if( currentTime - lastTime >= CLOCKS_PER_SEC){
		//printf("FPS = %d\n", FPS);
		lastTime = currentTime;
		FPS = 0;
	}*/

	GLfloat specular[4] = {1.0,1.0,1.0,1.0};
	GLfloat shiny[1] = {5.0};

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	
	
	
	glLoadIdentity();	

	Lighting();
	gluLookAt(camera.mPos.x,  camera.mPos.y,  camera.mPos.z, camera.mView.x, camera.mView.y, camera.mView.z, camera.mUp.x,   camera.mUp.y,   camera.mUp.z);
	
	glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,specular);
	glMaterialfv(GL_FRONT_AND_BACK,GL_SHININESS,shiny);

	Draw_Skybox(0,0,0,100,100,100);
	glColor4f(0.0,0.0,1.0,1.0);
	Draw_Grid(-100,100);

	
	//glLoadIdentity();									// Reset The Current Modelview Matrix
	//glTranslatef(0.0f,0.0f,-10.0f);						// Move Left 1.5 Units And Into The Screen 6.0

	/*	glBegin(GL_TRIANGLES);

	glVertex3d(0.0,0.0,-10.0);
	glVertex3d(5.0,0.0,-10.0);
	glVertex3d(0.0,5.0,-10.0);
	glEnd();*/

	int numSprites = 0;
	for( std::list<Sprite*>::iterator it = sprites.begin(); it != sprites.end(); it++)
	{

		for( std::list<Sprite*>::iterator it2 = sprites.begin(); it2 != sprites.end(); it2++)
		{
			
			if(it != it2)
			{
				//(*it)->collision(*it2);
			}
		}

		nVertices[numSprites] = (*it)->draw(amoebaVertices[numSprites], AmoebaNormals[numSprites]);
		numSprites++;		
		//(*it)->update();
	}
	glBegin(GL_TRIANGLES);
	for( int j = 0; j < numSprites; j++){
		for( int i = 0; i < nVertices[j]; i++){
			glVertex4d( amoebaVertices[j][i][0], amoebaVertices[j][i][1], amoebaVertices[j][i][2], amoebaVertices[j][i][3]); 
			glNormal3d( AmoebaNormals[j][i][0], AmoebaNormals[j][i][1], AmoebaNormals[j][i][2]);
		}
	}
	glEnd();

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

void HandleMainMenu(int){}
void HandleSpeedMenu(int){}
void HandleSpinMenu(int){}


void keyboard ( unsigned char key, int x, int y )
{
	switch ( key ) 
	{
		case(27):
			exit(0);
			break;
		
		case('<'):
			player->extendAttackArm();
			break;

		case('>'):
			player->retractArm();
			break;

		case('w'):
			camera.Move_Camera(CAMERASPEED);
			break;

		case('s'):
			camera.Move_Camera(-CAMERASPEED);
			break;
		case('a'):
			camera.Rotate_View(0,-CAMERASPEED, 0);
			break;

		case('d'):
			camera.Rotate_View(0, CAMERASPEED, 0);
			break;

		case('e'):
			camera.Fly(CAMERASPEED);
			break;

		case('q'):
			camera.Fly(-CAMERASPEED);
			break;

		case('r'):
			camera.Look(CAMERASPEED);
			break;

		case('	')://tab
			camera.Look(-CAMERASPEED);
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
	 int mainmenu,speedmenu,spinmenu;

	glutInit( &argc, argv );
	init();
	glutInitDisplayMode( GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize( 500, 500 ); 
	glutInitWindowPosition (100, 100); //set the position of the window
	glutCreateWindow( "Amoeba Boxing" );
	//glutGameModeString("800x600:16@60");
	//glutEnterGameMode();
	glutDisplayFunc( display );
	glutReshapeFunc( reshape );
	glutMouseFunc(mouse);
	glutKeyboardFunc( keyboard );
	glutSpecialFunc( arrow_keys );

	loadBMP_custom("cubemap1.bmp");
    loadBMP_custom("cubemap2.bmp");
	loadBMP_custom("cubemap3.bmp");
	loadBMP_custom("cubemap4.bmp");
	loadBMP_custom("cubemap5.bmp");
	loadBMP_custom("cubemap6.bmp");

	for(int i = 0; i<numTex; i++)
		printf("%d\n", tex[i].textureID);


	/* Set up the speed menu */
   speedmenu = glutCreateMenu(HandleSpeedMenu);
   glutAddMenuEntry("Stop",1);
   glutAddMenuEntry("Slow",2);
   glutAddMenuEntry("Medium",3);
   glutAddMenuEntry("Fast",4);
   glutAddMenuEntry("Very fast",5);

   /* Set up the spin menu */
   spinmenu = glutCreateMenu(HandleSpinMenu);
   glutAddMenuEntry("1 degree",1);
   glutAddMenuEntry("2 degrees",2);
   glutAddMenuEntry("3 degrees",3);
   glutAddMenuEntry("5 degrees",4);

   /* Set up the main menu */
   mainmenu = glutCreateMenu(HandleMainMenu);
   glutAddSubMenu("Rotation",speedmenu);
   glutAddSubMenu("Camera rotation steps",spinmenu);
   glutAddMenuEntry("Toggle construction lines",1);
   glutAddMenuEntry("Quit",9);
   glutAttachMenu(GLUT_RIGHT_BUTTON);

	glutMainLoop();

	return 0;
}