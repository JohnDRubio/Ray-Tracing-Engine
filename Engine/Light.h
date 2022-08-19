#pragma once
#include "Vec3.h"

class Light {
public:
	// Member variables
	enum LTYPE { ambient, point, directional };
	LTYPE type;
	double intensity;
	Vec3 DIRECTION;
	Vec3 POSITION;

	// Methods
	Light(LTYPE t = ambient, double i = 1, double d1 = 0, double d2 = 0, double d3 = 0, double p1 = 0, double p2 = 0, double p3 = 0);
	Light(const Light& L);
	Light& operator=(const Light&);
	void set(LTYPE t = ambient, double i = 1, double d1 = 0, double d2 = 0, double d3 = 0, double p1 = 0, double p2 = 0, double p3 = 0);
};