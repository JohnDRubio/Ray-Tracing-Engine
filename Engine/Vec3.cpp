#include "Vec3.h"

// Vector addition
Vec3 Vec3::operator+(const Vec3& other) const
{
	Vec3 res;
	res.V[0] = V[0] + other.V[0];
	res.V[1] = V[1] + other.V[1];
	res.V[2] = V[2] + other.V[2];
	return  res;
}

// Vector subtraction
Vec3 Vec3::operator-(const Vec3& other) const
{
	Vec3 res;
	res.V[0] = V[0] - other.V[0];
	res.V[1] = V[1] - other.V[1];
	res.V[2] = V[2] - other.V[2];
	return  res;
}


Vec3::Vec3(double x, double y, double z)
{
	V[0] = x;
	V[1] = y;
	V[2] = z;
}

// Vector assignment
Vec3& Vec3::operator=(const Vec3& rhs)
{
	if (this != &rhs)
	{
		for (int i = 0; i < 3; i++)
		{
			V[i] = rhs.V[i];
		}
		return *this;
	}
	else
		return *this;
}

// Scalar * Vector
//		Helper function for Vec3::operator*(double s, const Vec3& rhs)
Vec3 Vec3::operator*(const Vec3& other)
{
	V[0] *= other.V[0];
	V[1] *= other.V[1];
	V[2] *= other.V[2];
	return *this;
}

// Scalar * Vector
//		For case where scalar is on the right
Vec3 Vec3::operator*(double s)
{
	V[0] *= s;
	V[1] *= s;
	V[2] *= s;
	return *this;
}

// Scalar * Vector
//		Case where scalar is on the left
//		Creates temporary Vec3 and multiplies it by rhs - which calls Vec3::operator*(const Vec3& other)
Vec3 operator*(double s, const Vec3& rhs)
{
	return Vec3(s,s,s) * rhs;
}
