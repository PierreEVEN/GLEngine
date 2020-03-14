#pragma once

#include <OpenglEnginePCH.h>

#define CrossProductMacro(varA, varB) {(varA).y * (varB).z - (varA).z * (varB).y, (varA).z * (varB).x - (varA).x * (varB).z, (varA).x* (varB).y - (varA).y * (varB).x}
#define DotProductMacro(varA, varB) ((varA).x * (varB).x + (varA).y * (varB).y + (varA).z * (varB).z)

struct SVector3
{
	SVector3() {}

	SVector3(const double& inValue)
		: SVector3(inValue, inValue, inValue) {}

	SVector3(const double& inX, const double& inY, const double& inZ)
		: x(inX), y(inY), z(inZ) {}

	SVector3(const glm::vec3& inGLMVector)
		: x(inGLMVector[0]), y(inGLMVector[1]), z(inGLMVector[2]) {}

	SVector3(const btVector3& inBulletVector)
		: SVector3(inBulletVector.getX(), inBulletVector.getY(), inBulletVector.getZ()) {}

	double x = 0;
	double y = 0;
	double z = 0;

	SIMD_FORCE_INLINE SVector3
	operator+(const SVector3& v1)
	{
		return SVector3(
			v1.x + x,
			v1.y + y,
			v1.z + z);
	}

	SVector3 operator-(const SVector3& v1)
	{
		return SVector3(
			x - v1.x,
			y - v1.y,
			z - v1.z
		);
	}

	SVector3 operator*(const double& s1)
	{
		return SVector3(
			s1 * x,
			s1 * y,
			s1 * z);
	}

	SVector3 operator*(const SVector3& v1)
	{
		return SVector3(
			v1.x * x,
			v1.y * y,
			v1.z * z);
	}

	SVector3 operator/(const SVector3& v1)
	{
		return SVector3(
			x / v1.x,
			y / v1.y,
			z / v1.z
		);
	}

	void operator+=(const SVector3& v)
	{
		x += v.x;
		y += v.y;
		z += v.z;
	}

	void operator-=(const SVector3& v)
	{
		x -= v.x;
		y -= v.y;
		z -= v.z;
	}

	void operator*=(const SVector3& v)
	{
		x *= v.x;
		y *= v.y;
		z *= v.z;
	}

	void operator/=(const SVector3& v)
	{
		x /= v.x;
		y /= v.y;
		z /= v.z;
	}

	void operator=(const SVector3& v)
	{
		x = v.x;
		y = v.y;
		z = v.z;
	}

	void operator=(const btVector3& v)
	{
		x = v.getX();
		y = v.getY();
		z = v.getZ();
	}

	bool operator==(const SVector3& v) const
	{
		return x == v.x && y == v.y && z == v.z;
	}

	std::string ToString() const {
		return "x=" + std::to_string(x) + ", y=" + std::to_string(y) + ", z=" + std::to_string(z);
	}

	btVector3 ToBulletVector() const
	{
		return btVector3(btScalar(x), btScalar(y), btScalar(z));
	}

	glm::vec3 ToGLVector() const
	{
		return glm::vec3((double)x, (double)y, (double)z);
	}

	double GetLength() const
	{
		return sqrt(x * x + y * y + z * z);
	}

	double GetMax() const
	{
		if (x > y && x > z)
		{
			return x;
		}
		else if (y > x && y > z)
		{
			return y;
		}
		else
		{
			return z;
		}
	}

	double GetMin() const
	{
		if (x < y && x < z)
		{
			return x;
		}
		else if (y < x && y < z)
		{
			return y;
		}
		else
		{
			return z;
		}
	}

	SVector3 Normalize()
	{
		double length = sqrt(x * x + y * y + z * z);
		x /= length;
		y /= length;
		z /= length;
		return *this;
	}
};

struct SRotator
{
private:
	void SetFromEuleurAngles(const double& inRoll, const double& inPitch, const double& inYaw)
	{
		double cy = cos(((inYaw / 180) * M_PI) * 0.5);
		double sy = sin(((inYaw / 180) * M_PI) * 0.5);
		double cp = cos(((inPitch / 180) * M_PI) * 0.5);
		double sp = sin(((inPitch / 180) * M_PI) * 0.5);
		double cr = cos(((inRoll / 180) * M_PI) * 0.5);
		double sr = sin(((inRoll / 180) * M_PI) * 0.5);

		w = cy * cp * cr + sy * sp * sr;
		x = cy * cp * sr - sy * sp * cr;
		y = sy * cp * sr + cy * sp * cr;
		z = sy * cp * cr - cy * sp * sr;
	}

	void SetFromForwardVectorAndAngle(const double& inX, const double& inY, const double& inZ, const double& inAngle)
	{
		double rAngle = (inAngle / 180) * M_PI;
		double s = sin(rAngle / 2);
		x = inX * s;
		y = inY * s;
		z = inZ * s;
		w = cos(rAngle / 2);
	}

public:

	double x = 0;
	double y = 0;
	double z = 0;
	double w = 0;

	SRotator()
		: SRotator(0) {}

	SRotator(double inValue)
		: SRotator(inValue, inValue, inValue) {}

	SRotator(double inX, double inY, double inZ, double inW)
		: x(inX), y(inY), z(inZ), w(inW) { }

	SRotator(double inRoll, double inPitch, double inYaw)
	{
		SetFromEuleurAngles(inRoll, inPitch, inYaw);
	}

	SRotator(SVector3 inForwardVector, double inAngle)
	{
		SetFromForwardVectorAndAngle(inForwardVector.x, inForwardVector.y, inForwardVector.z, inAngle);
	}
	
	SVector3 GetForwardVector() const
	{
		return SVector3
		(
			1 - 2 * (y*y + z * z),
			2 * (x*y + w * z),
			2 * (x*z - w * y)
		);
	}
	SVector3 GetRightVector() const
	{
		return SVector3
		(
			2 * (x*y - w * z),
			1 - 2 * (x*x + z * z),
			2 * (y*z + w * x)
		);
	}
	SVector3 GetUpVector() const
	{
		return SVector3
		(
			2 * (x*z + w * y),
			2 * (y*z - w * x),
			1 - 2 * (x*x + y * y)
		);

	}
	double GetAngle() const
	{
		return (acos(w) * 2) / M_PI * 180;
	}

	double GetRoll() const
	{
		double sinr_cosp = 2 * (w * x + y * z);
		double cosr_cosp = 1 - 2 * (x * x + y * y);
		return std::atan2(sinr_cosp, cosr_cosp) / M_PI * 180;
	}
	double GetPitch() const
	{
		double sinp = 2 * (w * y - z * x);
		if (std::abs(sinp) >= 1)
			return std::copysign(M_PI / 2, sinp) / M_PI * 180; // use 90 degrees if out of range
		else
			return std::asin(sinp) / M_PI * 180;
	}
	double GetYaw() const
	{
		double siny_cosp = 2 * (w * z + x * y);
		double cosy_cosp = 1 - 2 * (y * y + z * z);
		return std::atan2(siny_cosp, cosy_cosp) / M_PI * 180;
	}


	SRotator operator+(const SRotator& r)
	{
		return SRotator(
			r.x + x,
			r.y + y,
			r.z + z,
			r.w + w
		);
	}

	SRotator operator-(const SRotator& r)
	{
		return SRotator(
			r.x - x,
			r.y - y,
			r.z - z,
			r.w - w
		);
	}

	SRotator operator*(const SRotator& r)
	{
		return SRotator(
			r.x * x,
			r.y * y,
			r.z * z,
			r.w * w
		);
	}

	SRotator operator/(const SRotator& r)
	{
		return SRotator(
			r.x / x,
			r.y / y,
			r.z / z,
			r.w / w
		);
	}

	bool operator==(const SRotator& r) const
	{
		return
			r.x == x &&
			r.y == y &&
			r.z == z &&
			r.w == w;			
	}

	bool operator!=(const SRotator& r) const
	{
		return !(r == *this);
	}
};

struct STransform
{
private:

	SVector3 Location = SVector3(0);
	SRotator Rotation = SRotator(0);
	SVector3 Scale3D = SVector3(0);

public:
	STransform() { ToIdentity(); }

	STransform(SVector3 inLocation)
		: Location(inLocation) {}

	STransform(SRotator inRotation)
		: Rotation(inRotation) {}

	STransform(SVector3 inLocation, SRotator inRotation)
		: Location(inLocation), Rotation(inRotation) {}

	STransform(SVector3 inLocation, SRotator inRotation, SVector3 inScale3D)
		: Location(inLocation), Rotation(inRotation), Scale3D(inScale3D) {}

	STransform(btTransform inBulletTransform)
		: Location(inBulletTransform.getOrigin()), Rotation(inBulletTransform.getRotation().getAxis(), inBulletTransform.getRotation().getAngle()), Scale3D(1.0) {}

	STransform operator+(const STransform& t1)
	{
		return STransform(
			t1.GetLocation() + Location
		);
	}

	STransform operator-(const STransform& t1)
	{
		return STransform(
			Location - t1.GetLocation(),
			Rotation - t1.GetRotation(),
			Scale3D - t1.GetScale3D()
		);
	}

	STransform operator*(const STransform& t1)
	{
		return STransform(
			t1.GetLocation() * Location,
			t1.GetRotation() * Rotation,
			t1.GetScale3D() * Scale3D
		);
	}

	STransform operator/(const STransform& t1)
	{
		return STransform(
			Location / t1.GetLocation(),
			Rotation / t1.GetRotation(),
			Scale3D / t1.GetScale3D()
		);
	}

	void operator=(const STransform& t)
	{
		Location = t.GetLocation();
		Rotation = t.GetRotation();
		Scale3D = t.GetScale3D();
	}
	
	SVector3 GetLocation() const { return Location; }
	SRotator GetRotation() const { return Rotation; }
	SVector3 GetScale3D() const { return Scale3D; }

	void SetLocation(const SVector3 inLocation) { Location = inLocation; }
	void SetRotation(const SRotator inRotation) { Rotation = inRotation; }
	void SetScale3D(const SVector3 inScale3D) { Scale3D = inScale3D; }

	void ToIdentity()
	{
		Location = SVector3(0);
		Rotation = SRotator(0);
		Scale3D = SVector3(1);
	}
};