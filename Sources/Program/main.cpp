

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

StaticMesh* GLobalMesh;

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
	/* GLFW window creation                                                 */
	/************************************************************************/

	World* WorldOne = new World();

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	/************************************************************************/
	/* Textures building / compilation                                      */
	/************************************************************************/

	Texture2D* defaultTexture_A = new Texture2D("GLEngine/Sources/Ressources/metal_plate_diff_1k.jpg");
	Texture2D* defaultTexture_S = new Texture2D("GLEngine/Sources/Ressources/TexturesCom_MetalBare0234_1_seamless_S.jpg");
	Texture2D* texture2 = new Texture2D("GLEngine/Sources/Ressources/metal_plate_diff_1k.jpg");

	/************************************************************************/
	/* Shaders building / compilation                                       */
	/************************************************************************/

	Material* defaultMaterial = new Material("GLEngine/Sources/Shaders/defaultVertexShader.vs", "GLEngine/Sources/Shaders/defaultFragmentShaderss.fs", { defaultTexture_S });
	Material* secondMaterial = new Material("GLEngine/Sources/Shaders/defaultVertexShader.vs", "GLEngine/Sources/Shaders/defaultFragmentShaderss.fs", { texture2 });

	Material* lampShaders = new Material("GLEngine/Sources/Shaders/defaultVertexShader.vs", "GLEngine/Sources/Shaders/lampFragmentShaders.fs", { texture2 });

	/************************************************************************/
	/* Mesh loading                                                         */
	/************************************************************************/

	StaticMesh* cubeMesh = new StaticMesh("GLEngine/Sources/Ressources/TransporterBasic.obj", { defaultMaterial });
	StaticMesh* cubeMesh2 = new StaticMesh("GLEngine/Sources/Ressources/cube.obj", { secondMaterial });
	StaticMesh* lightBuble = new StaticMesh("GLEngine/Sources/Ressources/LightBuble.obj", { lampShaders });




	MeshSectionComponent* Comp0 = new MeshSectionComponent(WorldOne, &cubeMesh->meshSections[0]);
	Comp0->SetLocation(glm::vec3(0,-3.7, 0));
	Comp0->SetScale3D(glm::vec3(2.f));

	MeshSectionComponent* Comp1 = new MeshSectionComponent(WorldOne, &cubeMesh2->meshSections[0]);
	Comp1->SetLocation(glm::vec3(3, -3.2, 5));

	MeshSectionComponent* Comp2 = new MeshSectionComponent(WorldOne, &cubeMesh2->meshSections[0]);
	Comp2->SetLocation(glm::vec3(0, -4.2, 0));
	Comp2->SetScale3D(glm::vec3(200.f, 0.1f, 200.f));

	StaticMeshComponent* LightBubleMesh = new StaticMeshComponent(WorldOne, lightBuble);


	PointLight* Light = new PointLight(WorldOne);

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
		
		Light->SetLocation(glm::vec3(sin(glfwGetTime() * .2f), 0.f, cos(glfwGetTime() * .2f)) * 40.f);
		LightBubleMesh->SetLocation(Light->GetLocation());

		secondMaterial->use();
		secondMaterial->setVec3("light.position", Light->GetLocation());
		secondMaterial->setVec3("viewPos", WorldOne->GetCamera()->GetCameraLocation());

		// light properties
		glm::vec3 lightColor = glm::vec3(1.f);
		lightColor.x = sin(glfwGetTime() * 2.f) / 10.f + .75f;
		lightColor.y = sin(glfwGetTime() * 2.f) / 10.f + .75f;
		lightColor.z = sin(glfwGetTime() * 2.f) / 10.f + .75f;

		lampShaders->setVec3("lightColor", lightColor);

		glm::vec3 diffuseColor = lightColor * glm::vec3(5.f); // decrease the influence
		glm::vec3 ambientColor = diffuseColor * glm::vec3(0.2f); // low influence
		secondMaterial->setVec3("light.ambient", ambientColor);
		secondMaterial->setVec3("light.diffuse", diffuseColor);
		secondMaterial->setVec3("light.specular", 1.0f, 1.0f, 1.0f);

		// material properties
		secondMaterial->setVec3("material.ambient", 1.0f, 0.5f, 0.31f);
		secondMaterial->setVec3("material.diffuse", 1.0f, 0.5f, 0.31f);
		secondMaterial->setVec3("material.specular", 0.5f, 0.5f, 0.5f); // specular lighting doesn't have full effect on this object's material
		secondMaterial->setFloat("material.shininess", 32.0f);






		defaultMaterial->use();
		defaultMaterial->setVec3("light.position", Light->GetLocation());
		defaultMaterial->setVec3("viewPos", WorldOne->GetCamera()->GetCameraLocation());

		// light properties

		diffuseColor = lightColor * glm::vec3(5.f); // decrease the influence
		ambientColor = diffuseColor * glm::vec3(0.2f); // low influence
		defaultMaterial->setVec3("light.ambient", ambientColor);
		defaultMaterial->setVec3("light.diffuse", diffuseColor);
		defaultMaterial->setVec3("light.specular", 1.0f, 1.0f, 1.0f);

		// material properties
		defaultMaterial->setVec3("material.ambient", 1.0f, 0.5f, 0.31f);
		defaultMaterial->setVec3("material.diffuse", 1.0f, 0.5f, 0.31f);
		defaultMaterial->setVec3("material.specular", 0.5f, 0.5f, 0.5f); // specular lighting doesn't have full effect on this object's material
		defaultMaterial->setFloat("material.shininess", 32.0f);


		World::UpdateWorlds(DeltaSecond);

		defaultMaterial->setVec3("light.position", Light->GetLocation());
		defaultMaterial->setVec3("viewPos", WorldOne->GetCamera()->GetCameraLocation());

		// light properties

		diffuseColor = lightColor * glm::vec3(5.f); // decrease the influence
		ambientColor = diffuseColor * glm::vec3(0.2f); // low influence
		defaultMaterial->setVec3("light.ambient", ambientColor);
		defaultMaterial->setVec3("light.diffuse", diffuseColor);
		defaultMaterial->setVec3("light.specular", 1.0f, 1.0f, 1.0f);

		// material properties
		defaultMaterial->setVec3("material.ambient", 1.0f, 0.5f, 0.31f);
		defaultMaterial->setVec3("material.diffuse", 1.0f, 0.5f, 0.31f);
		defaultMaterial->setVec3("material.specular", 0.5f, 0.5f, 0.5f); // specular lighting doesn't have full effect on this object's material
		defaultMaterial->setFloat("material.shininess", 32.0f);


	}

	glfwTerminate();
	return 0;
}