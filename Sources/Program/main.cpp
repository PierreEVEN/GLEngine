

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
	double MaxFPS = 60.0;

	/************************************************************************/
	/* World initialization                                                 */
	/************************************************************************/

	World* WorldOne = new World("Main");

	assert(gladLoadGLLoader((GLADloadproc)glfwGetProcAddress));

	AssetLibrary::LoadLibraryFiles("GLEngine/Sources/Assets");


	/************************************************************************/
	/* World generation                                                     */
	/************************************************************************/


	StaticMeshComponent* campusMeshComp = new StaticMeshComponent(WorldOne, AssetLibrary::FindAssetByName<StaticMesh>("CampusMesh"));
	campusMeshComp->SetLocation(glm::vec3(0,-.7, 0));
	campusMeshComp->SetScale3D(glm::vec3(10.f));

	StaticMeshComponent* Comp2 = new StaticMeshComponent(WorldOne, AssetLibrary::FindAssetByName<StaticMesh>("CubeMesh"));
	Comp2->SetLocation(glm::vec3(0, -4.2, 0));
	Comp2->SetScale3D(glm::vec3(2000.f, 0.1f, 2000.f));

	glEnable(GL_DEPTH_TEST);

	// render loop
	// -----------
	while (!glfwWindowShouldClose(WorldOne->GetWindow()))
	{
		if (glfwGetTime() - LastTime < 1 / MaxFPS) continue;
		DeltaSecond = glfwGetTime() - LastTime;
		if (DeltaSecond > 1 / 10.f)
		{
			DeltaSecond = 1 / 10.f;
		}
		LastTime = glfwGetTime();
		
		World::UpdateWorlds(DeltaSecond);

	}

	glfwTerminate();
	return 0;
}