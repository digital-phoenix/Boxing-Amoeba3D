#include <GL/glut.h>
#include <time.h>
#include <stdio.h>
#include <math.h>
#include "CCamera.h"


#ifndef GL_BGR
#define GL_BGR 0x80E0
#endif

#define CAMERASPEED	0.03f// The Camera Speed

//angle of rotation
GLfloat angle = 0.0;
 
//diffuse light color variables
GLfloat dlr = 1.0;
GLfloat dlg = 1.0;
GLfloat dlb = 1.0;
 
//ambient light color variables
GLfloat alr = 1.0;
GLfloat alg = 1.0;
GLfloat alb = 1.0;
 
//light position variables
GLfloat lx = 0.0;
GLfloat ly = 0.0;
GLfloat lz = 1.0;
GLfloat lw = 0.0;

//Used to control framerate
clock_t deltaTime = 0.0;
clock_t currentTime = 0.0;
clock_t lastTime = clock();


//used for camera

int cameraX = 0;
int cameraY = 0;
int cameraZ = 0;

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



CCamera objCamera; 


//draw the cube
void cube (void) {
  glRotatef(angle, 1.0, 0.0, 0.0); //rotate on the x axis
  glRotatef(angle, 0.0, 1.0, 0.0); //rotate on the y axis
  glRotatef(angle, 0.0, 0.0, 1.0); //rotate on the z axis
  glutSolidCube(2); //draw the cube
}


void Draw_Grid()
{															

	for(float i = -500; i <= 500; i += 5)
	{
		glBegin(GL_LINES);
			glColor3ub(150, 190, 150);						
			glVertex3f(-500, 0, i);					
			glVertex3f(500, 0, i);
			glVertex3f(i, 0,-500);							
			glVertex3f(i, 0, 500);
		glEnd();
	}
}

 
void init (void) {
    glShadeModel(GL_SMOOTH);							// Enable Smooth Shading
	glClearColor(0.0f, 0.0f, 0.0f, 0.5f);				// Black Background
	glClearDepth(1.0f);									// Depth Buffer Setup
	glEnable(GL_DEPTH_TEST);							// Enables Depth Testing
	glDepthFunc(GL_LEQUAL);								// The Type Of Depth Testing To Do
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);	// Really Nice Perspective Calculations

	glEnable(GL_TEXTURE_2D);

				           // Position      View(target)  Up
	objCamera.Position_Camera(0, 2.5f, 5,	0, 2.5f, 0,   0, 1, 0);


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

void Draw_Skybox(float x, float y, float z, float width, float height, float length)
{
	// Center the Skybox around the given x,y,z position
	x = x - width  / 2;
	y = y - height / 2;
	z = z - length / 2;


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

	
	glTexImage2D(GL_TEXTURE_2D, 0,GL_RGB, tex[numTex].width, tex[numTex].height, 0, GL_BGR, GL_UNSIGNED_BYTE, tex[numTex].data);
	

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	
	

	numTex++;
}
 
void display (void) 
{

	 currentTime = clock();
     deltaTime = currentTime - lastTime;
        
        if (deltaTime < 20){
                return;
        }
 
        lastTime = currentTime;

    glClearColor (0.0,0.0,0.0,1.0); //clear the screen to black
    glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //clear the color buffer and the depth buffer

	glLoadIdentity();  

	gluLookAt(objCamera.mPos.x,  objCamera.mPos.y,  objCamera.mPos.z,	
			  objCamera.mView.x, objCamera.mView.y, objCamera.mView.z,	
			  objCamera.mUp.x,   objCamera.mUp.y,   objCamera.mUp.z);
		
	Draw_Grid();
	Draw_Skybox(0,0,0,100,100,100);
 
    
    glutSwapBuffers(); //swap the buffers
	

	


}


void keyboard (unsigned char key, int x, int y) {
    if (key=='r') { 
        dlr = 1.0; //change light to red
        dlg = 0.0;
        dlb = 0.0;
    }
    if (key=='g') { 
        dlr = 0.0; //change light to green
        dlg = 1.0;
        dlb = 0.0;
    }
    if (key=='b') { 
        dlr = 0.0; //change light to blue
        dlg = 0.0;
        dlb = 1.0;
    }
  /*  if (key=='w') { 
        ly += 10.0; //move the light up
    }
    if (key=='s') { 
        ly -= 10.0; //move the light down
    }
    if (key=='a') { 
        lx -= 10.0; //move the light left
    }
    if (key=='d') { 
        lx += 10.0; //move the light right
    }*/

	if(key == 'w')
	{	
		objCamera.Move_Camera(CAMERASPEED);
	}

	if(key == 's')
	{
		objCamera.Move_Camera(-CAMERASPEED);
	}

	if(key == 'a')
	{	
		objCamera.Rotate_View(0,-CAMERASPEED, 0);
	}

	if(key == 'd')
	{
		objCamera.Rotate_View(0, CAMERASPEED, 0);
	}
}
 
/*int main (int argc, char **argv) {
    glutInit (&argc, argv);
    glutInitDisplayMode (GLUT_DOUBLE | GLUT_DEPTH); //set the display to Double buffer, with depth
    glutInitWindowSize (500, 500); //set the window size
    glutInitWindowPosition (100, 100); //set the position of the window
    glutCreateWindow ("A basic OpenGL Window"); //the captionof the window
    init (); //call the init function
    glutDisplayFunc (display); //use the display function to draw everything
    glutIdleFunc (display); //update any variables in display,display can be changed to anyhing, as long as you move the variables to be updated, in this case, angle++;
    glutReshapeFunc (reshape); //reshape the window accordingly
 
    glutKeyboardFunc (keyboard); //check the keyboard
	loadBMP_custom("cubemap1.bmp");
	loadBMP_custom("cubemap2.bmp");
	loadBMP_custom("cubemap3.bmp");
	loadBMP_custom("cubemap4.bmp");
	loadBMP_custom("cubemap5.bmp");
	loadBMP_custom("cubemap6.bmp");

	for(int i = 0; i<numTex; i++)
		printf("%d", tex[i].textureID);
	
	
    glutMainLoop (); //call the main loop
	

    return 0;
}*/