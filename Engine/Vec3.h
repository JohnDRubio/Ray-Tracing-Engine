#pragma once

class Vec3
{
public:
	// 3-D vector
	double V[3]{};

	// Overload operators for vector arithmetic
	Vec3& operator=(const Vec3& rhs);
	Vec3 operator+(const Vec3& other) const;
	Vec3 operator-(const Vec3& rhs) const;
	Vec3 operator*(const Vec3& other);
	Vec3 operator*(double s);

	// Scalar * Vector is slightly more complicated
	//	requires creation of a temporary Vec3 with each component equal to scalar value
	//	this temporary Vec3 is then multiplied by vector on right-hand side which calls the function below
	friend Vec3 operator*(double s, const Vec3& rhs);

	// Constructor
	Vec3(double x = 0, double y = 0, double z = 0);
};