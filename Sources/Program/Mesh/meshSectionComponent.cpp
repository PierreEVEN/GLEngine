
#include "../Shader/material.h"
#include "../Camera/camera.h"
#include "staticMesh.h"
#include "meshSectionComponent.h"
#include "../Asset/assetLibrary.h"
#include <glad/glad.h>


#include <bullet3D/BulletCollision/CollisionShapes/btTriangleMesh.h>
#include <bullet3D/BulletCollision/CollisionShapes/btConvexShape.h>
#include <bullet3D/BulletCollision/CollisionShapes/btShapeHull.h>
#include <bullet3D/BulletCollision/CollisionShapes/btConvexTriangleMeshShape.h>
#include <bullet3D/LinearMath/btDefaultMotionState.h>
#include <bullet3D/BulletDynamics/Dynamics/btRigidBody.h>
#include <bullet3D/BulletDynamics/Dynamics/btDiscreteDynamicsWorld.h>
#include "../Engine/debugerTool.h"
#include "staticMeshComponent.h"

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
	// vertex color
	glEnableVertexAttribArray(5);
	glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, VertexColor));

	glBindVertexArray(0);
	RebuildLocalBounds();
}

MeshSectionComponent::MeshSectionComponent(Scene* inScene)
	: PrimitiveComponent(inScene)
{
	materialOverride = nullptr;
	staticMeshSection = nullptr;
}

MeshSectionComponent::MeshSectionComponent(Scene* inScene, StaticMeshComponent* inParentStaticMeshComponent, StaticMeshSection* inStaticMeshSection)
	: MeshSectionComponent(inScene)
{
	if (inParentStaticMeshComponent)
	{
		SetLocation(inParentStaticMeshComponent->GetLocation());
		SetRotation(inParentStaticMeshComponent->GetRotation());
		SetAngle(inParentStaticMeshComponent->GetAngle());
		SetForwardVector(inParentStaticMeshComponent->GetForwardVector());
		SetScale3D(inParentStaticMeshComponent->GetScale3D());
	}
	parentStaticMeshComponent = inParentStaticMeshComponent;
	staticMeshSection = inStaticMeshSection;
	if (inStaticMeshSection)
	{
		BuildMesh();
	}
}

MeshSectionComponent::~MeshSectionComponent()
{
	materialOverride = nullptr;
	staticMeshSection = nullptr;
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
}

void MeshSectionComponent::Tick()
{
	PrimitiveComponent::Tick();
	{
		ProfileStat("Draw mesh material");
		Material* usedMaterial = materialOverride ? materialOverride : staticMeshSection->material;
		if (usedMaterial)
		{
			usedMaterial->use();

			/** Set materials commons */
			usedMaterial->setMat4("model", sectionTransform);

			/** Load additional textures */
			for (unsigned int i = 0; i < staticMeshSection->textures.size(); ++i)
			{
				if (staticMeshSection->textures[i]->GetTextureID() > -1)
				{
					ProfileStat("Set material dynamic textures");
					usedMaterial->setInt(std::string("DynamicTexture_") + std::to_string(i), i + usedMaterial->textures.size());
					glActiveTexture(GL_TEXTURE0 + i + usedMaterial->textures.size());
					glBindTexture(GL_TEXTURE_2D, staticMeshSection->textures[i]->GetTextureID());
				}
			}
		}
		else
		{
			/** Draw default material */
			MaterialEditorDebuger::GetGridMaterial()->use();
			MaterialEditorDebuger::GetGridMaterial()->setMat4("model", sectionTransform);
		}
	}

	{
		/** Draw vertices */
		ProfileStat("vertex rendering");
		glBindVertexArray(VAO);
		StatViewer::AddDrawcall();
		if (staticMeshSection->sectionIndices.size() > 0)
			glDrawElements(GL_TRIANGLES, staticMeshSection->sectionIndices.size(), GL_UNSIGNED_INT, 0);
		else
			glDrawArrays(GL_TRIANGLES, 0, staticMeshSection->sectionVertices.size());
	}
	/** Set GL to defaults */
	glBindVertexArray(0);
	glActiveTexture(GL_TEXTURE0);
}

void MeshSectionComponent::RebuildTransformData()
{
	sectionTransform = glm::mat4(1.f);
	sectionTransform = glm::translate(sectionTransform, GetLocation().ToGLVector());
	sectionTransform = glm::rotate(sectionTransform, glm::radians(GetAngle()), GetForwardVector());
	sectionTransform = glm::scale(sectionTransform, GetScale3D().ToGLVector());
	PrimitiveComponent::RebuildTransformData();
}

void MeshSectionComponent::RebuildLocalBounds()
{
	localBounds = staticMeshSection->bounds;
	PrimitiveComponent::RebuildLocalBounds();
}

CubemapSectionComponent::CubemapSectionComponent(Scene* inScene, StaticMeshComponent* inParentStaticMeshComponent, StaticMeshSection* inStaticMeshSection)
	: MeshSectionComponent(inScene, inParentStaticMeshComponent, inStaticMeshSection)
{
	drawPriority = DrawPriority::DrawPriority_First;
}

void CubemapSectionComponent::Tick()
{
	PrimitiveComponent::Tick();
	glDepthMask(GL_FALSE);
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::rotate(model, glm::radians(GetAngle()), GetForwardVector());
	model = glm::scale(model, GetScale3D().ToGLVector());

	Material* usedMaterial = materialOverride ? materialOverride : GetSectionData()->material;
	if (usedMaterial)
	{
		usedMaterial->use();

		/** Set materials commons */
		usedMaterial->setMat4("model", model);
	}
	else
	{
		MaterialEditorDebuger::GetGridMaterial()->use();
	}

	{
		/** Draw vertices */
		ProfileStat("vertex rendering");
		glBindVertexArray(VAO);
		StatViewer::AddDrawcall();
		if (GetSectionData()->sectionIndices.size() > 0)
			glDrawElements(GL_TRIANGLES, GetSectionData()->sectionIndices.size(), GL_UNSIGNED_INT, 0);
		else
			glDrawArrays(GL_TRIANGLES, 0, GetSectionData()->sectionVertices.size());
	}
	/** Set GL to defaults */
	glBindVertexArray(0);
	glActiveTexture(GL_TEXTURE0);
	glDepthMask(GL_TRUE);
}
