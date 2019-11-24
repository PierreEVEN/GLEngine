

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
#include "Mesh/primitiveComponent.h"
#include "World/world.h"
#include "Mesh/staticMesh.h"
#include "Texture/texture.h"

StaticMesh* GLobalMesh;

double DeltaSecond;

int main()
{
	// glfw: initialize and configure
	// ------------------------------
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // uncomment this statement to fix compilation on OS X
#endif

	/************************************************************************/
	/* GLFW window creation                                                 */
	/************************************************************************/

	World* WorldOne = new World();

	/************************************************************************/
	/* Glad initialization                                                  */
	/************************************************************************/
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	/************************************************************************/
	/* Textures building / compilation                                      */
	/************************************************************************/

	Texture2D* defaultTexture = new Texture2D("Sources/Ressources/grass-with-little-snow-pbr-texture-02.png");
	Texture2D* texture2 = new Texture2D("Sources/Ressources/container.jpg");

	/************************************************************************/
	/* Shaders building / compilation                                       */
	/************************************************************************/

	Material* defaultMaterial = new Material("Sources/Shaders/defaultVertexShader.vs", "Sources/Shaders/defaultFragmentShaderss.fs", { defaultTexture });

	Material* secondMaterial = new Material("Sources/Shaders/defaultVertexShader.vs", "Sources/Shaders/defaultFragmentShaderss.fs", { texture2 });

	/************************************************************************/
	/* Mesh loading                                                         */
	/************************************************************************/

	StaticMesh* cubeMesh = new StaticMesh("Sources/Ressources/test.obj", { defaultMaterial });
	StaticMesh* cubeMesh2 = new StaticMesh("Sources/Ressources/cube.obj", { secondMaterial });

	PrimitiveComponent* Comp0 = new PrimitiveComponent(WorldOne, &cubeMesh->meshSections[0]);
	Comp0->SetLocation(glm::vec3(0,2,0));

	PrimitiveComponent* Comp1 = new PrimitiveComponent(WorldOne, &cubeMesh2->meshSections[0]);
	Comp1->SetLocation(glm::vec3(3, -2, 0));

	PrimitiveComponent* Comp2 = new PrimitiveComponent(WorldOne, &cubeMesh2->meshSections[0]);
	Comp2->SetLocation(glm::vec3(0, -4.2, 0));
	Comp2->SetScale3D(glm::vec3(200.f, 0.1f, 200.f));

	glEnable(GL_DEPTH_TEST);

	double LastTime = 0.0;
	double MaxFPS = 60.0;

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

// 		glActiveTexture(GL_TEXTURE0);
// 		glBindTexture(GL_TEXTURE_2D, texture1);
// 		glActiveTexture(GL_TEXTURE1);
// 		glBindTexture(GL_TEXTURE_2D, texture2);
		
		World::UpdateWorlds(DeltaSecond);
	}

	// optional: de-allocate all resources once they've outlived their purpose:
	// ------------------------------------------------------------------------
	
	delete defaultMaterial;
	delete cubeMesh;
	delete WorldOne;

	// glfw: terminate, clearing all previously allocated GLFW resources.
	// ------------------------------------------------------------------
	glfwTerminate();
	return 0;
}