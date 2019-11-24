#ifndef PRIMITIVECOMPONENT_H
#define PRIMITIVECOMPONENT_H

#include <vector> 
#include <iostream>
#include <glm/gtx/matrix_decompose.hpp>
#include <string>


class Material;
class vec3;
class vec4;
class World;
struct StaticMeshSection;

class PrimitiveComponent
{

protected:
	   
	glm::vec3 location;
	glm::vec3 forwardVector;
	float angle;
	glm::vec3 scale3D;

	StaticMeshSection* staticMeshSection;

	World* OwningWorld;

	/** Buffers */
	unsigned int VBO, VAO, EBO;

	virtual void BuildMesh();


public:
	virtual void Update(double DeltaTime) {

	}

	PrimitiveComponent(World* inWorld);
	PrimitiveComponent(World* inWorld, StaticMeshSection* inStaticMeshSection);
	~PrimitiveComponent();

	virtual void Draw(glm::mat4 viewMatrix, glm::mat4 projection);

	void SetLocation(glm::vec3 newLocation)
	{
		location = newLocation;
	}
	void SetForwardVector(glm::vec3 newForwardVector)
	{
		forwardVector = newForwardVector;
	}	
	void SetAngle(float newAngle)
	{
		angle = newAngle;
	}
	void SetScale3D(glm::vec3 newScale3D)
	{
		scale3D = newScale3D;
	}
};

#endif