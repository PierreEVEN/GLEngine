#include "sceneComponentEditor.h"
#include "../../World/sceneComponent.h"
#include "../../World/world.h"
#include "../../Physic/primitiveComponent.h"

#include <bullet3D/btBulletDynamicsCommon.h>
#include <bullet3D/LinearMath/btIDebugDraw.h>
#include "../../Mesh/meshSectionComponent.h"
#include "../../Mesh/staticMeshComponent.h"
#include "../../Engine/Gizmo.h"

void SceneComponentEditor::Draw(World* inWorld)
{
	ImGui::Begin(windowTitle.data(), &bKeepOpen);
	if (!component)
	{
		ImGui::End();
		return;
	}
	component->DrawEditorWindow();
	
	if (ImGui::Button("Destroy item"))
	{
		DeleteSelectedComponent(inWorld);
	}
	ImGui::End();
}

void SceneComponentEditor::DeleteSelectedComponent(World* inWorld)
{
	if (component)
	{
		delete dynamic_cast<StaticMeshComponent*>(component);
		SetAttachComponent(inWorld, nullptr);
	}
}

void SceneComponentEditor::SetAttachComponent(World* inWorld, SceneComponent* inComponent)
{
	component = inComponent;
}
