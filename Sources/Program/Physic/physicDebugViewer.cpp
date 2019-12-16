#include "physicDebugViewer.h"
#include <glad/glad.h>
#include <glfw3/glfw3.h>
#include "../Shader/material.h"
#include "../World/world.h"
#include "../Camera/camera.h"
#include <glm/gtx/rotate_vector.inl>

GLDebugDrawer::GLDebugDrawer(World* inParentWorld)
{
	parentWorld = inParentWorld;
}

GLDebugDrawer::~GLDebugDrawer()
{

}

void GLDebugDrawer::drawLine(const btVector3& from, const btVector3& to, const btVector3& fromColor, const btVector3& toColor)
{
	GLuint VBO, VAO;
	// Vertex data
	GLfloat points[22];

	points[0] = from.x();
	points[1] = from.y();
	points[2] = from.z();
	points[3] = 0;
	points[4] = 0;
	points[5] = 0;
	points[6] = 0;
	points[7] = 0;
	points[8] = fromColor.x();
	points[9] = fromColor.y();
	points[10] = fromColor.z();

	points[11] = to.x();
	points[12] = to.y();
	points[13] = to.z();
	points[14] = 0;
	points[15] = 0;
	points[16] = 0;
	points[17] = 1;
	points[18] = 1;
	points[19] = toColor.x();
	points[20] = toColor.y();
	points[21] = toColor.z();

	glGenBuffers(1, &VBO);
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(points), &points, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), 0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glBindVertexArray(0);

	glm::mat4 model = glm::mat4(1.0f);	
	model = glm::translate(model, glm::vec3(0));
	model = glm::rotate(model, glm::radians(0.f), glm::vec3(1, 0, 0));
	model = glm::scale(model, glm::vec3(1.f));
	MaterialEditorDebuger::GetDebugMaterial()->use(parentWorld);
	MaterialEditorDebuger::GetDebugMaterial()->setMat4("view", parentWorld->GetCamera()->GetViewMatrix());
	MaterialEditorDebuger::GetDebugMaterial()->setMat4("projection", parentWorld->GetProjection());
	MaterialEditorDebuger::GetDebugMaterial()->setMat4("model", model);
	MaterialEditorDebuger::GetDebugMaterial()->setMat4("ambient", model);
	glBindVertexArray(VAO);
	glDrawArrays(GL_LINES, 0, 2);
	glBindVertexArray(0);

	glDeleteBuffers(1, &VBO);
	glDeleteVertexArrays(1, &VAO);
}

void GLDebugDrawer::drawLine(const btVector3& from, const btVector3& to, const btVector3& color)
{
	GLuint VBO, VAO;
	// Vertex data
	GLfloat points[22];

	points[0] = from.x();
	points[1] = from.y();
	points[2] = from.z();
	points[3] = 0;
	points[4] = 0;
	points[5] = 0;
	points[6] = 0;
	points[7] = 0;
	points[8] = color.x();
	points[9] = color.y();
	points[10] = color.z();

	points[11] = to.x();
	points[12] = to.y();
	points[13] = to.z();
	points[14] = 0;
	points[15] = 0;
	points[16] = 0;
	points[17] = 1;
	points[18] = 1;
	points[19] = color.x();
	points[20] = color.y();
	points[21] = color.z();

	glGenBuffers(1, &VBO);
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(points), &points, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), 0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glBindVertexArray(0);

	MaterialEditorDebuger::GetDebugMaterial()->use(parentWorld);
	glBindVertexArray(VAO);
	glDrawArrays(GL_LINES, 0, 2);
	glBindVertexArray(0);

	glDeleteBuffers(1, &VBO);
	glDeleteVertexArrays(1, &VAO);
}

void GLDebugDrawer::drawSphere(const btVector3& p, btScalar radius, const btVector3& color)
{

}

void GLDebugDrawer::drawTriangle(const btVector3& a, const btVector3& b, const btVector3& c, const btVector3& color, btScalar alpha)
{
	GLuint VBO, VAO;
	// Vertex data
	GLfloat points[22];

	points[0] = a.x();
	points[1] = a.y();
	points[2] = a.z();
	points[3] = 0;
	points[4] = 0;
	points[5] = 0;
	points[6] = 0;
	points[7] = 0;
	points[8] = color.x();
	points[9] = color.y();
	points[10] = color.z();

	points[11] = b.x();
	points[12] = b.y();
	points[13] = b.z();
	points[14] = 0;
	points[15] = 0;
	points[16] = 0;
	points[17] = 1;
	points[18] = 1;
	points[19] = color.x();
	points[20] = color.y();
	points[21] = color.z();

	points[11] = c.x();
	points[12] = c.y();
	points[13] = c.z();
	points[14] = 0;
	points[15] = 0;
	points[16] = 0;
	points[17] = 1;
	points[18] = 1;
	points[19] = color.x();
	points[20] = color.y();
	points[21] = color.z();

	glGenBuffers(1, &VBO);
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(points), &points, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), 0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glBindVertexArray(0);

	MaterialEditorDebuger::GetDebugMaterial()->use(parentWorld);
	glBindVertexArray(VAO);
	glDrawArrays(GL_LINES, 0, 2);
	glBindVertexArray(0);

	glDeleteBuffers(1, &VBO);
	glDeleteVertexArrays(1, &VAO);
}

void GLDebugDrawer::drawContactPoint(const btVector3& PointOnB, const btVector3& normalOnB, btScalar distance, int lifeTime, const btVector3& color)
{

}

void GLDebugDrawer::reportErrorWarning(const char* warningString)
{

}

void GLDebugDrawer::draw3dText(const btVector3& location, const char* textString)
{

}

void GLDebugDrawer::setDebugMode(int debugMode)
{
	m_debugMode = debugMode;
}

