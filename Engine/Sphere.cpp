#include "Sphere.h"

void Sphere::noColor()
{
	color.V[0] = color.V[1] = color.V[2] = 0;
}

Sphere::Sphere(int x, int y, int z, int r, int g, int b, double rad, int s)
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

Sphere::Sphere(const Sphere& sphere)
{
	radius = sphere.radius;
	specular = sphere.specular;
	for (int i = 0; i < 3; i++)
	{
		center.V[i] = sphere.center.V[i];
		color.V[i] = sphere.color.V[i];
	}
}