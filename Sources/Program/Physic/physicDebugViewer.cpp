#include "physicDebugViewer.h"
#include <glad/glad.h>
#include <glfw3/glfw3.h>
#include "../Shader/material.h"
#include "../Camera/camera.h"
#include <glm/gtx/rotate_vector.inl>
#include "../Mesh/staticMesh.h"
#include "../Engine/debugerTool.h"
#include "../MathLibrary/vector3.h"

std::vector<Vertex> vertexArray = {};
int currentLineVerticeIndex = 0;

GLDebugDrawer::GLDebugDrawer(Scene* inRenderScene)
{
	renderScene = inRenderScene;
}

GLDebugDrawer::~GLDebugDrawer()
{

}

void GLDebugDrawer::drawLine(const btVector3& from, const btVector3& to, const btVector3& fromColor, const btVector3& toColor)
{
	GLDebugDrawer::DrawDebugLine(from, to, fromColor, toColor);
}

void GLDebugDrawer::drawLine(const btVector3& from, const btVector3& to, const btVector3& color)
{
	GLDebugDrawer::DrawDebugLine(from, to, color);
}

void GLDebugDrawer::drawContactPoint(const btVector3& PointOnB, const btVector3& normalOnB, btScalar distance, int lifeTime, const btVector3& color)
{
}

void GLDebugDrawer::reportErrorWarning(const char* warningString)
{
	GFullLog(LogVerbosity::Warning, "Bullet3D", warningString);
}

void GLDebugDrawer::draw3dText(const btVector3& location, const char* textString)
{

}

void GLDebugDrawer::setDebugMode(int debugMode)
{
	m_debugMode = debugMode;
}

void GLDebugDrawer::RenderDebugDraw(World* inRenderWorld)
{

}

void GLDebugDrawer::RenderDebugDraw()
{
	if (currentLineVerticeIndex > 0)
	{
		ProfileStat("Draw debug trace");
		GLuint VBO, VAO;
		glGenBuffers(1, &VBO);
		glGenVertexArrays(1, &VAO);
		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, currentLineVerticeIndex * sizeof(Vertex), vertexArray.data(), GL_STATIC_DRAW);

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

		MaterialEditorDebuger::GetDebugMaterial()->use();
		glBindVertexArray(VAO);
		glDrawArrays(GL_LINES, 0, vertexArray.size());
		glBindVertexArray(0);

		glDeleteBuffers(1, &VBO);
		glDeleteVertexArrays(1, &VAO);
		vertexArray.clear();
		currentLineVerticeIndex = 0;
	}
}

void GLDebugDrawer::DrawDebugLine(const SVector3& from, const SVector3& to, const SVector3& fromColor, const SVector3& toColor)
{
	Vertex posA;
	posA.Position = from.ToGLVector();
	posA.TexCoords = glm::vec2(0);
	posA.VertexColor = glm::vec4(fromColor.x, fromColor.y, fromColor.z, 1.f);

	Vertex posB;
	posB.Position = to.ToGLVector();
	posB.TexCoords = glm::vec2(1);
	posB.VertexColor = glm::vec4(toColor.x, toColor.y, toColor.z, 1.f);

	if (vertexArray.size() - currentLineVerticeIndex < 2)
	{
		vertexArray.resize(vertexArray.size() + 1024);
	}

	vertexArray[currentLineVerticeIndex] = posA;
	currentLineVerticeIndex++;
	vertexArray[currentLineVerticeIndex] = posB;
	currentLineVerticeIndex++;
}

void GLDebugDrawer::DrawDebugLine(const SVector3& from, const SVector3& to, const SVector3& fromColor)
{
	GLDebugDrawer::DrawDebugLine(from, to, fromColor, fromColor);
}

