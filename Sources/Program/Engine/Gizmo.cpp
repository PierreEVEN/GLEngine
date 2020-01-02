#include "Gizmo.h"
#include "../Physic/physicDebugViewer.h"
#include "../MathLibrary/vector3.h"
#include "../UI/EditorWindows/sceneComponentEditor.h"
#include "../Physic/primitiveComponent.h"
#include "../Mesh/staticMesh.h"
#include "../Shader/material.h"
#include "debugerTool.h"
#include <corecrt_math_defines.h>
#include "../Camera/camera.h"
#include "../MathLibrary/worldInteraction.h"
#include "../World/scene.h"
#include <glad/glad.h>
#include "../World/world.h"
#include "inputManager.h"
#include <glfw3/glfw3.h>

#define VERTICES_COUNT 57
#define TRIANGLES_COUNT 145
#define GIZMO_WIDTH .05f
#define GIZMO_LENGTH 2.f
#define GIZMO_ARROW_LENGTH .5f
#define GIZMO_ARROW_RADIUS .2f

Gizmo::Gizmo(EditorScene* inScene)
	: PrimitiveComponent(inScene)
{
	Vertex vertices[VERTICES_COUNT] = {
		//LineX (0, 5)
		Vertex(glm::vec3(-GIZMO_WIDTH,-GIZMO_WIDTH,-GIZMO_WIDTH), glm::vec4(1, 0, 0, 1)),
		Vertex(glm::vec3(GIZMO_WIDTH ,GIZMO_WIDTH ,-GIZMO_WIDTH), glm::vec4(1, 0, 0, 1)),
		Vertex(glm::vec3(GIZMO_LENGTH,GIZMO_WIDTH ,-GIZMO_WIDTH), glm::vec4(1, 0, 0, 1)),
		Vertex(glm::vec3(GIZMO_LENGTH,-GIZMO_WIDTH,-GIZMO_WIDTH), glm::vec4(1, 0, 0, 1)),
		Vertex(glm::vec3(GIZMO_WIDTH ,-GIZMO_WIDTH,GIZMO_WIDTH ), glm::vec4(1, 0, 0, 1)),
		Vertex(glm::vec3(GIZMO_LENGTH,-GIZMO_WIDTH,GIZMO_WIDTH ), glm::vec4(1, 0, 0, 1)),

		//LineY (6, 11)
		Vertex(glm::vec3(-GIZMO_WIDTH,-GIZMO_WIDTH,-GIZMO_WIDTH), glm::vec4(0, 1, 0, 1)),
		Vertex(glm::vec3(GIZMO_WIDTH ,GIZMO_WIDTH ,-GIZMO_WIDTH), glm::vec4(0, 1, 0, 1)),
		Vertex(glm::vec3(GIZMO_WIDTH ,GIZMO_LENGTH,-GIZMO_WIDTH), glm::vec4(0, 1, 0, 1)),
		Vertex(glm::vec3(-GIZMO_WIDTH,GIZMO_LENGTH,-GIZMO_WIDTH), glm::vec4(0, 1, 0, 1)),
		Vertex(glm::vec3(-GIZMO_WIDTH,GIZMO_WIDTH ,GIZMO_WIDTH ), glm::vec4(0, 1, 0, 1)),
		Vertex(glm::vec3(-GIZMO_WIDTH,GIZMO_LENGTH,GIZMO_WIDTH ), glm::vec4(0, 1, 0, 1)),

		//LineZ (12, 17)
		Vertex(glm::vec3(-GIZMO_WIDTH,-GIZMO_WIDTH,-GIZMO_WIDTH), glm::vec4(0, 0, 1, 1)),
		Vertex(glm::vec3(GIZMO_WIDTH ,-GIZMO_WIDTH,GIZMO_WIDTH ), glm::vec4(0, 0, 1, 1)),
		Vertex(glm::vec3(GIZMO_WIDTH ,-GIZMO_WIDTH,GIZMO_LENGTH), glm::vec4(0, 0, 1, 1)),
		Vertex(glm::vec3(-GIZMO_WIDTH,-GIZMO_WIDTH,GIZMO_LENGTH), glm::vec4(0, 0, 1, 1)),
		Vertex(glm::vec3(-GIZMO_WIDTH,GIZMO_WIDTH ,GIZMO_WIDTH ), glm::vec4(0, 0, 1, 1)),
		Vertex(glm::vec3(-GIZMO_WIDTH,GIZMO_WIDTH ,GIZMO_LENGTH), glm::vec4(0, 0, 1, 1)),

		//coneX (18, 30)
		Vertex(glm::vec3(0)),
		Vertex(glm::vec3(0)),
		Vertex(glm::vec3(0)),
		Vertex(glm::vec3(0)),
		Vertex(glm::vec3(0)),
		Vertex(glm::vec3(0)),
		Vertex(glm::vec3(0)),
		Vertex(glm::vec3(0)),
		Vertex(glm::vec3(0)),
		Vertex(glm::vec3(0)),
		Vertex(glm::vec3(0)),
		Vertex(glm::vec3(0)),
		Vertex(glm::vec3(GIZMO_ARROW_LENGTH + GIZMO_LENGTH,0,0), glm::vec4(1, 0, 0, 1)),

		//coneY (31, 43)
		Vertex(glm::vec3(0)),
		Vertex(glm::vec3(0)),
		Vertex(glm::vec3(0)),
		Vertex(glm::vec3(0)),
		Vertex(glm::vec3(0)),
		Vertex(glm::vec3(0)),
		Vertex(glm::vec3(0)),
		Vertex(glm::vec3(0)),
		Vertex(glm::vec3(0)),
		Vertex(glm::vec3(0)),
		Vertex(glm::vec3(0)),
		Vertex(glm::vec3(0)),
		Vertex(glm::vec3(0,GIZMO_ARROW_LENGTH + GIZMO_LENGTH,0), glm::vec4(0, 1, 0, 1)),

		//coneZ (44, 56)
		Vertex(glm::vec3(0)),
		Vertex(glm::vec3(0)),
		Vertex(glm::vec3(0)),
		Vertex(glm::vec3(0)),
		Vertex(glm::vec3(0)),
		Vertex(glm::vec3(0)),
		Vertex(glm::vec3(0)),
		Vertex(glm::vec3(0)),
		Vertex(glm::vec3(0)),
		Vertex(glm::vec3(0)),
		Vertex(glm::vec3(0)),
		Vertex(glm::vec3(0)),
		Vertex(glm::vec3(0,0,GIZMO_ARROW_LENGTH + GIZMO_LENGTH), glm::vec4(0, 0, 1, 1))
	};

	for (int i = 0; i < 12; ++i)
	{
		float step = (i / 12.f) * 2.f * (float)M_PI;
		vertices[i + 18] = Vertex(glm::vec3(GIZMO_LENGTH, cos(step) * GIZMO_ARROW_RADIUS, sin(step) * GIZMO_ARROW_RADIUS), glm::vec4(1, 0, 0, 1));
	}
	for (int i = 0; i < 12; ++i)
	{
		float step = (i / 12.f) * 2.f * (float)M_PI;
		vertices[i + 31] = Vertex(glm::vec3(cos(step) * GIZMO_ARROW_RADIUS, GIZMO_LENGTH, sin(step) * GIZMO_ARROW_RADIUS), glm::vec4(0, 1, 0, 1));
	}
	for (int i = 0; i < 12; ++i)
	{
		float step = (i / 12.f) * 2.f * (float)M_PI;
		vertices[i + 44] = Vertex(glm::vec3(cos(step) * GIZMO_ARROW_RADIUS, sin(step) * GIZMO_ARROW_RADIUS, GIZMO_LENGTH), glm::vec4(0, 0, 1, 1));
	}

	unsigned int triangles[TRIANGLES_COUNT] = {
		//LineX (0, 11)
		0, 1, 2,
		0, 2, 3,
		0, 4, 5,
		0, 5, 3,

		//LineY (12, 23)
		6, 7, 8,
		6, 8, 9,
		6, 10, 11,
		6, 11, 9,

		//LineZ (24, 35)
		12, 13, 14,
		12, 14, 15,
		12, 16, 17,
		12, 17, 15,

		//coneX (36, 71)
		0, 0, 0,
		0, 0, 0,
		0, 0, 0,
		0, 0, 0,
		0, 0, 0,
		0, 0, 0,
		0, 0, 0,
		0, 0, 0,
		0, 0, 0,
		0, 0, 0,
		0, 0, 0,
		0, 0, 0,

		//coneY (72, 107)
		0, 0, 0,
		0, 0, 0,
		0, 0, 0,
		0, 0, 0,
		0, 0, 0,
		0, 0, 0,
		0, 0, 0,
		0, 0, 0,
		0, 0, 0,
		0, 0, 0,
		0, 0, 0,
		0, 0, 0,

		//coneZ (108, 144)
		0, 0, 0,
		0, 0, 0,
		0, 0, 0,
		0, 0, 0,
		0, 0, 0,
		0, 0, 0,
		0, 0, 0,
		0, 0, 0,
		0, 0, 0,
		0, 0, 0,
		0, 0, 0,
		0, 0, 0
	};
	for (int i = 0; i < 12; ++i)
	{
		triangles[(3 * i + 36)] = 30;
		triangles[(3 * i + 36) + 1] = 18 + i;
		triangles[(3 * i + 36) + 2] = (i == 11) ? 18 : 18 + i + 1;
	}
	for (int i = 0; i < 12; ++i)
	{
		triangles[(3 * i + 72)] = 43;
		triangles[(3 * i + 72) + 1] = 31 + i;
		triangles[(3 * i + 72) + 2] = (i == 11) ? 31 : 31 + i + 1;
	}
	for (int i = 0; i < 12; ++i)
	{
		triangles[(3 * i + 108)] = 56;
		triangles[(3 * i + 108) + 1] = 44 + i;
		triangles[(3 * i + 108) + 2] = (i == 11) ? 44 : 44 + i + 1;
	}
	// create buffers/arrays
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);
	// load data into vertex buffers
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, VERTICES_COUNT * sizeof(Vertex), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, TRIANGLES_COUNT * sizeof(unsigned int), triangles, GL_STATIC_DRAW);
	

	// set the vertex attribute pointers
	// vertex Positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
	// vertex normals
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
	// vertex texture coords
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));
	// vertex tangent
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Tangent));
	// vertex bitangent
	glEnableVertexAttribArray(4);
	glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Bitangent));
	// vertex color
	glEnableVertexAttribArray(5);
	glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, VertexColor));
	glBindVertexArray(0);

	drawPriority = DrawPriority_Last;
	inScene->GetParentWorld()->GetInputManager()->AddInput(new KeyBind(inScene->GetParentWorld()->GetInputManager(), { Key(GLFW_MOUSE_BUTTON_1, true) }, ActionType_JustPress, this, & Gizmo::BeginDrag));
	inScene->GetParentWorld()->GetInputManager()->AddInput(new KeyBind(inScene->GetParentWorld()->GetInputManager(), { Key(GLFW_MOUSE_BUTTON_1, true) }, ActionType_JustRelease, this, & Gizmo::EndDrag));
	inScene->GetParentWorld()->GetInputManager()->BindOnMousePositionChanged(this, &Gizmo::Drag);
}

void Gizmo::Tick()
{
	SceneComponent::Tick();
	//if (!attachedComponent) return;
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	glm::mat4 gizmoTransform = glm::mat4(1.f);
	gizmoTransform = glm::translate(gizmoTransform, GetLocation().ToGLVector());
	gizmoTransform = glm::rotate(gizmoTransform, glm::radians(GetAngle()), GetForwardVector());
	gizmoTransform = glm::scale(gizmoTransform, GetScale3D().ToGLVector());

	//@TOFIX
	glm::vec3 traceDirection = GetScene()->PixelToWorldDirection(((EditorScene*)GetScene())->GetCursorPositionX(), ((EditorScene*)GetScene())->GetCursorPositionY()).ToGLVector();
	glm::vec3 xPos = glm::translate(gizmoTransform, glm::vec3(GIZMO_LENGTH, 0, 0))[3];
	glm::vec3 yPos = glm::translate(gizmoTransform, glm::vec3(0, GIZMO_LENGTH, 0))[3];
	glm::vec3 zPos = glm::translate(gizmoTransform, glm::vec3(0, 0, GIZMO_LENGTH))[3];

	float distanceX = (float)WorldInteractionLibrary::GetSegmentDistanceToLine(GetScene()->GetCamera()->GetCameraLocation(), traceDirection, GetLocation(), xPos);
	float distanceY = (float)WorldInteractionLibrary::GetSegmentDistanceToLine(GetScene()->GetCamera()->GetCameraLocation(), traceDirection, GetLocation(), yPos);
	float distanceZ = (float)WorldInteractionLibrary::GetSegmentDistanceToLine(GetScene()->GetCamera()->GetCameraLocation(), traceDirection, GetLocation(), zPos);

	MaterialEditorDebuger::GetGizmoMaterial()->use();
	MaterialEditorDebuger::GetGizmoMaterial()->setMat4("model", gizmoTransform);
	if (distanceX < GIZMO_ARROW_RADIUS || distanceY < GIZMO_ARROW_RADIUS || distanceZ < GIZMO_ARROW_RADIUS)
	{
		if (distanceX < distanceY && distanceX < distanceZ)
		{
			if (!bIsDraging) currentState = 1;
		}
		else if (distanceY < distanceZ && distanceY < distanceX)
		{
			if (!bIsDraging) currentState = 2;
		}		
		else
		{
			if (!bIsDraging) currentState = 3;
		}
	}
	else
	{
		if (!bIsDraging) currentState = 0;
	}
	MaterialEditorDebuger::GetGizmoMaterial()->setInt("state", currentState);
	/** Draw vertices */
	glBindVertexArray(VAO);
	StatViewer::AddDrawcall();
	glDrawElements(GL_TRIANGLES, TRIANGLES_COUNT, GL_UNSIGNED_INT, 0);

	/** Set GL to defaults */
	glBindVertexArray(0);
	glActiveTexture(GL_TEXTURE0);

	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
}

void Gizmo::SetCanDrag(bool bInDrag)
{
	if (bInDrag != bIsDraging)
	{
		bIsDraging = bInDrag;
	}
}

void Gizmo::BeginDrag()
{
	double GizmoDistance = (GetScene()->GetCamera()->GetCameraLocation() - GetLocation()).GetLength();
	double lastXPos = ((EditorScene*)GetScene())->GetParentWorld()->GetInputManager()->GetMouseLocationX() + ((EditorScene*)GetScene())->GetParentWorld()->GetInputManager()->GetMousePositionDeltaX();
	double lastYPos = ((EditorScene*)GetScene())->GetParentWorld()->GetInputManager()->GetMouseLocationY() + ((EditorScene*)GetScene())->GetParentWorld()->GetInputManager()->GetMousePositionDeltaY();
	double xPos = ((EditorScene*)GetScene())->GetParentWorld()->GetInputManager()->GetMouseLocationX();
	double yPos = ((EditorScene*)GetScene())->GetParentWorld()->GetInputManager()->GetMouseLocationY();
	SVector3 lastDir = GetScene()->PixelToWorldDirection((float)lastXPos, (float)lastYPos);
	SVector3 newDir = GetScene()->PixelToWorldDirection((float)xPos, (float)yPos);
	if ((float)WorldInteractionLibrary::GetPointDistanceToLine(lastDir, GetScene()->GetCamera()->GetCameraLocation(), GetLocation()) > GIZMO_LENGTH + GIZMO_ARROW_LENGTH) return;
	currentDragDirection = (DragType)currentState;
	SetCanDrag(true);
}

void Gizmo::EndDrag()
{
	SetCanDrag(false);
}

void Gizmo::AttachToComponent(SceneComponent* inComponent)
{
	if (inComponent)
	{
		SetLocation(inComponent->GetLocation());
	}
	attachedComponent = inComponent;
}

void Gizmo::Drag()
{
	if (!bIsDraging) return;
	SVector3 movementVector;
	double GizmoDistance = (GetScene()->GetCamera()->GetCameraLocation() - GetLocation()).GetLength();
	double lastXPos = ((EditorScene*)GetScene())->GetParentWorld()->GetInputManager()->GetMouseLocationX() + ((EditorScene*)GetScene())->GetParentWorld()->GetInputManager()->GetMousePositionDeltaX();
	double lastYPos = ((EditorScene*)GetScene())->GetParentWorld()->GetInputManager()->GetMouseLocationY() + ((EditorScene*)GetScene())->GetParentWorld()->GetInputManager()->GetMousePositionDeltaY();
	double xPos = ((EditorScene*)GetScene())->GetParentWorld()->GetInputManager()->GetMouseLocationX();
	double yPos = ((EditorScene*)GetScene())->GetParentWorld()->GetInputManager()->GetMouseLocationY();
	SVector3 lastDir = GetScene()->PixelToWorldDirection((float)lastXPos, (float)lastYPos);
	SVector3 newDir = GetScene()->PixelToWorldDirection((float)xPos, (float)yPos);


	movementVector = ((GetScene()->GetCamera()->GetCameraLocation() + newDir * GizmoDistance) - (GetScene()->GetCamera()->GetCameraLocation() + lastDir * GizmoDistance));

	if (currentDragDirection == DragType::DirX)
	{
		float offset = 1;// / (1 - abs(DotProduct(GetScene()->GetCamera()->GetCameraForwardVector(), GetForwardVector())));
		int signe = DotProduct(movementVector, GetForwardVector()) > 0 ? 1 : -1;
		movementVector = SVector3(GetForwardVector()) * movementVector.GetLength() * offset * signe;
	}
	else if (currentDragDirection == DragType::DirY)
	{
		float offset = 1;// / (1 - abs(DotProduct(GetScene()->GetCamera()->GetCameraForwardVector(), GetForwardVector())));
		int signe = DotProduct(movementVector, SVector3(0, 1, 0)) > 0 ? 1 : -1;
		movementVector = SVector3(SVector3(0, 1, 0)) * movementVector.GetLength() * offset * signe;
	}
	else if (currentDragDirection == DragType::DirZ)
	{
		float offset = 1;// / (1 - abs(DotProduct(GetScene()->GetCamera()->GetCameraForwardVector(), GetForwardVector())));
		int signe = DotProduct(movementVector, SVector3(0, 0, 1)) > 0 ? 1 : -1;
		movementVector = SVector3(SVector3(0, 0, 1)) * movementVector.GetLength() * offset * signe;
	}
	SetLocation(GetLocation() + movementVector);
	if (attachedComponent)
	{
		attachedComponent->SetLocation(GetLocation());
	}
}