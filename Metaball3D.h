#ifndef METABALL3D_H_
#define METABALL3D_H_

class Metaball3D
{
	

public:

	Metaball3D(double px , double py ,double pz, double radius)
	{
		gooey = 1.0;
		this->px = px;
		this->py = py;
		this->pz = pz;
		this->radius = radius;
		radiusSquared = radius * radius;
	}

	Metaball3D(){
		gooey = 1.0;
		px = 0.0;
		py = 0.0;
		pz = 0.0;
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

	void move(double x, double y, double z)
	{
		px = x;
		py = y;
		pz = z;
	}

	void shift(double x, double y, double z){
		px += x;
		py += y;
		pz += z;
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
	double px,py,pz;//point x, y and z
	double radius;
	double radiusSquared;
};

#endif