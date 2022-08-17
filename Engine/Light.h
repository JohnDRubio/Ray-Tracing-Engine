#pragma once

class Light {
public:
	enum LTYPE { ambient, point, directional };
	LTYPE type;
	double intensity;
	Vec3 DIRECTION;
	Vec3 POSITION;

	Light(LTYPE t = ambient, double i = 1, double d1 = 0, double d2 = 0, double d3 = 0, double p1 = 0, double p2 = 0, double p3 = 0)
	{
		type = t;
		intensity = i;
		DIRECTION.V[0] = d1;
		DIRECTION.V[1] = d2;
		DIRECTION.V[2] = d3;
		POSITION.V[0] = p1;
		POSITION.V[1] = p2;
		POSITION.V[2] = p3;
	}

	Light(const Light& L)
	{
		type = L.type;
		intensity = L.intensity;

		for (int i = 0; i < 3; i++)
		{
			DIRECTION.V[i] = L.DIRECTION.V[i];
			POSITION.V[i] = L.POSITION.V[i];
		}
	}

	Light& operator=(const Light& lhs)
	{
		if (this != &lhs)
		{
			type = lhs.type;
			intensity = lhs.intensity;

			for (int i = 0; i < 3; i++)
			{
				DIRECTION.V[i] = lhs.DIRECTION.V[i];
				POSITION.V[i] = lhs.POSITION.V[i];
			}
		}
		return *this;
	}

	void set(LTYPE t = ambient, double i = 1, double d1 = 0, double d2 = 0, double d3 = 0, double p1 = 0, double p2 = 0, double p3 = 0)
	{
		type = t;
		intensity = i;
		DIRECTION.V[0] = d1;
		DIRECTION.V[1] = d2;
		DIRECTION.V[2] = d3;
		POSITION.V[0] = p1;
		POSITION.V[1] = p2;
		POSITION.V[2] = p3;
	}
};