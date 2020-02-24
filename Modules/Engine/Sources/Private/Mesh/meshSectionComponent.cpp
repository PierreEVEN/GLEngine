
#include "Mesh/meshSectionComponent.h"
#include "Mesh/staticMeshComponent.h"
#include <Shader/material.h>
#include <Texture/texture.h>
#include <Mesh/MeshData.h>
#include <Mesh/staticMesh.h>

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

	glBufferData(GL_ARRAY_BUFFER, staticMeshSection->verticesCount * sizeof(Vertex), &staticMeshSection->sectionVertices[0], GL_STATIC_DRAW);

	if (staticMeshSection->indicesCount > 0)
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, staticMeshSection->indicesCount * sizeof(unsigned int), &staticMeshSection->sectionIndices[0], GL_STATIC_DRAW);
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

MeshSectionComponent::MeshSectionComponent(Scene* inScene, StaticMeshComponent* inParentStaticMeshComponent, MeshSectionData* inStaticMeshSection)
	: MeshSectionComponent(inScene)
{
	if (inParentStaticMeshComponent)
	{
		SetLocation(inParentStaticMeshComponent->GetLocation());
		SetRotation(inParentStaticMeshComponent->GetRotation());
		SetScale3D(inParentStaticMeshComponent->GetScale3D());
	}
	parentStaticMeshComponent = inParentStaticMeshComponent;
	staticMeshSection = inStaticMeshSection;
	if (inStaticMeshSection)
	{
		BuildMesh();
	}
	else
	{
		GFullLog(LogVerbosity::Assert, "MeshSectionComponent", "building mesh with null section");
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
		if (staticMeshSection->indicesCount > 0)
		{
			glDrawElements(GL_TRIANGLES, staticMeshSection->indicesCount, GL_UNSIGNED_INT, 0);
		}
		else
		{
			glDrawArrays(GL_TRIANGLES, 0, staticMeshSection->verticesCount);
		}
	}
	/** Set GL to defaults */
	glBindVertexArray(0);
	glActiveTexture(GL_TEXTURE0);
}

void MeshSectionComponent::RebuildTransformData()
{
	sectionTransform = glm::mat4(1.f);
	sectionTransform = glm::translate(sectionTransform, GetLocation().ToGLVector());

	sectionTransform = sectionTransform * glm::mat4_cast(glm::quat((float)GetRotation().w, (float)GetRotation().x, (float)GetRotation().y, (float)GetRotation().z));
	sectionTransform = glm::scale(sectionTransform, GetScale3D().ToGLVector());
	PrimitiveComponent::RebuildTransformData();
}

void MeshSectionComponent::RebuildLocalBounds()
{
	localBounds = staticMeshSection->localBounds;
	PrimitiveComponent::RebuildLocalBounds();
}
