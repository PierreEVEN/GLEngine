
#include "camera.h"
#include <glfw3/glfw3.h>
#include "../World/world.h"

Camera::Camera(World* inParentWorld, SVector3 position, SVector3 up, float yaw, float pitch)
	: ParentWorld(inParentWorld), Front(SVector3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
{
	Position = position;
	WorldUp = up;
	Yaw = yaw;
	Pitch = pitch;
	bDoesCaptureMouse = false;
	LastMouseDisplayChangeTime = 0.0;
	updateCameraVectors();
}

Camera::Camera(World* inParentWorld, float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch)
	: ParentWorld(inParentWorld), Front(SVector3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
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

void Camera::ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch /*= true*/)
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

}

void Camera::SwitchCaptureMouse()
{
	if (glfwGetTime() - LastMouseDisplayChangeTime > 0.5)
	{
		bDoesCaptureMouse = !bDoesCaptureMouse;
		if (bDoesCaptureMouse)
		{
			glfwSetInputMode(ParentWorld->GetWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		}
		else
		{
			glfwSetInputMode(ParentWorld->GetWindow(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		}
		LastMouseDisplayChangeTime = glfwGetTime();
	}
}

void Camera::updateCameraVectors()
{
	// Calculate the new Front vector
	glm::vec3 front;
	front.x = cos(glm::radians(-Yaw)) * cos(glm::radians(Pitch));
	front.z = sin(glm::radians(Pitch));
	front.y = sin(glm::radians(-Yaw)) * cos(glm::radians(Pitch));
	Front = glm::normalize(front);
	// Also re-calculate the Right and Up vector
	Right = glm::normalize(glm::cross(Front.ToGLVector(), WorldUp.ToGLVector()));  // Normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
	Up = glm::normalize(glm::cross(Right.ToGLVector(), Front.ToGLVector()));
}