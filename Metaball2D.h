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

	inline double Equation(double x, double y) 
	{ 
		double denom =sqrt((x -px)*(x-px) + (y-py)*(y-py));
		if( denom == 0)
			return 1000.0;
		double val = denom * denom - denom + 0.25;
		return (radius/denom);
	}

	inline void calcNormal( double x, double y, double *nx, double *ny){
		double denom =pow((x -px)*(x-px) + (y-py)*(y-py), 1.0 + gooey / 2.0) + 0.0001;
		double scale = -gooey * radius / denom;
		*nx = (px - x) * scale;
		*ny = (py - y) * scale;
	}

	inline void calcTangent( double x, double y, double *tx, double *ty){
		calcNormal(x, y, tx, ty);
		double tmp = *tx;
		*tx = -*ty;
		*ty = tmp;
	}

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

private:
	double gooey;
	double px,py;//point x and y
	double radius;
	double radiusSquared;
};

#endif