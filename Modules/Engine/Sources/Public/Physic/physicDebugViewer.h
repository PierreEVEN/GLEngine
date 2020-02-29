#pragma once

#include <vector>
#include <LinearMath/btIDebugDraw.h>

class World;
struct SVertex;
struct SVector3;
class Scene;

class GLDebugDrawer : public btIDebugDraw
{
	int m_debugMode;
	Scene* renderScene;
public:

	GLDebugDrawer(Scene* inRenderScene);
	virtual ~GLDebugDrawer();

	virtual void drawLine(const btVector3& from, const btVector3& to, const btVector3& fromColor, const btVector3& toColor);

	virtual void drawLine(const btVector3& from, const btVector3& to, const btVector3& color);

	virtual void drawContactPoint(const btVector3& PointOnB, const btVector3& normalOnB, btScalar distance, int lifeTime, const btVector3& color);

	virtual void reportErrorWarning(const char* warningString);

	virtual void draw3dText(const btVector3& location, const char* textString);

	virtual void setDebugMode(int debugMode);

	virtual int  getDebugMode() const { return m_debugMode; }

	void RenderDebugDraw(World* inRenderWorld);
	void RenderDebugDraw();
	void DrawDebugLine(const SVector3& from, const SVector3& to, const SVector3& fromColor, const SVector3& toColor);
	void DrawDebugLine(const SVector3& from, const SVector3& to, const SVector3& fromColor);
};