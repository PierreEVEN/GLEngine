
#include <imgui.h>
#include <UI/EditorWindows/sceneComponentEditor.h>
#include <World/world.h>
#include <Engine/inputManager.h>
#include <MathLibrary/worldInteraction.h>
#include <Mesh/meshSectionComponent.h>
#include <Camera/camera.h>
#include <Mesh/staticMeshComponent.h>
#include <Scene/scene.h>

SceneComponentEditor::SceneComponentEditor(std::string inWindowName, World* inParentWorld)
	: ImFloatingWindow(inWindowName)
{
	parentWorld = inParentWorld;

	inParentWorld->GetInputManager()->AddInput(new KeyBind(inParentWorld->GetInputManager(), { Key(GLFW_MOUSE_BUTTON_1, true) }, ActionType_JustPress, this, &SceneComponentEditor::ClickInScene));
}

void SceneComponentEditor::ClickInScene()
{
	SVector3 traceDirection = parentWorld->GetScene()->PixelToWorldDirection(((AdvancedScene*)parentWorld->GetScene())->GetCursorPositionX(), ((AdvancedScene*)parentWorld->GetScene())->GetCursorPositionY());
	SHitResult result = WorldInteractionLibrary::TraceScene(parentWorld->GetScene(), parentWorld->GetScene()->GetCamera()->GetLocation(), parentWorld->GetScene()->GetCamera()->GetLocation() + traceDirection * 1000000);
	if (result.bHitComponent && result.hitSection->parentStaticMeshComponent)
	{
		SetAttachComponent(result.hitSection->parentStaticMeshComponent);
		return;
	}
	//SetAttachComponent(nullptr);
}

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
		DeleteSelectedComponent();
	}
	ImGui::End();
}

void SceneComponentEditor::DeleteSelectedComponent()
{
	if (component)
	{
		dynamic_cast<StaticMeshComponent*>(component)->DestroyComponent();
		SetAttachComponent(nullptr);
	}
}

void SceneComponentEditor::SetAttachComponent(SceneComponent* inComponent)
{
	//@TODO
}
