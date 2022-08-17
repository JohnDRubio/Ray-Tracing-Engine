#pragma once
#include "Vec3.h"

class Sphere {
public:
	Vec3 center;
	Vec3 color;
	double radius = 1;
	int specular = -1;

	/*void CenterToOrigin()
	{
		center[0] = center[1] = center[2] = 0;
	}*/
	void noColor()
	{
		color.V[0] = color.V[1] = color.V[2] = 0;
	}

	Sphere(int x = 0, int y = 0, int z = 0, int r = 0, int g = 0, int b = 0, double rad = 1, int s = -1)
	{
		center.V[0] = x;
		center.V[1] = y;
		center.V[2] = z;
		color.V[0] = r;
		color.V[1] = g;
		color.V[2] = b;
		radius = rad;
		specular = s;
	}

	Sphere(const Sphere& sphere)
	{
		radius = sphere.radius;
		specular = sphere.specular;
		for (int i = 0; i < 3; i++)
		{
			center.V[i] = sphere.center.V[i];
			color.V[i] = sphere.color.V[i];
		}
	}
};