#include "Light.h"

Light::Light(LTYPE t, double i, double d1, double d2, double d3, double p1, double p2, double p3)
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

Light::Light(const Light& L)
{
	type = L.type;
	intensity = L.intensity;

	for (int i = 0; i < 3; i++)
	{
		DIRECTION.V[i] = L.DIRECTION.V[i];
		POSITION.V[i] = L.POSITION.V[i];
	}
}

Light& Light::operator=(const Light& lhs)
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

void Light::set(LTYPE t, double i, double d1, double d2, double d3, double p1, double p2, double p3)
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