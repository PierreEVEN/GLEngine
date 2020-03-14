#pragma once

#include <CorePCH.h>

struct GLFWwindow;


enum ActionType
{
	ActionType_Press = 0,
	ActionType_Release = 1,
	ActionType_JustRelease = 2,
	ActionType_JustPress = 3,
	ActionType_DoubleClic = 4,
	ActionType_Action_MAX = 4
};

struct Key
{
private:
	bool bMouseButton = false;
	int keyCode = -1;
public:
	Key(int inKeyCode, bool IsbMouseButton = false)
		: keyCode(inKeyCode), bMouseButton(IsbMouseButton) {}

	int GetKeyCode() const { return keyCode; }
	bool IsMouseButton() const { return bMouseButton; }
	int GetScanCode() const;
	const char* GetKeyName() const;
};

struct InputState
{
	InputState(Key inKey)
		: key(inKey) {}

	Key GetKey() const { return key; }

	bool bWasJustPressed = false;
	bool bWasJustReleased = false;
	bool bIsPressed = false;
	void Press();
	void Release();

private:
	Key key;
};

class InputManager;
struct KeyBind
{
private:
	std::vector<InputState*> keyMap;
	ActionType actionType;

	Event* callback;
public:

	void Update();

	bool WasJustpressed();
	bool WasJustReleased();
	bool IsPressed();
	bool IsReleased();

	template <typename Class>
	void Bind(Class* obj, void (Class::* func)(void))
	{
		callback->addListener(obj, func);
	}

	template <typename Class>
	KeyBind(InputManager* inputManager, std::vector<Key> inKeys, ActionType inActionType, Class* obj, void (Class::* func)(void))
		: KeyBind(inputManager, inKeys, inActionType)
	{
		Bind(obj, func);
	}

	KeyBind(InputManager* inputManager, std::vector<Key> inKeys, ActionType inActionType);
};

class InputManager
{
	GLFWwindow* window;

	bool bDoesCaptureMouse;
	double mouseLocationX;
	double mouseLocationY;
	double mouseDeltaX;
	double mouseDeltaY;
	std::vector<InputState*> keys = {};
	std::vector<KeyBind*> keyBinds;

	Event* mousePositionCallback;

public:
	InputManager(GLFWwindow* inWindow);

	__forceinline std::vector<InputState*> GetKeys() const { return keys; }

	void SetCaptureMouse(bool bSetCaptureMouse);
	bool GetCaptureMouse() const { return bDoesCaptureMouse; }

	double GetMouseLocationX() const { return mouseLocationX; }
	double GetMouseLocationY() const { return mouseLocationY; }

	void GetMousePositionDelta(double& dx, double& dy) { dx = mouseDeltaX; dy = mouseDeltaY; }
	double GetMousePositionDeltaX() const { return mouseDeltaX; }
	double GetMousePositionDeltaY() const { return mouseDeltaY; }

	void ProcessInputs();
	void AddInput(KeyBind* newKeyBind);

	void SetMousePosition(double posX, double posY);

	template <typename Class>
	void BindOnMousePositionChanged(Class* obj, void (Class::* func)(void))
	{
		mousePositionCallback->addListener(obj, func);
	}
};