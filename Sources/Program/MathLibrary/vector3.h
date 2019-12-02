#ifndef VECTOR3_H
#define VECTOR3_H

#include <glm/gtx/matrix_decompose.hpp>
#include <bullet3D/LinearMath/btVector3.h>
#include <bullet3D/LinearMath/btTransform.h>

struct SVector3
{
	SVector3() {}

	SVector3(double inValue)
		: SVector3(inValue, inValue, inValue) {}

	SVector3(double inX, double inY, double inZ)
		: X(inX), Y(inY), Z(inZ) {}

	SVector3(glm::vec3 inGLMVector)
		: SVector3(inGLMVector[0], inGLMVector[1], inGLMVector[2]) {}

	SVector3(btVector3 inBulletVector)
		: SVector3(inBulletVector.getX(), inBulletVector.getY(), inBulletVector.getZ()) {}

	double X = 0;
	double Y = 0;
	double Z = 0;

	SIMD_FORCE_INLINE SVector3
	operator+(const SVector3& v1)
	{
		return SVector3(
			v1.X + X,
			v1.Y + Y,
			v1.Z + Z);
	}

	SVector3 operator-(const SVector3& v1)
	{
		return SVector3(
			X - v1.X,
			Y - v1.Y,
			Z - v1.Z
		);
	}

	SVector3 operator*(const SVector3& v1)
	{
		return SVector3(
			v1.X * X,
			v1.Y * Y,
			v1.Z * Z);
	}

	SVector3 operator/(const SVector3& v1)
	{
		return SVector3(
			X / v1.X,
			Y / v1.Y,
			Z / v1.Z
		);
	}

	void operator+=(const SVector3& v)
	{
		X += v.X;
		Y += v.Y;
		Z += v.Z;
	}

	void operator-=(const SVector3& v)
	{
		X -= v.X;
		Y -= v.Y;
		Z -= v.Z;
	}

	void operator*=(const SVector3& v)
	{
		X *= v.X;
		Y *= v.Y;
		Z *= v.Z;
	}

	void operator/=(const SVector3& v)
	{
		X /= v.X;
		Y /= v.Y;
		Z /= v.Z;
	}

	void operator=(const SVector3& v)
	{
		X = v.X;
		Y = v.Y;
		Z = v.Z;
	}

	void operator=(const btVector3& v)
	{
		X = v.getX();
		Y = v.getY();
		Z = v.getZ();
	}


	btVector3 ToBulletVector() const
	{
		return btVector3(btScalar(X), btScalar(Y), btScalar(Z));
	}

	glm::vec3 ToGLVector() const
	{
		return glm::vec3((float)X, (float)Y, (float)Z);
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

#endif