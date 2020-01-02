
#include "camera.h"
#include <glfw3/glfw3.h>
#include "../World/scene.h"
#include "../World/world.h"
#include "../Engine/inputManager.h"

Camera::Camera(Scene* inRenderScene, SVector3 position, SVector3 up, float yaw, float pitch)
	: renderScene(inRenderScene), Front(SVector3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
{
	Position = position;
	WorldUp = up;
	Yaw = yaw;
	Pitch = pitch;
	bDoesCaptureMouse = false;
	LastMouseDisplayChangeTime = 0.0;
	updateCameraVectors();
}

Camera::Camera(Scene* inRenderScene, float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch)
	: renderScene(inRenderScene), Front(SVector3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
{
	Position = SVector3(posX, posY, posZ);
	WorldUp = SVector3(upX, upY, upZ);
	Yaw = yaw;
	Pitch = pitch;
	bDoesCaptureMouse = false;
	LastMouseDisplayChangeTime = 0.0;
	updateCameraVectors();
}

glm::mat4 Camera::GetViewMatrix()
{
	return glm::lookAt(Position.ToGLVector(), Position.ToGLVector() + Front.ToGLVector(), Up.ToGLVector());
}

void Camera::ProcessKeyboard(Camera_Movement direction, float deltaTime)
{
	float velocity = MovementSpeed * deltaTime;
	if (direction == FORWARD)
		Position += Front * velocity;
	if (direction == BACKWARD)
		Position -= Front * velocity;
	if (direction == LEFT)
		Position -= Right * velocity;
	if (direction == RIGHT)
		Position += Right * velocity;
	if (direction == UP)
		Position += Up * velocity;
	if (direction == DOWN)
		Position -= Up * velocity;
}

void Camera::ProcessMouseMovement(double xoffset, double yoffset, unsigned char constrainPitch /*= true*/)
{
	if (!bDoesCaptureMouse) return;
	xoffset *= MouseSensitivity;
	yoffset *= MouseSensitivity;

	Yaw += xoffset;
	Pitch += yoffset;

	// Make sure that when pitch is out of bounds, screen doesn't get flipped
	if (constrainPitch)
	{
		if (Pitch > 89.0f)
			Pitch = 89.0f;
		if (Pitch < -89.0f)
			Pitch = -89.0f;
	}

	// Update Front, Right and Up Vectors using the updated Euler angles
	updateCameraVectors();
}

void Camera::ProcessMouseScroll(float yoffset)
{
	if (yoffset != 0)
	{
		MovementSpeed += yoffset * MovementSpeed * .25f;
		if (MovementSpeed < 1) MovementSpeed = 1;
	}
}

void Camera::SwitchCaptureMouse()
{
	if (glfwGetTime() - LastMouseDisplayChangeTime > 0.5)
	{
		bDoesCaptureMouse = !bDoesCaptureMouse;
		LastMouseDisplayChangeTime = glfwGetTime();
	}
}

void Camera::updateCameraVectors()
{
	// Calculate the new Front vector
	glm::vec3 front;
	front.x = cos(glm::radians(-(float)Yaw)) * cos(glm::radians((float)Pitch));
	front.z = sin(glm::radians((float)Pitch));
	front.y = sin(glm::radians(-(float)Yaw)) * cos(glm::radians((float)Pitch));
	Front = glm::normalize(front);
	// Also re-calculate the Right and Up vector
	Right = glm::normalize(glm::cross(Front.ToGLVector(), WorldUp.ToGLVector()));  // Normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
	Up = glm::normalize(glm::cross(Right.ToGLVector(), Front.ToGLVector()));
}

EditorCamera::EditorCamera(EditorScene* inRenderScene, SVector3 position /*= SVector3(0.f, 0.f, 0.f)*/, SVector3 up /*= SVector3(0.f, 0.f, 1.f)*/, float yaw /*= YAW*/, float pitch /*= PITCH*/)
	: Camera(inRenderScene, position, up, yaw, pitch)
{
	inRenderScene->GetParentWorld()->GetInputManager()->BindOnMousePositionChanged(this, &EditorCamera::UpdateCameraDelta);
}

void EditorCamera::UpdateCameraDelta()
{
	ProcessMouseMovement(-((EditorScene*)renderScene)->GetParentWorld()->GetInputManager()->GetMousePositionDeltaX(), ((EditorScene*)renderScene)->GetParentWorld()->GetInputManager()->GetMousePositionDeltaY());
}
