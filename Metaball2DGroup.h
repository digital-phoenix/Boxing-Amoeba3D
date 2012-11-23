#ifndef METABALL2DGROUP_H_
#define METABALL2DGROUP_H_

#include "Sprite.h"
#include "Metaball2D.h"
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
	Metaball2D ball;
	Metaball2D groupBall;
	double r,g,b;
	bool cleared;
}MetaballDrawData;

class Metaball2DGroup
{

private:
	double px,py;//point x and y
	std::list<Metaball2D>balls;
	std::list<Metaball2DGroup*>subgroups;
	std::list<triangle>blocks[16];
	bool discovered [NUM_GRIDS][NUM_GRIDS];
	double values[NUM_GRIDS + 1][NUM_GRIDS + 1];

	double r,g,b;
	int moves[16];
	double minRadius;
	double gridSize;
	const static int triTable[256][16]; 

public:
	
	Metaball2DGroup(double red, double green, double blue){

		r = red; 
		g = green;
		b = blue;
		 
		minRadius = 1000;

	}

	Metaball2DGroup(){ };

	void addMetaball(Metaball2D* ball)
	{
		balls.push_back(*ball);
	}

	void popMetaball()
	{
		balls.pop_back();
	}
	
	void addSubgroup( Metaball2DGroup *other)
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

		for( std::list<Metaball2DGroup*>::iterator it = subgroups.begin(); it != subgroups.end(); it++){
			std::vector<MetaballDrawData> other = (*it)->getDrawData();
			for( int i=0; i < other.size(); i++){
				ballData.push_back( other[i]);
			}
		}
		
		MetaballDrawData data;
		for( std::list<Metaball2D>::iterator it = balls.begin(); it != balls.end(); it++){
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
		for( std::list<Metaball2D>::iterator it = balls.begin(); it != balls.end(); it++){
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
		for( std::list<Metaball2DGroup*>::iterator it = subgroups.begin(); it != subgroups.end(); it++){
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
		for( std::list<Metaball2D>::iterator it = balls.begin(); it != balls.end(); it++){
			if( it->getRadius() < val){
				it->setRadius(5.0);
			}else {
				it->setRadius( it->getRadius() - val);
			}
		}
	}

	void setRadius(double val){
		for(std::list<Metaball2D>::iterator it = balls.begin(); it != balls.end(); it++){
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

	int calcCorners( double x, double y, double z, std::vector<MetaballDrawData> ballData){
		int lookup = 0;
		// 7 -- x, y, z
		if (calculatePoint(x,y,z, ballData) >= THRESHOLD) lookup |= 128;
		// 6 -- (x + 1), y,  z
		if (calculatePoint(x + gridSize,y,z, ballData) >= THRESHOLD) lookup |= 64;
		// 2 -- (x + 1), (y + 1), z
		if (calculatePoint(x + gridSize,y + gridSize,z, ballData) >= THRESHOLD) lookup |= 4;
		// 3 -- x, (y + 1), z
		if (calculatePoint(x,y + gridSize,z, ballData) >= THRESHOLD) lookup |= 8;
		// 4 -- x, y, (z + 1)
		if (calculatePoint(x,y,z + gridSize, ballData) >= THRESHOLD) lookup |= 16;
		// 5 -- (x + 1), y, (z + 1)
		if (calculatePoint(x + gridSize,y,z + gridSize, ballData) >= THRESHOLD) lookup |= 32;
		// 1 -- (x + 1), (y + 1), (z + 1) 
		if (calculatePoint(x + gridSize,y + gridSize,z + gridSize, ballData) >= THRESHOLD) lookup |= 2;
		// 0 -- x + (y + 1), (z + 1)
		if (calculatePoint(x,y + gridSize,z + gridSize, ballData) >= THRESHOLD) lookup |= 1;
		return lookup;
	}
	
	void interpolatePoint( double vertex[3], double in[3], double out[3], int depth, std::vector<MetaballDrawData> &ballData){
		if( depth < 0)
			return;
		for( int i =0; i < 3; i++)
			vertex[i] = in[i] + (out[i] - in[i])/ 2;
		double temp = calculatePoint( vertex[0], vertex[1], vertex[2], ballData);
		if( temp == THRESHOLD){
			return;
		}else if( temp < THRESHOLD){
			interpolatePoint(vertex, in, vertex, depth - 1, ballData);
		}else{
			interpolatePoint(vertex, vertex, out, depth - 1, ballData);
		}
	}

	void interpolatePoint( double vertex[3], double ex1, double ey1, double ez1, double ex2, double ey2, double ez2, int depth, std::vector<MetaballDrawData> &ballData){
		double edge1[] = {ex1, ey1, ez1};
		double edge2[] = {ex2, ey2, ez2};

		if( calculatePoint(ex1, ey1, ez1, ballData) >= THRESHOLD){
			interpolatePoint(vertex, edge1, edge2, depth, ballData);
		}else{
			interpolatePoint(vertex, edge2, edge1, depth, ballData);
		}
	}

	void calcVertexes( double vertexes[16][3], double x, double y, double z, std::vector<MetaballDrawData> &ballData){
		// 0 - 1
		// x, (y + 1), (z + 1)
		// (x + 1),(y + 1), (z + 1)
		interpolatePoint(	vertexes[0], x, y + gridSize, z + gridSize,
			x + gridSize, y + gridSize, z + gridSize, 5, ballData);

		// 1 - 2
		// (x + 1), (y + 1), (z + 1) 
		// (x + 1), (y + 1), z
		interpolatePoint( vertexes[1],	x + gridSize, y + gridSize, z + gridSize,
							x + gridSize, y +gridSize, z, 5, ballData);

		// 2 - 3
		// (x + 1), (y + 1), z 
		// x, (y + 1), z 
		interpolatePoint(vertexes[2],	x + gridSize, y + gridSize, z,
							x, y + gridSize, z, 5, ballData);

		// 3 - 0
		// x, (y + 1), z
		// x, (y + 1), (z + 1) 
		interpolatePoint(	vertexes[3], x, y + gridSize, z,
							x, y  + gridSize, z + gridSize, 5, ballData);
			
		// 4 - 5
		// x, y, (z + 1) 
		// (x + 1), y, (z + 1) 
		interpolatePoint(	vertexes[4], x, y, z +gridSize,
							x + gridSize, y, z +gridSize, 5, ballData);
		// 5 - 6
		// (x + 1), y, (z + 1)
		// (x + 1), y, z
		interpolatePoint( vertexes[5],	x + gridSize, y, z + gridSize,
							x + gridSize, y, z, 5, ballData);

		// 6 - 7
		// (x + 1), y, z 
		// x , y, z
		interpolatePoint( vertexes[6],	x + gridSize, y, z,
							x, y, z, 5, ballData);

		// 7 - 4
		// x, y, z
		// x, y, (z + 1)
		interpolatePoint( vertexes[7],	x, y, z,
							x, y, z + gridSize, 5, ballData);

		// 0 - 4
		// x, (y + 1), (z + 1)
		// x, y, (z + 1)
		interpolatePoint(	vertexes[8], x, y + gridSize, z + gridSize,
							x, y, z + gridSize, 5, ballData);

		// 1 - 5
		// (x + 1), (y + 1), (z + 1)
		// (x + 1), y, (z + 1)
		interpolatePoint(vertexes[9], x + gridSize, y + gridSize, z + gridSize,
							x + gridSize, y, z + gridSize, 5, ballData);

		// 2 - 6
		// (x + 1), (y + 1), z
		// (x + 1), y, z
		interpolatePoint( vertexes[10],	x + gridSize, y + gridSize, z,
							x + gridSize, y, z, 5, ballData);

		// 3 - 7
		// x, (y + 1), z
		// x, y, z
		interpolatePoint(vertexes[11],	x, y +gridSize, z,
							x, y, z, 5, ballData);
	}
	
	int draw(double vertices[25000][4], double normals[25000][4]){
		double vertexes[16][3];
		int vertexNum = 0;
		double left, right, front, back, top, bottom;
		double nx, ny, nz;
		std::vector<MetaballDrawData> ballData = getDrawData();
		getBoundaries(&left, &right, &bottom, &top, &front, &back, ballData);
		double temp = min( right - left, back - front);
		temp = min(temp, top - bottom);
		gridSize = temp / (double)NUM_GRIDS;
		for( double x = left - gridSize; x <= right; x += gridSize){
			for( double y = bottom - gridSize; y <= top; y += gridSize){
				for( double z = front - gridSize; z <= back; z += gridSize){
					int lookUp = calcCorners( x, y, z, ballData);

					calcVertexes(vertexes, x, y, z ,ballData); 
					for( int i = 0; i < 16 && triTable[lookUp][i] != -1; i++){
						vertices[vertexNum][0] = vertexes[triTable[lookUp][i]][0];
						vertices[vertexNum][1] = vertexes[triTable[lookUp][i]][1];
						vertices[vertexNum][2] = vertexes[triTable[lookUp][i]][2];
						vertices[vertexNum][3] = 1;
						calcNormal(vertices[vertexNum][0], vertices[vertexNum][1], vertices[vertexNum][2], &nx, &ny, &nz, ballData);
						normals[vertexNum][0] = nx;
						normals[vertexNum][1] = ny;
						normals[vertexNum][2] = nz;
						normals[vertexNum][3] = 1;

						vertexNum++;
					}
				}
			}
		}
		return vertexNum;
	}
	

	void shiftGroup(double x, double y)
	{
		for( std::list<Metaball2D>::iterator it = balls.begin(); it != balls.end(); it++)
		{
			it->shift(x, y);
		}

		for(std:: list<Metaball2DGroup*>:: iterator it = subgroups.begin(); it != subgroups.end(); it++)
		{
			(*it)->shiftGroup(x,y);
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