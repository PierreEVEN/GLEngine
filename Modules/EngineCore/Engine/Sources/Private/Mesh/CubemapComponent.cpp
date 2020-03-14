#include <Mesh/CubemapComponent.h>
#include <Shader/material.h>
#include <Mesh/staticMesh.h>
#include <Scene/scene.h>
#include <Camera/camera.h>
#include <Engine/ThreadHandler.h>

void CubemapComponent::CreateMesh()
{
	CHECK_RENDER_THREAD;

	std::vector<SVertex> vArray;
	std::vector<unsigned int> vTriangle;

	//Bottom
	vTriangle.push_back(0);
	vTriangle.push_back(2);
	vTriangle.push_back(1);
	vTriangle.push_back(1);
	vTriangle.push_back(2);
	vTriangle.push_back(3);

	//Top
	vTriangle.push_back(4);
	vTriangle.push_back(5);
	vTriangle.push_back(6);
	vTriangle.push_back(6);
	vTriangle.push_back(5);
	vTriangle.push_back(7);

	//Left
	vTriangle.push_back(0);
	vTriangle.push_back(4);
	vTriangle.push_back(2);
	vTriangle.push_back(4);
	vTriangle.push_back(6);
	vTriangle.push_back(2);

	//Right
	vTriangle.push_back(1);
	vTriangle.push_back(3);
	vTriangle.push_back(5);
	vTriangle.push_back(3);
	vTriangle.push_back(7);
	vTriangle.push_back(5);

	//Front
	vTriangle.push_back(0);
	vTriangle.push_back(1);
	vTriangle.push_back(4);
	vTriangle.push_back(1);
	vTriangle.push_back(5);
	vTriangle.push_back(4);

	//Bottom
	vTriangle.push_back(3);
	vTriangle.push_back(2);
	vTriangle.push_back(6);
	vTriangle.push_back(3);
	vTriangle.push_back(6);
	vTriangle.push_back(7);

	vArray.push_back(SVertex(glm::vec3(-1, -1, -1)));//Root	0
	vArray.push_back(SVertex(glm::vec3(1, -1, -1)));//Root+X	 1
	vArray.push_back(SVertex(glm::vec3(-1, 1, -1)));//Root+Y	2
	vArray.push_back(SVertex(glm::vec3(1, 1, -1))); //Root+XY	3

	vArray.push_back(SVertex(glm::vec3(-1, -1, 1))); //Root + Z	4
	vArray.push_back(SVertex(glm::vec3(1, -1, 1))); //Root+ZX	5
	vArray.push_back(SVertex(glm::vec3(-1, 1, 1))); // Root+ZY	6
	vArray.push_back(SVertex(glm::vec3(1, 1, 1))); // Root+ZXY	7

	unsigned int VAO, VBO, EBO;

	// create buffers/arrays
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);
	// load data into vertex buffers
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vArray.size() * sizeof(SVertex), vArray.data(), GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, vTriangle.size() * sizeof(unsigned int), vTriangle.data(), GL_STATIC_DRAW);


	// set the vertex attribute pointers
	// vertex Positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(SVertex), (void*)0);
	// vertex normals
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(SVertex), (void*)offsetof(SVertex, Normal));
	// vertex texture coords
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(SVertex), (void*)offsetof(SVertex, TexCoords));
	// vertex tangent
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(SVertex), (void*)offsetof(SVertex, Tangent));
	// vertex bitangent
	glEnableVertexAttribArray(4);
	glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(SVertex), (void*)offsetof(SVertex, Bitangent));
	// vertex color
	glEnableVertexAttribArray(5);
	glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(SVertex), (void*)offsetof(SVertex, VertexColor));
	glBindVertexArray(0);

	SetMeshData(
		0, 
		SProxySectionData(VBO, VAO, EBO, (unsigned int)vTriangle.size(), 0),
		0
	);
}

CubemapComponent::CubemapComponent(Scene* inScene)
	: MeshComponent(inScene) {
	drawPriority = DrawPriority::DrawPriority_First;
}

void CubemapComponent::Render()
{
	if (!bHasMeshDataBeenInitialized)
	{
		bHasMeshDataBeenInitialized = true;
		CreateMesh();
	}
	MeshComponent::Render();
}