#ifndef METABALL3DGROUP_H_
#define METABALL3DGROUP_H_

#include "Sprite.h"
#include "Metaball3D.h"
#include "GraphicState.h"
#include <list>
#include <vector>
#include <stack>

#define THRESHOLD 1.0f
#define DEBUG 0
#define NUM_GRIDS 20
#define SQRT2 1.41421356237
#define PI 3.14159265359

typedef struct{
	Metaball3D ball;

	Metaball3D groupBall;
	double r,g,b;
	bool cleared;
}MetaballDrawData;

class Metaball3DGroup
{

private:
	double px,py,pz;//point x,y and z
	std::list<Metaball3D>balls;
	std::list<Metaball3DGroup*>subgroups;
	std::list<triangle>blocks[16];
	bool discovered [NUM_GRIDS][NUM_GRIDS];
	double values[NUM_GRIDS + 1][NUM_GRIDS + 1];

	double r,g,b;
	int moves[16];
	double minRadius;
	double gridSize;
	const static int triTable[256][16]; 

public:
	
	Metaball3DGroup(double red, double green, double blue){

		r = red; 
		g = green;
		b = blue;
		 
		minRadius = 1000;

	}

	Metaball3DGroup(){ };

	void addMetaball(Metaball3D* ball)
	{
		balls.push_back(*ball);
	}

	void popMetaball()
	{
		balls.pop_back();
	}
	
	void addSubgroup( Metaball3DGroup *other)
	{
		subgroups.push_back(other);
	}
	
	void getRGB( int *r, int *g, int *b){
		*r = this->r;
		*g = this->g;
		*b = this->b;
	}

	std::vector<MetaballDrawData> getDrawData(){
		std::vector<MetaballDrawData> ballData;

		for( std::list<Metaball3DGroup*>::iterator it = subgroups.begin(); it != subgroups.end(); it++){
			std::vector<MetaballDrawData> other = (*it)->getDrawData();
			for( int i=0; i < other.size(); i++){
				ballData.push_back( other[i]);
			}
		}
		
		MetaballDrawData data;
		for( std::list<Metaball3D>::iterator it = balls.begin(); it != balls.end(); it++){
			data.ball = *it;
			data.groupBall = balls.front();
			data.r = r;
			data.g = g;
			data.b = b;
			data.cleared = false;
			ballData.push_back(data);
		}
		return ballData;
	}

	void popSubgroup()
	{
		if( subgroups.size() >= 1){
			subgroups.pop_back();
		}
	}

	double evaluatePoint(double x, double y){
		double score =0;
		for( std::list<Metaball3D>::iterator it = balls.begin(); it != balls.end(); it++){
			score += it->Equation(x,y);
			if( score >= THRESHOLD){
				return score;
			}
		}
		return score;
	}

	double calculatePoint( double  x, double y, double z, std::vector<MetaballDrawData> &data){
		double score = 0;
		for( int i = 0; i < data.size(); i++){
			score += data[i].ball.Equation(x, y, z);
		}
		return score;
	}

	double evaluateSubGroups( int x, int y){
		double score = 0;
		for( std::list<Metaball3DGroup*>::iterator it = subgroups.begin(); it != subgroups.end(); it++){
			score += (*it)->evaluatePoint(x, y);
			if( score >= THRESHOLD )
				return score;
		}
		return score;
	}

/*	void calcTangent( double x, double y, double *tx, double *ty, std::vector<MetaballDrawData> &data){

		*tx = 0;
		*ty = 0;
		double tmpx, tmpy;

		for( int i = 0; i < data.size(); i++){
			data[i].ball.calcTangent(x, y, &tmpx, &tmpy);
			*tx += tmpx;
			*ty += tmpy;
		}
		
	}
*/
	double findSmallestRadius( std::vector<MetaballDrawData> &data){
		double min = 10000.0;
		double tmp;

		for( int i = 0; i < data.size(); i++){
			if( min > ( tmp = data[i].ball.getRadius() ) ){
				min = tmp;
			}
		}

		return min;
	}

	void calcNormal( double x, double y, double z, double *tx, double *ty, double *tz, std::vector<MetaballDrawData> &data){
		*tx = 0;
		*ty = 0; 
		*tz = 0;

		double tmpx, tmpy,tmpz;

		for( int i = 0; i < data.size(); i++){
			data[i].ball.calcNormal(x, y, z, &tmpx, &tmpy, &tmpz);
			*tx += tmpx;
			*ty += tmpy;
			*tz += tmpz;
		}
		double scale = *tx * *tx + *ty * *ty + *tz * *tz;
		*tx /= scale;
		*ty /= scale;
		*tz /= scale;
	}
	
	void drawCircle( double x, double y, double radius){
		glBegin( GL_TRIANGLES);
		double newX;
		double newY;
		double lastX = radius + x;
		double lastY = y;

		for( double i = 1.0; i <= 360; i++){
			newX = cos( i * PI / 180.0) * radius + x;
			newY = sin( i * PI / 180.0) * radius + y;

			glVertex2d( lastX, lastY);
			glVertex2d( newX, newY);
			glVertex2d( x, y);
		}
		glEnd();
	}
 
	void decreaseRadius( double val){
		for( std::list<Metaball3D>::iterator it = balls.begin(); it != balls.end(); it++){
			if( it->getRadius() < val){
				it->setRadius(5.0);
			}else {
				it->setRadius( it->getRadius() - val);
			}
		}
	}

	void setRadius(double val){
		for(std::list<Metaball3D>::iterator it = balls.begin(); it != balls.end(); it++){
			it->setRadius(val);
		}
	}

	void getBoundaries( double *left, double *right, double *bottom, double *top, double *front, double *back, std::vector<MetaballDrawData> ballData){
		*left = ballData[0].ball.getPx() - ballData[0].ball.getRadius();
		*right = ballData[0].ball.getPx() + ballData[0].ball.getRadius();
		*bottom = ballData[0].ball.getPy() - ballData[0].ball.getRadius();
		*top = ballData[0].ball.getPy() + ballData[0].ball.getRadius();
		*front = ballData[0].ball.getPz() - ballData[0].ball.getRadius();
		*back = ballData[0].ball.getPz() + ballData[0].ball.getRadius();
		
		double temp;
		for( unsigned int i = 0; i < ballData.size(); i++){
			temp = ballData[0].ball.getPx() - ballData[0].ball.getRadius();
			if( *left > temp){
				*left = temp;
			}
			temp= ballData[0].ball.getPx() + ballData[0].ball.getRadius();
			if( *right < temp){
				*right = temp;
			}
			temp = ballData[0].ball.getPy() - ballData[0].ball.getRadius();
			if( *bottom > temp){
				*bottom = temp;
			}
			temp = ballData[0].ball.getPy() + ballData[0].ball.getRadius();
			if( *top < temp){
				*top = temp;
			}
			temp = ballData[0].ball.getPz() - ballData[0].ball.getRadius();
			if( *front < temp){
				*front = temp;
			}
			temp = ballData[0].ball.getPz() + ballData[0].ball.getRadius();
			if( *back > temp){
				*back = temp;
			}
		}
	}
int calcCorners( int x, int y, int z, double cornerVertices[NUM_GRIDS + 1][NUM_GRIDS + 1][NUM_GRIDS + 1][4]){
int lookup = 0;
// 7 -- x, y, z
if (cornerVertices[x][y][z][3] >= THRESHOLD) lookup |= 128;
// 6 -- (x + 1), y, z
if ( cornerVertices[x + 1][y][z][3] >= THRESHOLD) lookup |= 64;
// 2 -- (x + 1), (y + 1), z
if (cornerVertices[x + 1][y + 1][z][3] >= THRESHOLD) lookup |= 4;
// 3 -- x, (y + 1), z
if (cornerVertices[x][y + 1][z][3] >= THRESHOLD) lookup |= 8;
// 4 -- x, y, (z + 1)
if (cornerVertices[x][y][z + 1][3] >= THRESHOLD) lookup |= 16;
// 5 -- (x + 1), y, (z + 1)
if (cornerVertices[x + 1][y][z + 1][3] >= THRESHOLD) lookup |= 32;
// 1 -- (x + 1), (y + 1), (z + 1)
if (cornerVertices[x + 1][y + 1][z + 1][3] >= THRESHOLD) lookup |= 2;
// 0 -- x + (y + 1), (z + 1)
if (cornerVertices[x][y + 1][z + 1][3] >= THRESHOLD) lookup |= 1;

return lookup;
}
	
void interpolatePoint( double vertex[3], double vertexNormal[3], int vertex0[3], int vertex1[3], int depth, double cornerVertices[NUM_GRIDS + 1][NUM_GRIDS + 1][NUM_GRIDS + 1][4], double cornerNormals[NUM_GRIDS + 1][NUM_GRIDS + 1][NUM_GRIDS + 1][3]){
double diff;


diff = (THRESHOLD - cornerVertices[vertex0[0]][vertex0[1]][vertex0[2]][3]) / (cornerVertices[vertex1[0]][vertex1[1]][vertex1[2]][3] - cornerVertices[vertex0[0]][vertex0[1]][vertex0[2]][3]);

double x1 = cornerVertices[vertex0[0]][vertex0[1]][vertex0[2]][0];
double y1 = cornerVertices[vertex0[0]][vertex0[1]][vertex0[2]][1];
double z1 = cornerVertices[vertex0[0]][vertex0[1]][vertex0[2]][2];
double x2 = cornerVertices[vertex1[0]][vertex1[1]][vertex1[2]][0];
double y2 = cornerVertices[vertex1[0]][vertex1[1]][vertex1[2]][1];
double z2 = cornerVertices[vertex1[0]][vertex1[1]][vertex1[2]][2];

vertex[0] = x1 + (x2 - x1) * diff;
vertex[1] = y1 + (y2 - y1) * diff;
vertex[2] = z1 + (z2 - z1) * diff;

double nx1 = cornerNormals[vertex0[0]][vertex0[1]][vertex0[2]][0];
double ny1 = cornerNormals[vertex0[0]][vertex0[1]][vertex0[2]][1];
double nz1 = cornerNormals[vertex0[0]][vertex0[1]][vertex0[2]][2];
double nx2 = cornerNormals[vertex1[0]][vertex1[1]][vertex1[2]][0];
double ny2 = cornerNormals[vertex1[0]][vertex1[1]][vertex1[2]][1];
double nz2 = cornerNormals[vertex1[0]][vertex1[1]][vertex1[2]][2];

vertexNormal[0]= nx1 + (nx2 - nx1) * diff;
vertexNormal[1]= ny1 + (ny2 - ny1) * diff;
vertexNormal[2] = nz1 + (nz2 - nz1) * diff;
}
	
void interpolatePoint( double vertex[3], double normal[3], int ex1, int ey1, int ez1, int ex2, int ey2, int ez2, int depth, double cornerVertices[NUM_GRIDS + 1][NUM_GRIDS + 1][NUM_GRIDS + 1][4], double cornerNormals[NUM_GRIDS + 1][NUM_GRIDS + 1][NUM_GRIDS + 1][3]){
int edge1[] = {ex1, ey1, ez1};
int edge2[] = {ex2, ey2, ez2};
interpolatePoint(vertex, normal, edge1, edge2, depth, cornerVertices, cornerNormals);
}


	void calcVertexes( double vertexes[16][3], double normals[16][3], int x, int y, int z, double cornerVertices[NUM_GRIDS + 1][NUM_GRIDS + 1][NUM_GRIDS + 1][4], double cornerNormals[NUM_GRIDS + 1][NUM_GRIDS + 1][NUM_GRIDS + 1][3]){
// 0 - 1
// x, (y + 1), (z + 1)
// (x + 1),(y + 1), (z + 1)
interpolatePoint(	vertexes[0], normals[0], x, y + 1, z + 1,
x + 1, y + 1, z + 1, 5, cornerVertices, cornerNormals);

// 1 - 2
// (x + 1), (y + 1), (z + 1)
// (x + 1), (y + 1), z
interpolatePoint( vertexes[1], normals[1],	x + 1, y + 1, z + 1,
x + 1, y +1, z, 5, cornerVertices, cornerNormals);

// 2 - 3
// (x + 1), (y + 1), z
// x, (y + 1), z
interpolatePoint(vertexes[2], normals[2], x + 1, y + 1, z,
x, y + 1, z, 5, cornerVertices, cornerNormals);

// 3 - 0
// x, (y + 1), z
// x, (y + 1), (z + 1)
interpolatePoint(	vertexes[3], normals[3], x, y + 1, z,
x, y + 1, z + 1, 5, cornerVertices, cornerNormals);

// 4 - 5
// x, y, (z + 1)
// (x + 1), y, (z + 1)
interpolatePoint(	vertexes[4], normals[4], x, y, z +1,
x + 1, y, z +1, 5, cornerVertices, cornerNormals);
// 5 - 6
// (x + 1), y, (z + 1)
// (x + 1), y, z
interpolatePoint( vertexes[5], normals[5],	x + 1, y, z + 1,
x + 1, y, z, 5, cornerVertices, cornerNormals);

// 6 - 7
// (x + 1), y, z
// x , y, z
interpolatePoint( vertexes[6], normals[6],	x + 1, y, z,
x, y, z, 5, cornerVertices, cornerNormals);

// 7 - 4
// x, y, z
// x, y, (z + 1)
interpolatePoint( vertexes[7], normals[7],	x, y, z,
x, y, z + 1, 5, cornerVertices, cornerNormals);

// 0 - 4
// x, (y + 1), (z + 1)
// x, y, (z + 1)
interpolatePoint(	vertexes[8], normals[8], x, y + 1, z + 1,
x, y, z + 1, 5, cornerVertices, cornerNormals);

// 1 - 5
// (x + 1), (y + 1), (z + 1)
// (x + 1), y, (z + 1)
interpolatePoint(vertexes[9], normals[9], x + 1, y + 1, z + 1,
x + 1, y, z + 1, 5, cornerVertices, cornerNormals);

// 2 - 6
// (x + 1), (y + 1), z
// (x + 1), y, z
interpolatePoint( vertexes[10], normals[10],	x + 1, y + 1, z,
x + 1, y, z, 5, cornerVertices, cornerNormals);

// 3 - 7
// x, (y + 1), z
// x, y, z
interpolatePoint(vertexes[11], normals[11],	x, y +1, z,
x, y, z, 5, cornerVertices, cornerNormals);
}

	int draw(double vertices[25000][4], double normals[25000][4]){
double cornerVertices[NUM_GRIDS + 1][NUM_GRIDS + 1][NUM_GRIDS + 1][4];
double cornerNormals[NUM_GRIDS + 1][NUM_GRIDS + 1][NUM_GRIDS + 1][3];

double vertexes[16][3];
double vertexNormals[16][3];
int vertexNum = 0;
double left, right, front, back, top, bottom;
double nx, ny, nz;
std::vector<MetaballDrawData> ballData = getDrawData();
getBoundaries(&left, &right, &bottom, &top, &front, &back, ballData);
double temp = min( right - left, back - front);
temp = min(temp, top - bottom);
gridSize = temp / (double)NUM_GRIDS;

for( int x = 0; x <= NUM_GRIDS; x ++){
for( int y = 0; y <= NUM_GRIDS; y++){
for( int z = 0; z <= NUM_GRIDS; z++){
cornerVertices[x][y][z][0] = x * gridSize + left;
cornerVertices[x][y][z][1] = y * gridSize + bottom;
cornerVertices[x][y][z][2] = z * gridSize + front;
cornerVertices[x][y][z][3] = calculatePoint(cornerVertices[x][y][z][0], cornerVertices[x][y][z][1], cornerVertices[x][y][z][2], ballData);
calcNormal(cornerVertices[x][y][z][0], cornerVertices[x][y][z][1], cornerVertices[x][y][z][2], &nx, &ny, &nz, ballData);
cornerNormals[x][y][z][0] = nx;
cornerNormals[x][y][z][1] = ny;
cornerNormals[x][y][z][2] = nz;
}
}
}

for( int x = 0; x < NUM_GRIDS; x ++){
for( int y = 0; y < NUM_GRIDS; y++){
for( int z = 0; z < NUM_GRIDS; z++){
int lookUp = calcCorners( x, y, z, cornerVertices);

calcVertexes(vertexes, vertexNormals, x, y, z ,cornerVertices, cornerNormals);
for( int i = 0; i < 16 && triTable[lookUp][i] != -1; i++){
int vPos = triTable[lookUp][i];
vertices[vertexNum][0] = vertexes[vPos][0];
vertices[vertexNum][1] = vertexes[vPos][1];
vertices[vertexNum][2] = vertexes[vPos][2];
vertices[vertexNum][3] = 1;
normals[vertexNum][0] = vertexNormals[vPos][0];
normals[vertexNum][1] = vertexNormals[vPos][1];
normals[vertexNum][2] = vertexNormals[vPos][2];
normals[vertexNum][3] = 1;

vertexNum++;
}
}
}
}

return vertexNum;
}


	void shiftGroup(double x, double y, double z)
	{
		for( std::list<Metaball3D>::iterator it = balls.begin(); it != balls.end(); it++)
		{
			it->shift(x, y, z);
		}

		for(std:: list<Metaball3DGroup*>:: iterator it = subgroups.begin(); it != subgroups.end(); it++)
		{
			(*it)->shiftGroup(x,y, z);
		}
	}

	inline bool checkSamples( int x, int y){
		double score;

		int sx = x + gridSize / 2;
		int sy = y + gridSize / 2;
		score = evaluatePoint(sx, sy);
		if( score >= 0.8){
			return true;
		} 
		score += evaluateSubGroups( sx, sy);
		if( score >= 0.8){
			return true;
		}

		return false;
	}
};

#endif