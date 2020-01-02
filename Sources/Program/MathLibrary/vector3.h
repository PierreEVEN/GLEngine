#pragma once

#include <glm/gtx/matrix_decompose.hpp>
#include <bullet3D/LinearMath/btVector3.h>
#include <bullet3D/LinearMath/btTransform.h>
#include <string>

#define CrossProduct(varA, varB) {(varA).y * (varB).z - (varA).z * (varB).y, (varA).z * (varB).x - (varA).x * (varB).z, (varA).x* (varB).y - (varA).y * (varB).x}
#define DotProduct(varA, varB) ((varA).x * (varB).x + (varA).y * (varB).y + (varA).z * (varB).z)

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

	std::string ToString() const {
		return "x=" + std::to_string(x) + ", y=" + std::to_string(y) + ", y=" + std::to_string(z);
	}

	btVector3 ToBulletVector() const
	{
		return btVector3(btScalar(x), btScalar(y), btScalar(z));
	}

	glm::vec3 ToGLVector() const
	{
		return glm::vec3((float)x, (float)y, (float)z);
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

	SVector3 ForwardVector;
	float angle = 0;

public:
	SRotator() { }

	SRotator(float inValue)
		: SRotator(inValue, inValue, inValue) {}

	SRotator(float inRoll, float inPitch, float inYaw)
		: ForwardVector(0), angle(0) {}

	SRotator(float inX, float inY, float inZ, float inAngle)
		: SRotator(SVector3(inX, inY, inZ), inAngle) {}

	SRotator(SVector3 inForwardVector, float inAngle)
		: ForwardVector(inForwardVector), angle(inAngle) {}
	
	SVector3 GetForwardVector() const { return ForwardVector; }
	float GetAngle() const { return angle; }


	SRotator operator+(const SRotator& r)
	{
		return SRotator(
			r.GetForwardVector() + ForwardVector,
			r.GetAngle() + angle
		);
	}

	SRotator operator-(const SRotator& r)
	{
		return SRotator(
			ForwardVector - r.GetForwardVector(),
			angle - r.GetAngle()
		);
	}

	SRotator operator*(const SRotator& r)
	{
		return SRotator(
			ForwardVector * r.GetForwardVector(),
			angle * r.GetAngle()
		);
	}

	SRotator operator/(const SRotator& r)
	{
		return SRotator(
			ForwardVector / r.GetForwardVector(),
			angle / r.GetAngle()
		);
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
		: Location(inBulletTransform.getOrigin()), Rotation(inBulletTransform.getRotation().getAxis().getX(), inBulletTransform.getRotation().getAxis().getY(), inBulletTransform.getRotation().getAxis().getZ(), inBulletTransform.getRotation().getAngle()), Scale3D(1.0) {}

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
	SVector3 GetScale3D() const { return Location; }

	void ToIdentity()
	{
		Location = SVector3(0);
		Rotation = SRotator(0);
		Scale3D = SVector3(1);
	}
};