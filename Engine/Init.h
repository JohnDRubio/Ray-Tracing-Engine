#pragma once

struct init
{
	// Member variables
	int x;
	int y;
	int z; 
	int r;
	int g;
	int b;
	double radius = 1;
	int specular = -1;
	
	// Methods
	init(class HWNDKey& key, int _x = 0, int _y = 0, int _z = 0, int _r = 0, int _g = 0, int _b = 0, double rad = 1, int s = -1);	
	void set(int _x = 0, int _y = 0, int _z = 0, int _r = 0, int _g = 0, int _b = 0, double rad = 1, int s = -1);
};