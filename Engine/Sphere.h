#pragma once
#include "Vec3.h"

class Sphere {
public:
	// Member variables
	Vec3 center;
	Vec3 color;
	double radius = 1;
	int specular = -1;

	// Methods
	void noColor();
	Sphere(int x = 0, int y = 0, int z = 0, int r = 0, int g = 0, int b = 0, double rad = 1, int s = -1);
	Sphere(const Sphere&);
};