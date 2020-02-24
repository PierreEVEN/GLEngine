#include <Mesh/CubemapComponent.h>
#include <Shader/material.h>
#include <Mesh/staticMesh.h>

CubemapComponent::CubemapComponent(Scene* inScene, StaticMeshComponent* inParentStaticMeshComponent, MeshSectionData* inStaticMeshSection)
	: MeshSectionComponent(inScene, inParentStaticMeshComponent, inStaticMeshSection)
{
	drawPriority = DrawPriority::DrawPriority_First;
}

void CubemapComponent::Tick()
{
	PrimitiveComponent::Tick();
	glDepthMask(GL_FALSE);
	glm::mat4 model = glm::mat4(1.0f);
	model = model * glm::mat4_cast(glm::quat((float)GetRotation().w, (float)GetRotation().x, (float)GetRotation().y, (float)GetRotation().z));
	model = glm::scale(model, GetScale3D().ToGLVector());

	Material* usedMaterial = materialOverride ? materialOverride : GetSectionData()->material;
	if (usedMaterial)
	{
		usedMaterial->use();

		/** Set materials commons */
		usedMaterial->setMat4("model", model);
	}
	else
	{
		MaterialEditorDebuger::GetGridMaterial()->use();
	}

	{
		/** Draw vertices */
		ProfileStat("vertex rendering");
		glBindVertexArray(VAO);
		StatViewer::AddDrawcall();
		if (GetSectionData()->indicesCount > 0)
			glDrawElements(GL_TRIANGLES, GetSectionData()->indicesCount, GL_UNSIGNED_INT, 0);
		else
			glDrawArrays(GL_TRIANGLES, 0, GetSectionData()->verticesCount);
	}
	/** Set GL to defaults */
	glBindVertexArray(0);
	glActiveTexture(GL_TEXTURE0);
	glDepthMask(GL_TRUE);
}
