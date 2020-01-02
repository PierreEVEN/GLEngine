#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>
#include "../MathLibrary/vector3.h"

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
class Camera
{
public:
	// Camera Attributes
	SVector3 Position;
	SVector3 Front;
	SVector3 Up;
	SVector3 Right;
	SVector3 WorldUp;
	// Euler Angles
	double Yaw;
	double Pitch;
	// Camera options
	float MovementSpeed;
	float MouseSensitivity;
	float Zoom;
	float Near = 0.1f;
	float Far = 5000.f;
	Scene* renderScene;

	// Constructor with vectors
	Camera(	Scene* inRenderScene,
			SVector3 position = SVector3(0.f, 0.f, 0.f),
			SVector3 up = SVector3(0.f, 0.f, 1.f),
			float yaw = YAW,
			float pitch = PITCH);

	// Constructor with scalar values
	Camera( Scene* inRenderScene,
			float posX,
			float posY,
			float posZ,
			float upX,
			float upY,
			float upZ,
			float yaw,
			float pitch);

	// Returns the view matrix calculated using Euler Angles and the LookAt Matrix
	glm::mat4 GetViewMatrix();

	// Processes input received from any keyboard-like input system. Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
	void ProcessKeyboard(Camera_Movement direction, float deltaTime);

	// Processes input received from a mouse input system. Expects the offset value in both the x and y direction.
	void ProcessMouseMovement(double xoffset, double yoffset, unsigned char constrainPitch = true);

	// Processes input received from a mouse scroll-wheel event. Only requires input on the vertical wheel-axis
	void ProcessMouseScroll(float yoffset);

	void SetCameraLocation(SVector3 inPosition) { Position = inPosition; }

	SVector3 GetCameraLocation() const { return Position; }
	SVector3 GetCameraForwardVector() const { return Front; }
	SVector3 GetCameraRightVector() const { return Right; }
	SVector3 GetCameraUpVector() const { return Up; }
	
	float GetCameraSpeed() const { return MovementSpeed; }
	void SetCameraSpeed(float newCameraSpeed) { MovementSpeed = newCameraSpeed; }

	bool bDoesCaptureMouse;
	double LastMouseDisplayChangeTime;

	void SwitchCaptureMouse();

	// Calculates the front vector from the Camera's (updated) Euler Angles
	void updateCameraVectors();
private:


};

class EditorScene;

class EditorCamera : public Camera
{
public:
	EditorCamera(EditorScene* inRenderScene, SVector3 position = SVector3(0.f, 0.f, 0.f), SVector3 up = SVector3(0.f, 0.f, 1.f), float yaw = YAW, float pitch = PITCH);

	void UpdateCameraDelta();
};