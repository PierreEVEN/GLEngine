
#include <Shader/material.h>
#include <Asset/AssetRegistry.h>
#include <Shader/shaderLoader.h>
#include <Engine/debugerTool.h>
#include <Camera/camera.h>
#include <Mesh/StaticMesh.h>
#include <Mesh/staticMeshComponent.h>
#include <Texture/texture.h>
#include <Scene/scene.h>

GLuint ubo = 0;
Material* lastUsedMaterial = nullptr;

void Material::UpdateLinkedTextures()
{
	textures.clear();
	unsigned int textureCount = ((SUIntPropertyValue*)GetProperty("TextureCount"))->GetUIntValue();
	for (unsigned int i = 0; i < textureCount; ++i)
	{
		if (SPropertyValue* textureName = GetProperty("Texture_" + std::to_string(i)))
		{
			if (textureName->IsValueValid())
			{
				if (Texture2D* foundTexture = AssetRegistry::FindAssetByName<Texture2D>((((SAssetRefPropertyValue*)textureName))->GetAssetRef()))
				{
					textures.push_back(foundTexture);
				}
			}
			else
			{
				GFullLog(LogVerbosity::Warning, "Material", "Failed to read texture property " + textureName->GetPropertyName());
				textures.push_back(nullptr);
			}
		}
	}
}

Material::Material(std::string textAssetPath)
	: Asset(textAssetPath)
{
}

unsigned int Material::GetShaderID()
{
	CHECK_RENDER_THREAD;
	if (ShaderID == -1 && AreDataLoaded())
	{
		if (!compiler)
		{
			GFullLog(LogVerbosity::Assert, "Material", "invalid shader compiler");
		}
		ShaderID = compiler->Get();
	}
	return ShaderID;
}

void Material::LoadProperties()
{
	SAssetReader reader(GetPath());
	if (!reader.IsValid()) return;

	SFileRefPropertyValue* vertexShader = new SFileRefPropertyValue(this, reader.Get(), "VertexShaderFilePath");
	SFileRefPropertyValue* fragmentShader = new SFileRefPropertyValue(this, reader.Get(), "FragmentShaderFilePath");
	SUIntPropertyValue* textureCount = new SUIntPropertyValue(this, reader.Get(), "TextureCount");

	RegisterProperty(vertexShader);
	RegisterProperty(fragmentShader);
	RegisterProperty(textureCount);

	vertexShaderPath = vertexShader->GetFileRef();
	fragmentShaderPath = fragmentShader->GetFileRef();
	unsigned int texturesCount = textureCount->GetUIntValue();

	for (unsigned int i = 0; i < texturesCount; ++i)
	{
		SAssetRefPropertyValue* textureName = new SAssetRefPropertyValue(this, reader.Get(), "Texture_" + std::to_string(i));
		RegisterProperty(textureName);
	}
	UpdateLinkedTextures();

	compiler = new ShaderLoader(vertexShaderPath.data(), fragmentShaderPath.data());
	Asset::LoadProperties();
}

void Material::AddTexture(Texture2D* inTexture /*= nullptr*/)
{
	SAssetRefPropertyValue* newTextureRef = new SAssetRefPropertyValue(this, nullptr, "Texture_" + std::to_string(GetTextureCount()));
	if (inTexture)
	{
		newTextureRef->SetAssetRef(inTexture->GetName());
	}
	RegisterProperty(newTextureRef);

	SUIntPropertyValue* textureCount = ((SUIntPropertyValue*)GetProperty("TextureCount"));
	textureCount->SetUIntValue(textureCount->GetUIntValue() + 1);
}

unsigned int Material::GetTextureCount()
{
	return textures.size();
}

void Material::use()
{
	CHECK_RENDER_THREAD;
	if (AreDataLoaded())
	{
		if (lastUsedMaterial != this)
		{
			bool bIsComplete = false;
			{
				glUseProgram(GetShaderID());
				unsigned int block_index = glGetUniformBlockIndex(GetShaderID(), "shader_data");
				GLuint binding_point_index = 0;
				glBindBufferBase(GL_UNIFORM_BUFFER, binding_point_index, ubo);
				glUniformBlockBinding(GetShaderID(), block_index, binding_point_index);
			}

			if (textures.size() > 0)
			{
				for (unsigned int textureIndex = 0; textureIndex < textures.size(); ++textureIndex)
				{
					if (textures[textureIndex])
					{
						if (textures[textureIndex]->AreDataLoaded())
						{
							if (dynamic_cast<TextureCube*>(textures[textureIndex]))
							{
								setInt(std::string("Cubemap_") + std::to_string(textureIndex), textureIndex);
								glActiveTexture(GL_TEXTURE0 + textureIndex);
								glBindTexture(GL_TEXTURE_CUBE_MAP, textures[textureIndex]->GetTextureID());
							}
							else
							{
								setInt(std::string("StaticTexture_") + std::to_string(textureIndex), textureIndex);
								glActiveTexture(GL_TEXTURE0 + textureIndex);
								glBindTexture(GL_TEXTURE_2D, textures[textureIndex]->GetTextureID());
							}
							bIsComplete = true;
						}
						else
						{
							textures[textureIndex]->CheckData(true);
						}
					}
				}
			}
			else
			{
				bIsComplete = true;
			}
			if (bIsComplete) lastUsedMaterial = this;
		}
	}
	else
	{
		CheckData(true);
	}
}

void Material::InitializeMaterials()
{
	DefaultShaderData shader_data;
	glGenBuffers(1, &ubo);
	glBindBuffer(GL_UNIFORM_BUFFER, ubo);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(shader_data), &shader_data, GL_DYNAMIC_COPY);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void Material::UpdateMaterialDefaults(Scene* DrawScene)
{
	DefaultShaderData shader_data;
	shader_data.viewMatrix = DrawScene->GetCamera()->GetViewMatrix();
	shader_data.worldProjection = DrawScene->GetProjection();
	shader_data.cameraLocation = glm::vec4(DrawScene->GetCamera()->GetLocation().ToGLVector(), 0);

	std::vector<PointLight*> pointLights = {};
	std::vector<DirectionalLight*> directionalLights = {};
	std::vector<SpotLight*> spotLights = {};
	for (auto& element : DrawScene->GetSceneComponents()) if (dynamic_cast<DirectionalLight*>(element)) directionalLights.push_back(dynamic_cast<DirectionalLight*>(element));
	for (auto& element : DrawScene->GetSceneComponents()) if (dynamic_cast<PointLight*>(element))  pointLights.push_back(dynamic_cast<PointLight*>(element));
	for (auto& element : DrawScene->GetSceneComponents()) if (dynamic_cast<SpotLight*>(element))  spotLights.push_back(dynamic_cast<SpotLight*>(element));
	shader_data.directionalLightCount = directionalLights.size();
	shader_data.pointLightCount = pointLights.size();
	shader_data.spotLightCount = spotLights.size();
	for (unsigned int i = 0; i < directionalLights.size() && i < MAX_DIRECTIONAL_LIGHTS; ++i)
	{
		shader_data.directionalLightBaseParams[i] = directionalLights[i]->lightParams;
		shader_data.directionalLightAdvancedParams[i] = directionalLights[i]->directionalLightParams;
	}
	for (unsigned int i = 0; i < pointLights.size() && i < MAX_POINT_LIGHTS; ++i)
	{
		shader_data.pointLightBaseParameters[i] = pointLights[i]->lightParams;
		shader_data.pointLightAdvancedParameters[i] = pointLights[i]->pointLightParams;
		shader_data.pointLightAdvancedParameters[i].lightPosition = glm::vec4(pointLights[i]->GetLocation().ToGLVector(), 0.f);
	}
	for (unsigned int i = 0; i < spotLights.size() && i < MAX_SPOT_LIGHTS; ++i)
	{
		shader_data.spotLightBaseParameters[i] = spotLights[i]->lightParams;
		shader_data.spotLightAdvancedParameters[i] = spotLights[i]->spotLightParams;
		shader_data.spotLightAdvancedParameters[i].lightPosition = glm::vec4(spotLights[i]->GetLocation().ToGLVector(), 0.f);
		shader_data.spotLightAdvancedParameters[i].direction = glm::vec4(0, 1, 0, 0);
	}
	glBindBuffer(GL_UNIFORM_BUFFER, ubo);
	GLvoid* p = glMapBuffer(GL_UNIFORM_BUFFER, GL_WRITE_ONLY);
	memcpy(p, &shader_data, sizeof(shader_data));
	glUnmapBuffer(GL_UNIFORM_BUFFER);
}

void Material::setBool(const std::string &name, bool value)
{
	StatViewer::AddDrawcall();
	glUniform1i(glGetUniformLocation(GetShaderID(), name.c_str()), (int)value);
}

void Material::setInt(const std::string &name, int value)
{
	StatViewer::AddDrawcall();
	glUniform1i(glGetUniformLocation(GetShaderID(), name.c_str()), value);
}

void Material::setFloat(const std::string &name, float value)
{
	StatViewer::AddDrawcall();
	glUniform1f(glGetUniformLocation(GetShaderID(), name.c_str()), value);
}

void Material::setVec2(const std::string &name, const glm::vec2 &value)
{
	StatViewer::AddDrawcall();
	glUniform2fv(glGetUniformLocation(GetShaderID(), name.c_str()), 1, &value[0]);
}

void Material::setVec2(const std::string &name, float x, float y)
{
	StatViewer::AddDrawcall();
	glUniform2f(glGetUniformLocation(GetShaderID(), name.c_str()), x, y);
}

void Material::setVec3(const std::string &name, const glm::vec3 &value)
{
	StatViewer::AddDrawcall();
	glUniform3fv(glGetUniformLocation(GetShaderID(), name.c_str()), 1, &value[0]);
}

void Material::setVec3(const std::string &name, float x, float y, float z)
{
	StatViewer::AddDrawcall();
	glUniform3f(glGetUniformLocation(GetShaderID(), name.c_str()), x, y, z);
}

void Material::setVec4(const std::string &name, const glm::vec4 &value)
{
	StatViewer::AddDrawcall();
	glUniform4fv(glGetUniformLocation(GetShaderID(), name.c_str()), 1, &value[0]);
}

void Material::setVec4(const std::string &name, float x, float y, float z, float w)
{
	StatViewer::AddDrawcall();
	glUniform4f(glGetUniformLocation(GetShaderID(), name.c_str()), x, y, z, w);
}

void Material::setMat2(const std::string &name, const glm::mat2 &mat)
{
	StatViewer::AddDrawcall();
	glUniformMatrix2fv(glGetUniformLocation(GetShaderID(), name.c_str()), 1, GL_FALSE, &mat[0][0]);
}

void Material::setMat3(const std::string &name, const glm::mat3 &mat)
{
	StatViewer::AddDrawcall();
	glUniformMatrix3fv(glGetUniformLocation(GetShaderID(), name.c_str()), 1, GL_FALSE, &mat[0][0]);
}

void Material::setMat4(const std::string &name, const glm::mat4 &mat)
{
	StatViewer::AddDrawcall();
	glUniformMatrix4fv(glGetUniformLocation(GetShaderID(), name.c_str()), 1, GL_FALSE, &mat[0][0]);
}

ImColor Material::GetAssetColor()
{
	return ImColor(0.3f, 0.6f, 0.3f, 1.f);
}

void Material::OnAssetClicked()
{
	//@Todo : new MaterialEditorWindow(GetName(), this);
}

void Material::SaveAsset()
{
	UpdateLinkedTextures();
	Asset::SaveAsset();
}

void Material::BuildThumbnail()
{
	CheckData();
	SPropertyValue* thumbnailProperty = GetBaseProperty("thumbnailTexture");
	Scene* thumbnailScene = new AdvancedScene();
	thumbnailScene->InitializeSceneGT();
	AStaticMesh* sphereMesh = AssetRegistry::FindAssetByName<AStaticMesh>("SphereMesh");
	sphereMesh->CheckData();
	for (auto& texture : textures)
	{
		texture->CheckData();
	}
	StaticMeshComponent* comp = new StaticMeshComponent(thumbnailScene, sphereMesh, { this });
	comp->SetRotation(SRotator(90, 0, 0));
	comp->RebuildTransformData();
	thumbnailScene->Render();
	thumbnailScene->Render();
	thumbnailScene->Render();
	DirectionalLight* dirLight = new DirectionalLight(thumbnailScene);
	dirLight->lightParams.ambiant = glm::vec4(1);
	thumbnailScene->GetCamera()->Pitch = 45;
	thumbnailScene->GetCamera()->Yaw = 35;
	thumbnailScene->GetCamera()->updateCameraVectors();
	thumbnailScene->GetCamera()->SetLocation(comp->GetWorldBounds().GetOrigin() + thumbnailScene->GetCamera()->GetRotation().GetForwardVector() * comp->GetWorldBounds().GetBoundRadius() * -1.6);
	thumbnailScene->UpdateFramebufferSize(THUMBNAIL_RESOLUTION, THUMBNAIL_RESOLUTION);
	thumbnailScene->Render();
	thumbnailScene->Render();
	thumbnailScene->Render();
	thumbnailScene->Render();
	float* renderTexture = (float*)malloc(THUMBNAIL_RESOLUTION * THUMBNAIL_RESOLUTION * 3 * sizeof(float));
	glBindTexture(GL_TEXTURE_2D, thumbnailScene->GetColorBuffer());
	glGetTexImage(GL_TEXTURE_2D, 0, GL_RGB, GL_FLOAT, renderTexture);
	thumbnailProperty->SetValue<float>(renderTexture, THUMBNAIL_RESOLUTION * THUMBNAIL_RESOLUTION * 3 * sizeof(float));
	free(renderTexture);
	delete thumbnailScene;
}

Material* debugMaterial = nullptr;
Material* gridMaterial = nullptr;
Material* gizmoMaterial = nullptr;

Material* MaterialEditorDebuger::GetDebugMaterial()
{
	if (!debugMaterial)
	{
		debugMaterial = AssetRegistry::FindAssetByName<Material>("DebugViewMaterial");
		assert(debugMaterial);
	}
	return debugMaterial;
}

Material* MaterialEditorDebuger::GetGridMaterial()
{
	if (!gridMaterial)
	{
		gridMaterial = AssetRegistry::FindAssetByName<Material>("DebugGridMaterial");
		assert(gridMaterial);
	}
	return gridMaterial;
}

Material* MaterialEditorDebuger::GetGizmoMaterial()
{
	if (!gizmoMaterial)
	{
		gizmoMaterial = AssetRegistry::FindAssetByName<Material>("GizmoMaterial");
		assert(gizmoMaterial);
	}
	return gizmoMaterial;
}
