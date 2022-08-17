#pragma once


struct init
	{
	public:
		int x;
		int y;
		int z; 
		int r;
		int g;
		int b;
		double radius = 1;
		int specular = -1;
	
		init(class HWNDKey& key, int _x = 0, int _y = 0, int _z = 0, int _r = 0, int _g = 0, int _b = 0, double rad = 1, int s = -1)
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
	
		void set(int _x = 0, int _y = 0, int _z = 0, int _r = 0, int _g = 0, int _b = 0, double rad = 1, int s = -1)
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
	};