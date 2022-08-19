#pragma once
#include "Init.h"

init::init(class HWNDKey& key, int _x, int _y, int _z, int _r, int _g, int _b, double rad, int s)
{
	x = _x;
	y = _y;
	z = _z;
	r = _r;
	g = _g;
	b = _b;
	radius = rad;
	specular = s;
}

void init::set(int _x, int _y, int _z, int _r, int _g, int _b, double rad, int s)
{
	x = _x;
	y = _y;
	z = _z;
	r = _r;
	g = _g;
	b = _b;
	radius = rad;
	specular = s;
}