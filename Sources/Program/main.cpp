#include <glad/glad.h>
#include <glfw3/glfw3.h>
#include "World/world.h"
#include "Asset/assetLibrary.h"
#include <bullet3D/btBulletDynamicsCommon.h>
#include "ImGUI/imgui.h"
#include "ImGUI/imgui_impl_opengl3.h"
#include "ImGUI/imgui_impl_glfw.h"
#include "Mesh/staticMesh.h"
#include "Mesh/staticMeshComponent.h"
#include "Asset/AssetRegistry.h"
#include "UI/FileExplorer/fileExplorer.h"

double DeltaSecond;

int main()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // uncomment this statement to fix compilation on OS X
#endif
	   
	double LastTime = 0.0;
	double MaxFPS = 20000.0;

	/************************************************************************/
	/* World initialization                                                 */
	/************************************************************************/

	World* WorldOne = new World("Main");
	assert(gladLoadGLLoader((GLADloadproc)glfwGetProcAddress));
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_STENCIL_TEST);
	glEnable(GL_CULL_FACE);

	AssetRegistry::ImportAssetFromDirectory("Sources/Assets");


	/************************************************************************/
	/* PHYSIC generation                                                    */
	/************************************************************************/
	

	btCollisionShape* shape = new btBoxShape(btVector3(100, 100, 1));
	btTransform myTransform;
	myTransform.setIdentity();
	myTransform.setOrigin(btVector3(0, 0, -1));
	btVector3 localInertia(0, 0, 0);
	btScalar mass = 0;
	shape->calculateLocalInertia(mass, localInertia);
	btDefaultMotionState* myMotionState = new btDefaultMotionState(myTransform);
	btRigidBody::btRigidBodyConstructionInfo myBoxRigidBodyConstructionInfo(mass, myMotionState, shape, localInertia);
	btRigidBody *body = new btRigidBody(myBoxRigidBodyConstructionInfo);
	WorldOne->GetPhysicWorld()->addRigidBody(body);


	/************************************************************************/
	/* IMGUI Initialization                                                 */
	/************************************************************************/
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO &io = ImGui::GetIO();
	ImGui_ImplGlfw_InitForOpenGL(WorldOne->GetWindow(), true);
	ImGui_ImplOpenGL3_Init("#version 130");
	// Setup Dear ImGui style
	ImGui::StyleColorsDark();

	if (StaticMesh* foundMesh = AssetRegistry::FindAssetByName<StaticMesh>("Cube1M"))
	{
		StaticMeshComponent* newComp0 = new StaticMeshComponent(WorldOne, foundMesh);
	}
	else
	{
		std::cout << "ERROR : failed to find mesh " << std::endl;
	}


	new FileExplorer(".");

	/************************************************************************/
	/* RENDER loop                                                          */
	/************************************************************************/

	while (!glfwWindowShouldClose(WorldOne->GetWindow()))
	{
		//if (glfwGetTime() - LastTime < 1.0 / MaxFPS) continue;
		DeltaSecond = glfwGetTime() - LastTime;
		if (DeltaSecond > 1.0 / 10.0)
		{
			DeltaSecond = 1.0 / 10.f;
		}
		LastTime = glfwGetTime();

		World::UpdateWorlds(DeltaSecond);
	}

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	glfwTerminate();
	return 0;
}