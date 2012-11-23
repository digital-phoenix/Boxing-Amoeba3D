#ifndef METABALL2D_H_
#define METABALL2D_H_

class Metaball2D
{
	

public:

	Metaball2D(double px , double py , double radius)
	{
		gooey = 1.0;
		this->px = px;
		this->py = py;
		this->pz = -100;
		this->radius = radius;
		radiusSquared = radius * radius;
	}

	Metaball2D(){
		gooey = 1.0;
		px = 0.0;
		py = 0.0;
		radius = 1.0;
		radiusSquared = radius * radius;
	}

	inline double Equation(double x, double y, double z = 0) 
	{ 
		double denom =sqrt((x -px)*(x-px) + (y-py)*(y-py) + (z-pz)*(z-pz) );
		if( denom == 0)
			return 1000.0;
		return (radius/denom);
	}
	/*
	*(2(a-x),2(b-y),2(c-z))/((x-a)^2 + (y -b)^2 + (z -c)^2)^3/2
	*
	*/
	inline void calcNormal( double x, double y, double z, double *nx, double *ny, double *nz){
		double denom =pow((x -px)*(x-px) + (y-py)*(y-py) + (z - pz) * (z - pz), 3.0/ 2.0) + 0.0001;
		*nx = 2 * (px - x) / denom;
		*ny =  2 * (py - y) / denom;
		*nz = 2 * (pz - z) / denom;
	}

	/*inline void calcTangent( double x, double y, double *tx, double *ty){
		calcNormal(x, y, tx, ty);
		double tmp = *tx;
		*tx = -*ty;
		*ty = tmp;
	}*/

	void move(double x, double y)
	{
		px = x;
		py = y;
	}

	void shift(double x, double y){
		px += x;
		py += y;
	}
	
	inline double getRadius(){
		return radius;
	}

	inline void setRadius(double val){
		radius = val;
	}

	inline double getPx(){
		return px;
	}

	inline double getPy(){
		return py;
	}

	inline double getPz(){
		return pz;
	}

private:
	double gooey;
	double px,py,pz;//point x and y
	double radius;
	double radiusSquared;
};

#endif