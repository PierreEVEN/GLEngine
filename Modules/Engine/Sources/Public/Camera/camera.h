#pragma once

#include <EnginePCH.h>
#include <Scene/sceneComponent.h>

class World;
class Scene;

// Defines several possible options for camera movement. Used as abstraction to stay away from window-system specific input methods
enum Camera_Movement {
	FORWARD,
	BACKWARD,
	LEFT,
	RIGHT,
	UP,
	DOWN
};


// Default camera values
const float YAW = 0.f;
const float PITCH = 0.0f;
const float SENSITIVITY = 0.1f;
const float SPEED = 10.f;
const float ZOOM = 45.0f;

// An abstract camera class that processes input and calculates the corresponding Euler Angles, Vectors and Matrices for use in OpenGL
class Camera : public SceneComponent
{
public:
	// Euler Angles
	double Yaw = 0;
	double Pitch = 0;
	// Camera options
	float MovementSpeed = 10.f;
	float MouseSensitivity = 0.1f;
	float Zoom = 45.f;
	float Near = 0.1f;
	float Far = 1000000.f;

	// Constructor with vectors
	Camera(	Scene* inRenderScene, SVector3 position = SVector3(0.f, 0.f, 0.f), SRotator startRotation = SRotator(0.f, 0.f, 0.f));

	// Returns the view matrix calculated using Euler Angles and the LookAt Matrix
	glm::mat4 GetViewMatrix();

	// Processes input received from any keyboard-like input system. Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
	void ProcessKeyboard(Camera_Movement direction, float deltaTime);

	// Processes input received from a mouse input system. Expects the offset value in both the x and y direction.
	void ProcessMouseMovement(double xoffset, double yoffset, unsigned char constrainPitch = true);

	// Processes input received from a mouse scroll-wheel event. Only requires input on the vertical wheel-axis
	void ProcessMouseScroll(float yoffset);
	float GetCameraSpeed() const { return MovementSpeed; }
	void SetCameraSpeed(float newCameraSpeed) { MovementSpeed = newCameraSpeed; }

	bool bDoesCaptureMouse;
	double LastMouseDisplayChangeTime;

	void SwitchCaptureMouse();

	// Calculates the front vector from the Camera's (updated) Euler Angles
	void updateCameraVectors();
};
