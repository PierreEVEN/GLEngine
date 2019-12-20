
#include "../Shader/material.h"
#include "../World/world.h"
#include "../Camera/camera.h"
#include <glfw3/glfw3.h>
#include "staticMesh.h"
#include "meshSectionComponent.h"
#include "../Asset/assetLibrary.h"


#include <bullet3D/BulletCollision/CollisionShapes/btTriangleMesh.h>
#include <bullet3D/BulletCollision/CollisionShapes/btConvexShape.h>
#include <bullet3D/BulletCollision/CollisionShapes/btShapeHull.h>
#include <bullet3D/BulletCollision/CollisionShapes/btConvexTriangleMeshShape.h>
#include <bullet3D/LinearMath/btDefaultMotionState.h>
#include <bullet3D/BulletDynamics/Dynamics/btRigidBody.h>
#include <bullet3D/BulletDynamics/Dynamics/btDiscreteDynamicsWorld.h>
#include "../Engine/debugerTool.h"

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
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, GetLocation().ToGLVector());
	model = glm::rotate(model, glm::radians(GetAngle()), GetForwardVector());
	model = glm::scale(model, GetScale3D().ToGLVector());

	{
		ProfileStat("Texture Rendering");
		if (staticMeshSection->material)
		{
			ProfileStat("Mesh material rendering");
			staticMeshSection->material->use(GetWorld());

			/** Set materials commons */
			//staticMeshSection->material->setMat4("view", GetWorld()->GetCamera()->GetViewMatrix());
			//staticMeshSection->material->setMat4("projection", GetWorld()->GetProjection());
			staticMeshSection->material->setMat4("model", model);
			//staticMeshSection->material->setMat4("ambient", model);

			/** Load additional textures */
			for (unsigned int i = 0; i < staticMeshSection->textures.size(); ++i)
			{
				staticMeshSection->material->setInt(std::string("DynamicTexture_") + std::to_string(i), i + staticMeshSection->material->textures.size());
				glActiveTexture(GL_TEXTURE0 + i + staticMeshSection->material->textures.size());
				glBindTexture(GL_TEXTURE_2D, staticMeshSection->textures[i]->GetTextureID());
			}
		}
		else
		{
			MaterialEditorDebuger::GetGridMaterial()->use(GetWorld());
			/** Set materials commons */
			MaterialEditorDebuger::GetGridMaterial()->setMat4("view", GetWorld()->GetCamera()->GetViewMatrix());
			MaterialEditorDebuger::GetGridMaterial()->setMat4("projection", GetWorld()->GetProjection());
			MaterialEditorDebuger::GetGridMaterial()->setMat4("model", model);
			MaterialEditorDebuger::GetGridMaterial()->setMat4("ambient", model);
		}
	}

	ProfileStat("vertex rendering");
	/** Draw vertices */
	glBindVertexArray(VAO);
	if (staticMeshSection->sectionIndices.size() > 0)
		glDrawElements(GL_TRIANGLES, staticMeshSection->sectionIndices.size(), GL_UNSIGNED_INT, 0);
	else
		glDrawArrays(GL_TRIANGLES, 0, staticMeshSection->sectionVertices.size());

	/** Set GL to defaults */
	glBindVertexArray(0);
	glActiveTexture(GL_TEXTURE0);
}