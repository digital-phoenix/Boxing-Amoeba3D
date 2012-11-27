#include <windows.h>
#include <gl/gl.h>     
#include <gl/glut.h>   
#include <list>
#include<time.h>
#include <stdio.h>
#include "Sprite.h"
#include "Amoeba.h"
#include "AI.h"
#include "GraphicState.h"
#include "Obstacle.h"
#include "CCamera.h"

#define CAMERASPEED	0.5f// The Camera Speed


#define ABS(x) (x < 0 ? -(x) : (x))
#define MIN(x,y) (x < y ? x : y)
#define MAX(x,y) (x > y ? x : y)
#define TRUE  1
#define FALSE 0
#define ESC 27
#define PI 3.141592653589793238462643
#define DTOR            0.0174532925
#define RTOD            57.2957795
#define CROSSPROD(p1,p2,p3) p3.x = p1.y*p2.z - p1.z*p2.y; p3.y = p1.z*p2.x - p1.x*p2.z; p3.z = p1.x*p2.y - p1.y*p2.x;

std::list<Sprite*> sprites;
Amoeba *player;
AI *ai;

int screenLeft = -500;
int screenRight = 500;
int screenTop = 500;
int screenBottom = -500;
clock_t currentTime;
clock_t lastTime = clock();
int FPS = 0;

/* Flags */
int fullscreen = FALSE;
int stereo = !TRUE;
int showconstruct = FALSE;
int windowdump = FALSE;
int record = FALSE;
int debug = FALSE;

//angle of rotation
GLfloat angle = 0.0;
 
//diffuse light color variables
GLfloat dlr = 0.5;
GLfloat dlg = 0.5;
GLfloat dlb = 0.5;
 
//ambient light color variables
GLfloat alr = 0.5;
GLfloat alg = 0.3;
GLfloat alb = 0.3;

//specular light color variables
GLfloat slr = 0.4;
GLfloat slg = 0.4;
GLfloat slb = 0.4;
 
//light position variables
GLfloat lx = 0.0;
GLfloat ly = 0.0;
GLfloat lz = 1.0;
GLfloat lw = 0.0;

//Shiny variable
GLfloat shinyVar = 5.0;

//Light increase value
GLfloat lightVal = 0.5;

//Texture Colour Parameters
GLfloat tr = 1.0;
GLfloat tg = 1.0;
GLfloat tb = 1.0;

typedef struct {
   double x,y,z;
} XYZ;
typedef struct {
   double r,g,b;
} COLOUR;
typedef struct {
   unsigned char r,g,b,a;
} PIXELA;
typedef struct {
   XYZ vp;              /* View position           */
   XYZ vd;              /* View direction vector   */
   XYZ vu;              /* View up direction       */
   XYZ pr;              /* Point to rotate about   */
   double focallength;  /* Focal Length along vd   */
   double aperture;     /* Camera aperture         */
   double eyesep;       /* Eye separation          */
   int screenwidth,screenheight;
} CAMERA;

CCamera ccamera;//Camera
void RotateCamera(int,int,int);
void TranslateCamera(int,int);
void CameraHome(int);
void Normalise(XYZ *);

XYZ origin = {0.0,0.0,0.0};
CAMERA camera;
int numTex = 0;
double dtheta = 1;
double rotateangle = 0.0;    /* Pulsar Rotation angle */
double rotatespeed = 1;

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

void GiveUsage(char *cmd)
{
   fprintf(stderr,"Usage: %s [-h] [-f] [-s] [-c] [-q n]\n",cmd);
   fprintf(stderr,"          -h   this text\n");
   fprintf(stderr,"          -f   full screen\n");
   fprintf(stderr,"          -s   stereo\n");
   fprintf(stderr,"          -c   show construction lines\n");
   fprintf(stderr,"Key Strokes\n");
   fprintf(stderr,"  arrow keys   rotate left/right/up/down\n");
   fprintf(stderr,"  left mouse   rotate\n");
   fprintf(stderr,"middle mouse   roll\n");
   fprintf(stderr,"           c   toggle construction lines\n");
   fprintf(stderr,"           i   translate up\n");
   fprintf(stderr,"           k   translate down\n");
   fprintf(stderr,"           j   translate left\n");
   fprintf(stderr,"           l   translate right\n");
   fprintf(stderr,"           [   roll clockwise\n");
   fprintf(stderr,"           ]   roll anti clockwise\n");
   fprintf(stderr,"           q   quit\n");
   exit(-1);
}

void Normalise(XYZ *p)
{
   double length;

   length = sqrt(p->x * p->x + p->y * p->y + p->z * p->z);
   if (length != 0) {
      p->x /= length;
      p->y /= length;
      p->z /= length;
   } else {
      p->x = 0;
      p->y = 0;
      p->z = 0;
   }
}

XYZ CalcNormal(XYZ p,XYZ p1,XYZ p2)
{
   XYZ n,pa,pb;

   pa.x = p1.x - p.x;
   pa.y = p1.y - p.y;
   pa.z = p1.z - p.z;
   pb.x = p2.x - p.x;
   pb.y = p2.y - p.y;
   pb.z = p2.z - p.z;
   Normalise(&pa);
   Normalise(&pb);
  
   n.x = pa.y * pb.z - pa.z * pb.y;
   n.y = pa.z * pb.x - pa.x * pb.z;
   n.z = pa.x * pb.y - pa.y * pb.x;
   Normalise(&n);

   return(n);
}

/*
   Move the camera to the home position 
*/
void CameraHome(int mode)
{
   camera.aperture = 50;
   camera.focallength = 70;
   camera.eyesep = camera.focallength / 20;

/*
   camera.vp.x = camera.focallength; 
   camera.vp.y = 0; 
   camera.vp.z = 0;
   camera.vd.x = -1;
   camera.vd.y = 0;
   camera.vd.z = 0;
*/
   /* Special camera position so the beam crosses the view */

/*   camera.vp.x = 39;
   camera.vp.y = 53;
   camera.vp.z = 22;
  camera.vd.x = -camera.vp.x; 
   camera.vd.y = -camera.vp.y; 
   camera.vd.z = -camera.vp.z;
*/
 // -41, 41, 50,-27, 23, 32,   0, 1, 0
	camera.vp.x = -41;
	camera.vp.y = 41;
	camera.vp.z = 50;
	camera.vd.x = 14; 
	camera.vd.y = -18; 
	camera.vd.z = -18;

   camera.vu.x = 0;  
   camera.vu.y = 1; 
   camera.vu.z = 0;

   XYZ lookAt;
   lookAt.x = camera.vp.x + camera.vd.x;
   lookAt.y = camera.vp.y + camera.vd.y;
   lookAt.z = camera.vp.z + camera.vd.z;
   camera.pr = lookAt;

}

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

	 glColor4f(tr,tg ,tb,1.0);
	
	 // Draw Front side
	glBindTexture(GL_TEXTURE_2D, tex[0].textureID);
	glBegin(GL_QUADS);	
		glTexCoord2f(1.0f, 0.0f); glVertex3f(x,		  y,		z+length);
		glTexCoord2f(1.0f, 1.0f); glVertex3f(x,		  y+height, z+length);
		glTexCoord2f(0.0f, 1.0f); glVertex3f(x+width, y+height, z+length); 
		glTexCoord2f(0.0f, 0.0f); glVertex3f(x+width, y,		z+length);
	glEnd();

	// Draw Back side
	glBindTexture(GL_TEXTURE_2D, tex[1].textureID);
	glBegin(GL_QUADS);		
		glTexCoord2f(1.0f, 0.0f); glVertex3f(x+width, y,		z);
		glTexCoord2f(1.0f, 1.0f); glVertex3f(x+width, y+height, z); 
		glTexCoord2f(0.0f, 1.0f); glVertex3f(x,		  y+height,	z);
		glTexCoord2f(0.0f, 0.0f); glVertex3f(x,		  y,		z);
	glEnd();

	// Draw Left side
	glBindTexture(GL_TEXTURE_2D, tex[2].textureID);
	glBegin(GL_QUADS);		
		glTexCoord2f(1.0f, 1.0f); glVertex3f(x,		  y+height,	z);	
		glTexCoord2f(0.0f, 1.0f); glVertex3f(x,		  y+height,	z+length); 
		glTexCoord2f(0.0f, 0.0f); glVertex3f(x,		  y,		z+length);
		glTexCoord2f(1.0f, 0.0f); glVertex3f(x,		  y,		z);		
	glEnd();

	// Draw Right side
	glBindTexture(GL_TEXTURE_2D, tex[3].textureID);
	glBegin(GL_QUADS);		
		glTexCoord2f(0.0f, 0.0f); glVertex3f(x+width, y,		z);
		glTexCoord2f(1.0f, 0.0f); glVertex3f(x+width, y,		z+length);
		glTexCoord2f(1.0f, 1.0f); glVertex3f(x+width, y+height,	z+length); 
		glTexCoord2f(0.0f, 1.0f); glVertex3f(x+width, y+height,	z);
	glEnd();

	// Draw Up side
	glBindTexture(GL_TEXTURE_2D, tex[4].textureID);
	glBegin(GL_QUADS);		
		glTexCoord2f(0.0f, 0.0f); glVertex3f(x+width, y+height, z);
		glTexCoord2f(1.0f, 0.0f); glVertex3f(x+width, y+height, z+length); 
		glTexCoord2f(1.0f, 1.0f); glVertex3f(x,		  y+height,	z+length);
		glTexCoord2f(0.0f, 1.0f); glVertex3f(x,		  y+height,	z);
	glEnd();

	// Draw Down side
	glBindTexture(GL_TEXTURE_2D, tex[5].textureID);
	glBegin(GL_QUADS);		
		glTexCoord2f(0.0f, 0.0f); glVertex3f(x,		  y,		z);
		glTexCoord2f(1.0f, 0.0f); glVertex3f(x,		  y,		z+length);
		glTexCoord2f(1.0f, 1.0f); glVertex3f(x+width, y,		z+length); 
		glTexCoord2f(0.0f, 1.0f); glVertex3f(x+width, y,		z);
	glEnd();

	glDisable(GL_TEXTURE_2D);

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
   glLightModeli(GL_LIGHT_MODEL_TWO_SIDE,GL_TRUE);

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
	return 1;
} 


/*
   Translate (pan) the camera view point
   In response to i,j,k,l keys
   Also move the camera rotate location in parallel
*/
/*
   Rotate (ix,iy) or roll (iz) the camera about the focal point
   ix,iy,iz are flags, 0 do nothing, +- 1 rotates in opposite directions
   Correctly updating all camera attributes
*/
void RotateCamera(int ix,int iy,int iz)
{
   XYZ vp,vu,vd;
   XYZ right;
   XYZ newvp,newr;
   double radius,dd,radians;
   double dx,dy,dz;

   vu = camera.vu;
   Normalise(&vu);
   vp = camera.vp;
   vd = camera.vd;
   Normalise(&vd);
   CROSSPROD(vd,vu,right);
   Normalise(&right);
   radians = dtheta * PI / 180.0;

   /* Handle the roll */
   if (iz != 0) {
      camera.vu.x += iz * right.x * radians;
      camera.vu.y += iz * right.y * radians;
      camera.vu.z += iz * right.z * radians;
      Normalise(&camera.vu);
      return;
   }

   /* Distance from the rotate point */
   dx = camera.vp.x - camera.pr.x;
   dy = camera.vp.y - camera.pr.y;
   dz = camera.vp.z - camera.pr.z;
   radius = sqrt(dx*dx + dy*dy + dz*dz);

   /* Determine the new view point */
   dd = radius * radians;
   newvp.x = vp.x + dd * ix * right.x + dd * iy * vu.x - camera.pr.x;
   newvp.y = vp.y + dd * ix * right.y + dd * iy * vu.y - camera.pr.y;
   newvp.z = vp.z + dd * ix * right.z + dd * iy * vu.z - camera.pr.z;
   Normalise(&newvp);
   camera.vp.x = camera.pr.x + radius * newvp.x;
   camera.vp.y = camera.pr.y + radius * newvp.y;
   camera.vp.z = camera.pr.z + radius * newvp.z;

   /* Determine the new right vector */
   newr.x = camera.vp.x + right.x - camera.pr.x;
   newr.y = camera.vp.y + right.y - camera.pr.y;
   newr.z = camera.vp.z + right.z - camera.pr.z;
   Normalise(&newr);
   newr.x = camera.pr.x + radius * newr.x - camera.vp.x;
   newr.y = camera.pr.y + radius * newr.y - camera.vp.y;
   newr.z = camera.pr.z + radius * newr.z - camera.vp.z;

   camera.vd.x = camera.pr.x - camera.vp.x;
   camera.vd.y = camera.pr.y - camera.vp.y;
   camera.vd.z = camera.pr.z - camera.vp.z;
   Normalise(&camera.vd);

   /* Determine the new up vector */
   CROSSPROD(newr,camera.vd,camera.vu);
   Normalise(&camera.vu);

   if (debug)
      fprintf(stderr,"Camera position: (%g,%g,%g)\n",
         camera.vp.x,camera.vp.y,camera.vp.z);
}

void TranslateCamera(int ix,int iy)
{
   XYZ vp,vu,vd;
   XYZ right;
   XYZ newvp,newr;
   double radians,delta;

   vu = camera.vu;
   Normalise(&vu);
   vp = camera.vp;
   vd = camera.vd;
   Normalise(&vd);
   CROSSPROD(vd,vu,right);
   Normalise(&right);
   radians = dtheta * PI / 180.0;
   delta = dtheta * camera.focallength / 90.0;

   camera.vp.x += iy * vu.x * delta;
   camera.vp.y += iy * vu.y * delta;
   camera.vp.z += iy * vu.z * delta;
   camera.pr.x += iy * vu.x * delta;
   camera.pr.y += iy * vu.y * delta;
   camera.pr.z += iy * vu.z * delta;

   camera.vp.x += ix * right.x * delta;
   camera.vp.y += ix * right.y * delta;
   camera.vp.z += ix * right.z * delta;
   camera.pr.x += ix * right.x * delta;
   camera.pr.y += ix * right.y * delta;
   camera.pr.z += ix * right.z * delta;
}

void init ( GLvoid )   
{
   glEnable(GL_DEPTH_TEST);
   glDisable(GL_LINE_SMOOTH);
   glDisable(GL_POINT_SMOOTH);
   glEnable(GL_POLYGON_SMOOTH); 
   glShadeModel(GL_SMOOTH);    
   glDisable(GL_DITHER);
   glDisable(GL_CULL_FACE);

	player = new Amoeba(50, 25, -50, 25,1, true);
	ai = new AI(100, 25, -50, 25,1, player, true);
	player->setupTarget( (Amoeba*) ai);
	sprites.push_back( (Sprite*) (player) );
	sprites.push_back( (Sprite*) ai );

	sprites.push_back( (Sprite*)(new Obstacle()) );
   glLineWidth(1.0);
   glPointSize(1.0);

   glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
   glFrontFace(GL_CW);
   glClearColor(0.0,0.0,0.0,0.0);         /* Background colour */
   glColorMaterial(GL_FRONT_AND_BACK,GL_AMBIENT_AND_DIFFUSE);
   glEnable(GL_COLOR_MATERIAL);
   glPixelStorei(GL_UNPACK_ALIGNMENT,1);
	
	

	ccamera.Position_Camera(-41, 41, 50,-27, 23, 32,   0, 1, 0);

}

static double amoebaVertices[5][24000][4];
static double AmoebaNormals[5][24000][4];
int numSprites;
int nVertices[10];

/*
   Create the geometry for the pulsar
*/
void MakeGeometry(void)
{
   GLfloat specular[4] = {1.0,1.0,1.0,1.0};
   GLfloat shiny[1] = {shinyVar};
 
   glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,specular);
   glMaterialfv(GL_FRONT_AND_BACK,GL_SHININESS,shiny);

	Draw_Skybox(0,0,0,1000,1000,1000);
	glColor4f(0.0,0.0,0.0,1.0);
	Draw_Grid(-1000,1000);
	glColor4f(0.0,0.0,1.0,1.0);

	glBegin(GL_TRIANGLES);
	for( int j = 0; j < numSprites; j++){
		for( int i = 0; i < nVertices[j]; i++){
			glVertex4d( amoebaVertices[j][i][0], amoebaVertices[j][i][1], amoebaVertices[j][i][2], amoebaVertices[j][i][3]); 
			glNormal3d( AmoebaNormals[j][i][0], AmoebaNormals[j][i][1], AmoebaNormals[j][i][2]);
		}
	}
	glEnd();


}

void display ( void )   
{
	printf("(%f, %f, %f):: (%f, %f, %f)\n", ccamera.mPos.x, ccamera.mPos.y, ccamera.mPos.z,  ccamera.mView.x, ccamera.mView.y, ccamera.mView.z); 
	numSprites = 0;

	for( std::list<Sprite*>::iterator it = sprites.begin(); it != sprites.end(); it++)
	{

		for( std::list<Sprite*>::iterator it2 = sprites.begin(); it2 != sprites.end(); it2++)
		{
			
			if(it != it2)
			{
				(*it)->collision(*it2);
			}
		}

		
		nVertices[numSprites] = (*it)->draw(amoebaVertices[numSprites], AmoebaNormals[numSprites]);
		numSprites++;		
		(*it)->update();
	}

	
	FPS++;
	currentTime = clock();
	if( currentTime - lastTime >= CLOCKS_PER_SEC){
		//printf("FPS = %d\n", FPS);
		lastTime = currentTime;
		FPS = 0;
	}

   if (stereo) 
   {
	   double dist,ratio,radians,scale,wd2,ndfl;
	   double left,right,top,bottom,near1=0.1,far1=10000;

	   near1 = camera.focallength / 5;

	   /* Misc stuff */
	   ratio  = camera.screenwidth / (double)camera.screenheight;
	   radians = DTOR * camera.aperture / 2;
	   wd2     = near1 * tan(radians);
	   ndfl    = near1 / camera.focallength;

   
		XYZ r;
	   
	   
	   

      /* Derive the two eye positions */
      CROSSPROD(camera.vd,camera.vu,r);
      Normalise(&r);
      r.x *= camera.eyesep / 2.0;
      r.y *= camera.eyesep / 2.0;
      r.z *= camera.eyesep / 2.0;

      glMatrixMode(GL_PROJECTION);
      glLoadIdentity();
      left  = - ratio * wd2 - 0.5 * camera.eyesep * ndfl;
      right =   ratio * wd2 - 0.5 * camera.eyesep * ndfl;
      top    =   wd2;
      bottom = - wd2;
      glFrustum(left,right,bottom,top,near1,far1);

      glMatrixMode(GL_MODELVIEW);
      glDrawBuffer(GL_BACK_RIGHT);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
      glLoadIdentity();
      gluLookAt(camera.vp.x + r.x,camera.vp.y + r.y,camera.vp.z + r.z,
                camera.vp.x + r.x + camera.vd.x,
                camera.vp.y + r.y + camera.vd.y,
                camera.vp.z + r.z + camera.vd.z,
                camera.vu.x,camera.vu.y,camera.vu.z);
      Lighting();
      MakeGeometry();

      glMatrixMode(GL_PROJECTION);
      glLoadIdentity();
      left  = - ratio * wd2 + 0.5 * camera.eyesep * ndfl;
      right =   ratio * wd2 + 0.5 * camera.eyesep * ndfl;
      top    =   wd2;
      bottom = - wd2;
      glFrustum(left,right,bottom,top,near1,far1);

      glMatrixMode(GL_MODELVIEW);
      glDrawBuffer(GL_BACK_LEFT);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
      glLoadIdentity();
      gluLookAt(camera.vp.x - r.x,camera.vp.y - r.y,camera.vp.z - r.z,
                camera.vp.x - r.x + camera.vd.x,
                camera.vp.y - r.y + camera.vd.y,
                camera.vp.z - r.z + camera.vd.z,
                camera.vu.x,camera.vu.y,camera.vu.z);
      Lighting();
      MakeGeometry();

   } 
   else 
   {
      glLoadIdentity();
	  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
      glLoadIdentity();
	  gluLookAt(ccamera.mPos.x,  ccamera.mPos.y,  ccamera.mPos.z, ccamera.mView.x, ccamera.mView.y, ccamera.mView.z, ccamera.mUp.x,  ccamera.mUp.y,   ccamera.mUp.z);
      MakeGeometry();
	  Lighting();
   }


	rotateangle += rotatespeed;


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

	gluPerspective(45.0f,(GLfloat)w/(GLfloat)h,0.1f,2000.0f);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void mouse(int btn, int state, int x, int y)
{
    if(btn==GLUT_LEFT_BUTTON && state==GLUT_DOWN)
    {
		//player->setLeftMousePos(x,screenTop -y);
		//player->extendAttackArm();
    }

    if(btn==GLUT_RIGHT_BUTTON && state==GLUT_DOWN)
    {
		//player->setRightMousePos(x, screenTop - y);
		//player->extendDefendArm();
    }
}

void HandleMainMenu(int row)
{

   switch (row) 
   {
   case 9: 
      exit(0); 
      break;
   }
}

void HandleAmbientLight(int row)
{
	switch (row) 
   {
	   case 1: 
			alr += lightVal;
		  break;

	   case 2: 
			alg += lightVal;
		  break;

		case 3: 
		alb += lightVal;
		break;

		 case 4: 
			alr -= lightVal;
		  break;

	   case 5: 
			alg -= lightVal;
		  break;

		case 6: 
		alb -= lightVal;
		break;

		case 7:
			alr = 0.2;
			alg = 0.2;
			alb = 0.2;
   }

}

void HandleDiffuseLight(int row)
{
	switch (row) 
   {
	   case 1: 
			dlr += lightVal;
		  break;

	   case 2: 
			dlg += lightVal;
		  break;

		case 3: 
		dlb += 0.1;
		break;

		 case 4: 
			dlr -= lightVal;
		  break;

	   case 5: 
			dlg -= lightVal;
		  break;

		case 6: 
			dlb -= lightVal;
		break;

		case 7: 
			dlr = 1.0;
			dlg = 1.0;
			dlb = 1.0;
		break;
   }
}

void HandleSpecularLight(int row)
{
	switch (row) 
   {
	   case 1: 
			slr += lightVal;
		  break;

	   case 2: 
			slg += lightVal;
		  break;

		case 3: 
		slb += lightVal;
		break;

		 case 4: 
			slr -= lightVal;
		  break;

	   case 5: 
			slg -= lightVal;
		  break;

		case 6: 
			slb -= lightVal;
		break;

		case 7: 
			slr= 0.0;
			slb = 0.0;
			slg = 0.0;
		break;
   }
}

void HandleShiny(int row)
{
		switch (row) 
		{
			case 1: 
				shinyVar += 100.0;
			  break;

		   case 2: 
				shinyVar -= 100.0;
			  break;

			case 3: 
				shinyVar = 5.0;
			break;
		}

}

void HandleLightPosition(int row)
{
	switch (row) 
		{
			case 1: 
				puts("Enter your Light's X Position:");
				scanf("%f", &lx);
				puts("Enter your Light's Y Position:");
				scanf("%f", &ly);
				puts("Enter your Light's Z Position:");
				scanf("%f", &lz);
				printf("The Lights current position is [%f, %f, %f]", lx, ly, lz);
			  break;

		   case 2: 
				lx++;
			  break;

			case 3: 
				ly++;
			break;

			case 4: 
				lz++;
			break;

			case 5: 
				lx--;
			break;

			case 6: 
				ly--;
			break;

			case 7: 
				lz--;
			break;


			case 8: 
				lx = 0.0;
				ly = 0.0;
				lz = 0.0;
			break;
		}
}

void HandleLightMenu(int row)
{
	switch(row)
	{
	  case 1: 
			lightVal += 0.1;
		  break;

	   case 2: 
			lightVal -= 0.1;
		  break;

	   case 3: 
			lightVal = 0.5;
		  break;
	}
}

void HandleTextureMenu(int row)
{
	switch(row)
	{
	  case 1: 
			tr += 0.5;
		  break;

	   case 2: 
			tg += 0.5;
		  break;

	   case 3: 
			tb += 0.5;
		  break;

		  case 4: 
			tr -= 0.5;
		  break;

	   case 5: 
			tg -= 0.5;
		  break;

	   case 6: 
			tb -= 0.5;
		  break;

		  case 7: 
			tr = 1.0;
			tg = 1.0;
			tb = 1.0;
		  break;
	}
}

void keyboard ( unsigned char key, int x, int y )
{
	if( stereo){
		switch(key){
			case 'H':
				CameraHome(0);
				break;
			case '[':                           /* Roll anti clockwise */
				RotateCamera(0,-1,0);
				break;
			case ']':                           /* Roll clockwise */
				RotateCamera(0,1,0);
				break;
			case 'q':                           /* Roll anti clockwise */
				RotateCamera(-1,0,0);
				break;
			case 'e':                           /* Roll clockwise */
				RotateCamera(1,0,0);
				break;

			case 'w':                           /* Translate camera up */
			case 'W':
				TranslateCamera(0,1);
				break;
			case 's':                           /* Translate camera down */
			case 'S':
				TranslateCamera(0,-1);
				break;
			case 'a':                           /* Translate camera left */
			case 'A':
				TranslateCamera(-1,0);
				break;
			case 'd':                           /* Translate camera right */
			case 'D':
				TranslateCamera(1,0);
				break;

			case '1':
				camera.eyesep+=0.1;
				break;
			case '2':
				camera.eyesep-=0.1;
				break;
		}
	}else{
		switch(key){
			case('w'):
				ccamera.Move_Camera(CAMERASPEED);
				break;

			case('s'):
				ccamera.Move_Camera(-CAMERASPEED);
				break;
			case('a'):
				ccamera.Rotate_View(0,-CAMERASPEED, 0);
				break;

			case('d'):
				ccamera.Rotate_View(0, CAMERASPEED, 0);
				break;

			case('e'):
				ccamera.Fly(CAMERASPEED);
				break;

			case('q'):
				ccamera.Fly(-CAMERASPEED);
				break;

			case('r'):
				ccamera.Look(CAMERASPEED);
				break;

			case('	')://tab
				ccamera.Look(-CAMERASPEED);
				break;
		}
	}

	switch ( key ) 
	{ 
		case(27):
			exit(0);
			break;
		case(' '):
			player->setVelocity(0,0,0);
			break;

		case('<'):
			player->extendAttackArm();
			break;
		case('>'):
			player->extendDefendArm();
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
				player->setVelz( 5.0f);
			break;
		case GLUT_KEY_DOWN:
			if( moves[1] )
				player->setVelz( -5.0f);
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
	
	int i;

	camera.screenwidth = 400;
	camera.screenheight = 300;

	/* Parse the command line arguments */
	for (i=1;i<argc;i++) {
		if (strstr(argv[i],"-h") != NULL) 
			GiveUsage(argv[0]);
		if (strstr(argv[i],"-f") != NULL)
			fullscreen = TRUE;
		if (strstr(argv[i],"-s") != NULL)
			stereo = TRUE;
		if (strstr(argv[i],"-d") != NULL)
			debug = TRUE;
		if (strstr(argv[i],"-c") != NULL)
			showconstruct = TRUE;
	}

	/* Set things up and go */
	glutInit(&argc,argv);
	if (!stereo)
		glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	else
		glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH | GLUT_STEREO);

	glutCreateWindow("Amoeba Boxing");
	glutReshapeWindow(500, 500);
	if (fullscreen)
		glutFullScreen();
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboard);
	glutSpecialFunc(arrow_keys);
	glutMouseFunc(mouse);
	init();
	CameraHome(0);

	loadBMP_custom("cubemap1.bmp");
    loadBMP_custom("cubemap2.bmp");
	loadBMP_custom("cubemap3.bmp");
	loadBMP_custom("cubemap4.bmp");
	loadBMP_custom("cubemap5.bmp");
	loadBMP_custom("cubemap6.bmp");

	for(int i = 0; i<numTex; i++)
		printf("%d\n", tex[i].textureID);

	/*
	

   /* Set up the main menu */
  
	int AmbientLightMenu, DiffuseLightMenu, SpecularLightMenu, ShinyMenu, lightPos,textureMenu, LightMenu, mainmenu;
	
   AmbientLightMenu = glutCreateMenu(HandleAmbientLight);
   glutAddMenuEntry("Increase Ambient Red",1);
   glutAddMenuEntry("Increase Ambient Green",2);
   glutAddMenuEntry("Increase Ambient Blue",3);
   glutAddMenuEntry("Decrease Ambient Red",4);
   glutAddMenuEntry("Decrease Ambient Green",5);
   glutAddMenuEntry("Decrease Ambient Blue",6);
   glutAddMenuEntry("Reset Ambient",7);

   DiffuseLightMenu = glutCreateMenu(HandleDiffuseLight);
   glutAddMenuEntry("Increase Diffuse Red",1);
   glutAddMenuEntry("Increase Diffuse Green",2);
   glutAddMenuEntry("Increase Diffuse Blue",3);
   glutAddMenuEntry("Decrease Diffuse Red",4);
   glutAddMenuEntry("Decrease Diffuse Green",5);
   glutAddMenuEntry("Decrease Diffuse Blue",6);
   glutAddMenuEntry("Reset Diffuse",7);

   SpecularLightMenu = glutCreateMenu(HandleSpecularLight);
   glutAddMenuEntry("Increase Specular Red",1);
   glutAddMenuEntry("Increase Specular Green",2);
   glutAddMenuEntry("Increase Specular Blue",3);
   glutAddMenuEntry("Decrease Specular Red",4);
   glutAddMenuEntry("Decrease Specular Green",5);
   glutAddMenuEntry("Decrease Specular Blue",6);
   glutAddMenuEntry("Reset Specular",7);

   ShinyMenu = glutCreateMenu(HandleShiny);
   glutAddMenuEntry("Increase shiniest",1);
   glutAddMenuEntry("Decrease shiniest", 2);
   glutAddMenuEntry("Reset shiniest", 3);

   lightPos = glutCreateMenu(HandleLightPosition);
   glutAddMenuEntry("Set Light Position",1);
   glutAddMenuEntry("Increment X Position", 2);
   glutAddMenuEntry("Increment Y Position", 3);
   glutAddMenuEntry("Increment Z Position", 4);
   glutAddMenuEntry("Decrement X Position", 5);
   glutAddMenuEntry("Decrement Y Position", 6);
   glutAddMenuEntry("Decrement Z Position", 7);
   glutAddMenuEntry("Reset Light Position", 8);


   LightMenu = glutCreateMenu(HandleLightMenu);
   glutAddSubMenu("Ambient",AmbientLightMenu);
   glutAddSubMenu("Diffuse",DiffuseLightMenu);
   glutAddSubMenu("Specular",SpecularLightMenu);
   glutAddSubMenu("Shiny", ShinyMenu);
    glutAddSubMenu("Light Position", lightPos);
   glutAddMenuEntry("Increment Light Value",1);
   glutAddMenuEntry("Decrement Light Value",2);
   glutAddMenuEntry("Reset Light Value",3);

   textureMenu = glutCreateMenu(HandleTextureMenu);
   glutAddMenuEntry("Increment Red",1);
   glutAddMenuEntry("Increment Blue", 2);
   glutAddMenuEntry("Increment Green", 3);
   glutAddMenuEntry("Decrement Red", 4);
   glutAddMenuEntry("Decrement Blue", 5);
   glutAddMenuEntry("Decrement Green", 6);
   glutAddMenuEntry("Reset Texture Colour", 7);

   mainmenu = glutCreateMenu(HandleMainMenu);
   glutAddSubMenu("Lighting",LightMenu);
   glutAddSubMenu("Texture",textureMenu);
   glutAddMenuEntry("Quit",9);
   glutAttachMenu(GLUT_RIGHT_BUTTON);

	glutMainLoop();

	return 0;
}
