
#include "../Shader/material.h"
#include "../World/world.h"
#include "../Camera/camera.h"
#include <glfw3/glfw3.h>
#include "staticMesh.h"
#include "meshSectionComponent.h"

void MeshSectionComponent::BuildMesh()
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

MeshSectionComponent::MeshSectionComponent(World* inWorld)
	: SceneComponent(inWorld) { }

MeshSectionComponent::MeshSectionComponent(World* inWorld, StaticMeshSection* inStaticMeshSection)
	: MeshSectionComponent(inWorld)
{
	staticMeshSection = inStaticMeshSection;
	if (inStaticMeshSection)
	{
		BuildMesh();
	}
}

MeshSectionComponent::~MeshSectionComponent()
{
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
}

void MeshSectionComponent::MarkRenderStateDirty()
{
	if (staticMeshSection->material)
	{
		staticMeshSection->material->use(GetWorld());

		glm::mat4 model = glm::mat4(1.0f); 
		model = glm::translate(model, location);
		model = glm::rotate(model, glm::radians(angle), forwardVector);
		model = glm::scale(model, scale3D);
		
		staticMeshSection->material->setMat4("view", GetWorld()->GetCamera()->GetViewMatrix());
		staticMeshSection->material->setMat4("projection", GetWorld()->GetProjection());
		staticMeshSection->material->setMat4("model", model);
		staticMeshSection->material->setMat4("ambient", model);
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