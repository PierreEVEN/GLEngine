
#include <World/world.h>
#include <Engine/inputManager.h>
#include <Camera/camera.h>
#include <Scene/scene.h>

Camera::Camera(Scene* inRenderScene, SVector3 position, SRotator startRotation)
	: SceneComponent(inRenderScene)
{
	SetLocation(position);
	SetRotation(startRotation);
	Yaw = 0;
	Pitch = 0;
	bDoesCaptureMouse = false;
	LastMouseDisplayChangeTime = 0.0;
	updateCameraVectors();
}

glm::mat4 Camera::GetViewMatrix()
{
	return glm::lookAt(GetLocation().ToGLVector(), GetLocation().ToGLVector() + GetRotation().GetForwardVector().ToGLVector(), GetRotation().GetUpVector().ToGLVector());
}

void Camera::ProcessKeyboard(Camera_Movement direction, float deltaTime)
{
	float velocity = MovementSpeed * deltaTime;
	if (direction == FORWARD)
		SetLocation(GetLocation() + GetRotation().GetForwardVector() * velocity);
	if (direction == BACKWARD)
		SetLocation(GetLocation() - GetRotation().GetForwardVector() * velocity);
	if (direction == LEFT)
		SetLocation(GetLocation() + GetRotation().GetRightVector() * velocity);
	if (direction == RIGHT)
		SetLocation(GetLocation() - GetRotation().GetRightVector() * velocity);
	if (direction == UP)
		SetLocation(GetLocation() + GetRotation().GetUpVector() * velocity);
	if (direction == DOWN)
		SetLocation(GetLocation() - GetRotation().GetUpVector() * velocity);
}

void Camera::ProcessMouseMovement(double xoffset, double yoffset, unsigned char constrainPitch /*= true*/)
{
	if (!bDoesCaptureMouse) return;
	xoffset *= MouseSensitivity;
	yoffset *= MouseSensitivity;

	Pitch -= yoffset;
	Yaw -= xoffset;

	// Make sure that when pitch is out of bounds, screen doesn't get flipped
	if (constrainPitch)
	{
		if (Pitch > 89.0f)
			Pitch = 89.0f;
		if (Pitch < -89.0f)
			Pitch = -89.0f;
	}

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
	SetRotation(SRotator(0, Pitch, Yaw));
}
