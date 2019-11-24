#include "primitiveComponent.h"
#include "../Shader/material.h"
#include "../World/world.h"
#include <glfw3/glfw3.h>
#include "staticMesh.h"

std::vector<PrimitiveComponent*> GPrimitives;

void PrimitiveComponent::BuildMesh()
{
	// create buffers/arrays
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);
	// load data into vertex buffers
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	// A great thing about structs is that their memory layout is sequential for all its items.
	// The effect is that we can simply pass a pointer to the struct and it translates perfectly to a glm::vec3/2 array which
	// again translates to 3/2 floats which translates to a byte array.

	std::cout << staticMeshSection->sectionVertices.size() << std::endl;
	std::cout << staticMeshSection->sectionIndices.size() << std::endl;

	glBufferData(GL_ARRAY_BUFFER, staticMeshSection->sectionVertices.size() * sizeof(Vertex), &staticMeshSection->sectionVertices[0], GL_STATIC_DRAW);

	if (staticMeshSection->sectionIndices.size() > 0)
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, staticMeshSection->sectionIndices.size() * sizeof(unsigned int), &staticMeshSection->sectionIndices[0], GL_STATIC_DRAW);
	}

	// set the vertex attribute pointers
	// vertex Positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
	// vertex normals
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
	// vertex texture coords
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));
	// vertex tangent
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Tangent));
	// vertex bitangent
	glEnableVertexAttribArray(4);
	glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Bitangent));

	glBindVertexArray(0);
}

PrimitiveComponent::PrimitiveComponent(World* inWorld)
{
	GPrimitives.push_back(this);

	location = glm::vec3(0);
	forwardVector = glm::vec3(1.f, 0.f, 0.f);
	angle = 0.f;
	scale3D = glm::vec3(1);
	OwningWorld = inWorld;
	OwningWorld->AddPrimitive(this);
}

PrimitiveComponent::PrimitiveComponent(World* inWorld, StaticMeshSection* inStaticMeshSection)
	:PrimitiveComponent(inWorld)
{
	staticMeshSection = inStaticMeshSection;
	if (inStaticMeshSection)
	{
		BuildMesh();
	}
	else
	{
		std::cout << "FAILED to load mesh" << std::endl;
	}
}

PrimitiveComponent::~PrimitiveComponent()
{
	for (unsigned int i = 0; i < GPrimitives.size(); ++i)
	{
		if (GPrimitives[i] == this)
		{
			GPrimitives.erase(GPrimitives.begin() + i);
			break;
		}
	}
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
}

void PrimitiveComponent::Draw(glm::mat4 viewMatrix, glm::mat4 projection)
{
	if (staticMeshSection->material)
	{
		staticMeshSection->material->use();

		glm::mat4 model = glm::mat4(1.0f); 
		model = glm::translate(model, location);
		model = glm::rotate(model, glm::radians(angle), forwardVector);
		model = glm::scale(model, scale3D);

		staticMeshSection->material->setMat4("view", viewMatrix);
		staticMeshSection->material->setMat4("projection", projection);
		staticMeshSection->material->setMat4("model", model);
	}

	glBindVertexArray(VAO);

	if (staticMeshSection->sectionIndices.size() > 0)
	{
		glDrawElements(GL_TRIANGLES, staticMeshSection->sectionIndices.size(), GL_UNSIGNED_INT, 0);
	}
	else
	{
		glDrawArrays(GL_TRIANGLES, 0, staticMeshSection->sectionVertices.size());
	}

	glBindVertexArray(0);

	// always good practice to set everything back to defaults once configured.
	glActiveTexture(GL_TEXTURE0);
}