

#include <glad/glad.h>
#include <glfw3/glfw3.h>
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/ext/matrix_float4x4.hpp>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/gtc/type_ptr.hpp>


#include "ThirdParty/stb_image.h"
#include "Camera/camera.h"
#include "Shader/material.h"

#include <iostream>
#include "World/world.h"
#include "Mesh/staticMesh.h"
#include "Texture/texture.h"
#include "Mesh/meshSectionComponent.h"
#include "Lighting/pointLight.h"
#include "Mesh/staticMeshComponent.h"
#include "Asset/assetLibrary.h"



#include <bullet3D/btBulletDynamicsCommon.h>
#include "ImGUI/imgui.h"
#include "ImGUI/imgui_impl_opengl3.h"
#include "ImGUI/imgui_impl_glfw.h"
#include "Lighting/directionalLight.h"
#include "Asset/assetSerializer.h"
#include "Asset/AssetImporter.h"

double DeltaSecond;

int main()
{
	AssetSerializer::TestDebug();

	//while(true) {}



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

	AssetLibrary::RegisterAssetFiles("GLEngine/Sources/Assets");


	/************************************************************************/
	/* World generation                                                     */
	/************************************************************************/

// 	StaticMeshComponent* campusMeshComp = new StaticMeshComponent(WorldOne, AssetLibrary::FindAssetByName<StaticMesh>("CampusMesh"));
// 	campusMeshComp->SetLocation(glm::vec3(0, 20, 0));
// 	campusMeshComp->SetScale3D(glm::vec3(50.f));

// 	StaticMeshComponent* Comp2 = new StaticMeshComponent(WorldOne, AssetLibrary::FindAssetByName<StaticMesh>("CubeMesh"));
// 	Comp2->SetLocation(glm::vec3(0, 0, 0));
// 	Comp2->SetScale3D(glm::vec3(2000.f, 2000.f, 0.1f));
// 
// 	StaticMeshComponent* testCube = new StaticMeshComponent(WorldOne, AssetLibrary::FindAssetByName<StaticMesh>("CubeMesh"));
// 	testCube->SetLocation(glm::vec3(0, 0, 3));
// 	testCube->SetScale3D(glm::vec3(1.f, 1.f, 1.f));
// 
// 	StaticMeshComponent* testCube2 = new StaticMeshComponent(WorldOne, AssetLibrary::FindAssetByName<StaticMesh>("CubeMesh"));
// 	testCube->SetLocation(glm::vec3(4, 0, 3));
// 	testCube->SetScale3D(glm::vec3(1.f, 1.f, 1.f));
// 
// 	DirectionalLight* directionalLight = new DirectionalLight(WorldOne);
// 	directionalLight->SetForwardVector(glm::vec3(0, 0, -1));

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_STENCIL_TEST);
	glEnable(GL_CULL_FACE);

	
	btCollisionShape* shape = new btBoxShape(btVector3(100, 100, 1));

	btTransform myTransform;
	myTransform.setIdentity();
	myTransform.setOrigin(btVector3(0, 0, -1));

	btVector3 localInertia(0, 0, 0);
	btScalar mass = 0;
	shape->calculateLocalInertia(mass, localInertia);

	//using motionstate is recommended, it provides interpolation capabilities, and only synchronizes 'active' objects
	btDefaultMotionState* myMotionState = new btDefaultMotionState(myTransform);
	btRigidBody::btRigidBodyConstructionInfo myBoxRigidBodyConstructionInfo(mass, myMotionState, shape, localInertia);
	btRigidBody *body = new btRigidBody(myBoxRigidBodyConstructionInfo);

	WorldOne->GetPhysicWorld()->addRigidBody(body);


	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO &io = ImGui::GetIO();
	// Setup Platform/Renderer bindings
	ImGui_ImplGlfw_InitForOpenGL(WorldOne->GetWindow(), true);
	ImGui_ImplOpenGL3_Init("#version 130");
	// Setup Dear ImGui style
	ImGui::StyleColorsDark();

	// render loop
	// -----------
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