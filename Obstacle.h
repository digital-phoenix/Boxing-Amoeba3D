#ifndef OBSTACLE_H_
#define OBSTACLE_H_

#include"Sprite.h"
#include "Metaball2DGroup.h"

class Obstacle : Sprite  {
	
	private:
		Metaball2DGroup ballGroups[2];
		Metaball2DGroup ballContainer;
		std::pair<double, double> velocities[2];
		std::pair<double, double> positions[2];
		double radiuses[2];

	public:
		Obstacle(): ballContainer(0.0, 0.6, 0.0){
			positions[0]  = std::pair<double, double>(30, 400);
			positions[1] = std::pair<double, double> (300, 30);

			for( int i=0; i<2; i++){
				radiuses[i]  = 20.0;
				ballGroups[i] = Metaball2DGroup(0.0,0.6,0.0);
				ballGroups[i].addMetaball( new Metaball2D(positions[i].first, positions[i].second, radiuses[i]));
				ballContainer.addSubgroup( ballGroups + i);
			}


			velocities[0]  = std::pair<double, double>(1, -1);
			velocities[1] = std::pair<double, double> (-2, 2);
		}

		void draw(){
			ballContainer.draw();
		}

		void collision(Sprite*){
		
			for( int i =0; i<2; i++){
				/*Wall Collision Test*/
				if(positions[i].first-radiuses[i] < 0 || positions[i].first+radiuses[i] > screenRight)
				{
					if( positions[i].first + radiuses[i] > screenRight){
						ballGroups[i].shiftGroup(screenRight - positions[i].first - radiuses[i] , 0);
						positions[i].first += screenRight - positions[i].first - radiuses[i];
					} else if(positions[i].first - radiuses[i] < 0)
					{
						ballGroups[i].shiftGroup(-positions[i].first +radiuses[i], 0);
						positions[i].first += -positions[i].first + radiuses[i];		
					}
					velocities[i].first = -velocities[i].first;		
				}

				if(positions[i].second - radiuses[i] < 0 || positions[i].second + radiuses[i] > screenTop)
				{
					if(positions[i].second + radiuses[i] > screenTop){
						ballGroups[i].shiftGroup(0, screenTop - positions[i].second - radiuses[i]);
						positions[i].second += screenTop - positions[i].second - radiuses[i];
					} else if(positions[i].second - radiuses[i] < 0)
					{
						ballGroups[i].shiftGroup(0, -positions[i].second +radiuses[i]);
						positions[i].second += -positions[i].second + radiuses[i];		
					}

					velocities[i].second = -velocities[i].second;			
				}	 
			}
		}

		void getRadiuses( double rs[2]){
			rs[0] = radiuses[0];
			rs[1] = radiuses[1];
		}

		void getPositions( std::pair<double, double> ps[2]){
			ps[0] = positions[0];
			ps[1] = positions[1];
		}

		void update(){
			for( int i =0; i<2; i++){
				positions[i].first += velocities[i].first;
				positions[i].second += velocities[i].second;
			
				ballGroups[i].shiftGroup( velocities[i].first, velocities[i].second);
			}
		}

		int getIdentifier(){
			return OBSTACLE_TYPE;
		}
};

#endif