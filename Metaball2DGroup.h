#ifndef METABALL2DGROUP_H_
#define METABALL2DGROUP_H_

#include "Sprite.h"
#include "Metaball2D.h"
#include "GraphicState.h"
#include <list>
#include <vector>
#include <stack>

#define THRESHOLD 1.0f
#define GRID_SIZE 4
#define DEBUG 0
#define NUM_GRIDS 500 / GRID_SIZE
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

	double calculatePoint( double  x, double y, std::vector<MetaballDrawData> &data){
		double score = 0;
		for( int i = 0; i < data.size(); i++){
			score += data[i].ball.Equation(x, y);
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

	void calcTangent( double x, double y, double *tx, double *ty, std::vector<MetaballDrawData> &data){

		*tx = 0;
		*ty = 0;
		double tmpx, tmpy;

		for( int i = 0; i < data.size(); i++){
			data[i].ball.calcTangent(x, y, &tmpx, &tmpy);
			*tx += tmpx;
			*ty += tmpy;
		}
		
	}

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

	void calcNormal( double x, double y, double *tx, double *ty, std::vector<MetaballDrawData> &data){
		*tx = 0;
		*ty = 0; 

		double tmpx, tmpy;

		for( int i = 0; i < data.size(); i++){
			data[i].ball.calcNormal(x, y, &tmpx, &tmpy);
			*tx += tmpx;
			*ty += tmpy;
		}

	}
	
	std::pair<double,double> stepOnceTowardsBorder(std::pair<double,double> start, double force, std::vector<MetaballDrawData> &data){
		std::pair<double, double> norm;
		calcNormal( start.first, start.second, &norm.first, &norm.second, data);

		double mag = sqrt( norm.second * norm.second + norm.first * norm.first);
		if( abs(mag) > 0.0001){
			norm.first /= mag;
			norm.second /= mag;
		}

		if( abs(norm.first) < 0.0001 && abs(norm.second) < 0.0001){
			norm.first = 0.0001;
			norm.second = 0.0001;
		}

		double stepSize = minRadius / THRESHOLD - ( minRadius / force) + 0.01;
		norm.first *= stepSize;
		norm.second *= stepSize;
		
		norm.first += start.first;
		norm.second += start.second;

		return norm;
	}

	std::pair<double, double> findBorder( std::pair<double,double> start, std::vector<MetaballDrawData> &data){
		double force;
		while( ( force = calculatePoint( start.first, start.second, data) ) > THRESHOLD){
			start = stepOnceTowardsBorder( start, force, data);
		}
		return start;
	}

	std::pair<double, double> traceBorder( std::pair<double,double> start, double stepSize, std::vector<MetaballDrawData> &data){
		std::pair<double, double> res;
		calcTangent( start.first, start.second, &res.first, &res.second, data);

		double mag = sqrt( res.first * res.first+ res.second * res.second);
		if( mag > 0.0001){
			res.first /= mag;
			res.second /= mag;
		}

		res.first *= stepSize / 2.0;
		res.second *= stepSize / 2.0;
		calcTangent( start.first + res.first, start.second + res.second, &res.first, &res.second, data);

		mag = sqrt( res.first * res.first+ res.second * res.second);
		if( mag > 0.0001){
			res.first /= mag;
			res.second /= mag;
		}

		res.first *= stepSize;
		res.second *= stepSize;
		res.first = start.first + res.first;
		res.second = start.second + res.second;
		res = stepOnceTowardsBorder(res, calculatePoint(res.first, res.second, data), data );
		return res;
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

	void draw(){
		int ballsCleared = 0;
		std::vector<MetaballDrawData> ballData = getDrawData();
		std::pair<double, double> start;
		std::pair<double, double> lastPoint;
		std::pair<double, double> nextPoint;
		minRadius = findSmallestRadius(ballData);
		double min = 5000.0;
		double min2 = min;
		double tmp;
		int closest;
		int closest2nd;
		
		for( int i=0; i<ballData.size(); i++){
			if( ballData[i].cleared ){
				continue;
			}

			start = findBorder( std::pair<double,double>(ballData[i].ball.getPx(), ballData[i].ball.getPy()), ballData);
			lastPoint = start;
			for( int j =0; j < 500; j++){
				nextPoint = traceBorder( lastPoint, 5, ballData);
				min = 5000.0;
				min2 = min;
				closest = 0;
				closest2nd = -1;
				for( int k=0; k < ballData.size(); k++){
					if( min > (tmp = ( nextPoint.first - ballData[k].ball.getPx()) * ( nextPoint.first - ballData[k].ball.getPx()) + ( nextPoint.second - ballData[k].ball.getPy()) * ( nextPoint.second - ballData[k].ball.getPy()) )){
						min = tmp;
						closest = k;
					}
				}
				ballData[closest].cleared = true;
				
				if( closest != i){
					glBegin( GL_TRIANGLES);

					if( lastPoint.first * ballData[i].groupBall.getPy() - lastPoint.second * ballData[i].groupBall.getPx() + lastPoint.second * nextPoint.first
						- lastPoint.first * nextPoint.second + ballData[i].groupBall.getPx() * nextPoint.second - nextPoint.first * ballData[i].groupBall.getPy() > 0.00001){
					
						glVertex2d( lastPoint.first, lastPoint.second);
						glVertex2d( nextPoint.first, nextPoint.second);
						glVertex2d( ballData[i].groupBall.getPx(), ballData[i].groupBall.getPy());

					} else{
						glVertex2d( nextPoint.first, nextPoint.second);
						glVertex2d( lastPoint.first, lastPoint.second);
						glVertex2d( ballData[i].groupBall.getPx(), ballData[i].groupBall.getPy());
					}
					glEnd();
					
				}

				glColor3f(ballData[closest].r,ballData[closest].g, ballData[closest].b);
				glBegin( GL_TRIANGLES);

				if( lastPoint.first * ballData[closest].groupBall.getPy() - lastPoint.second * ballData[closest].groupBall.getPx() + lastPoint.second * nextPoint.first
					- lastPoint.first * nextPoint.second + ballData[closest].groupBall.getPx() * nextPoint.second - nextPoint.first * ballData[closest].groupBall.getPy() > 0.00001){
					
					glVertex2d( lastPoint.first, lastPoint.second);
					glVertex2d( nextPoint.first, nextPoint.second);
					glVertex2d( ballData[closest].groupBall.getPx(), ballData[closest].groupBall.getPy());

				} else{
					glVertex2d( nextPoint.first, nextPoint.second);
					glVertex2d( lastPoint.first, lastPoint.second);
					glVertex2d( ballData[closest].groupBall.getPx(), ballData[closest].groupBall.getPy());
				}
				glEnd();

				tmp =  (start.first - nextPoint.first) * (start.first - nextPoint.first) + (start.second - nextPoint.second) * (start.second - nextPoint.second);
				if( tmp < 20){

					glBegin( GL_TRIANGLES);
					if( start.first * ballData[closest].groupBall.getPy() - start.second * ballData[closest].groupBall.getPx() + start.second * nextPoint.first
						- start.first * nextPoint.second + ballData[closest].groupBall.getPx() * nextPoint.second - nextPoint.first * ballData[closest].groupBall.getPy() > 0.00001){

						glVertex2d( nextPoint.first, nextPoint.second);		
						glVertex2d( start.first, start.second);
						glVertex2d( ballData[closest].groupBall.getPx(), ballData[closest].groupBall.getPy());

					} else{
						glVertex2d( start.first, start.second);
						glVertex2d( nextPoint.first, nextPoint.second);
						glVertex2d( ballData[closest].groupBall.getPx(), ballData[closest].groupBall.getPy());
					}
					glEnd();
					
					break;
				}

				lastPoint = nextPoint;
			}
			
		}
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

		int sx = x + GRID_SIZE / 2;
		int sy = y + GRID_SIZE / 2;
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