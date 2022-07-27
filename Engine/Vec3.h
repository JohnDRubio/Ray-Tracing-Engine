#pragma once

class Vec3
{
public:

	// 3-D vector
	double V[3]{};

	// Assignment overload
	Vec3& operator=(const Vec3& rhs);
};