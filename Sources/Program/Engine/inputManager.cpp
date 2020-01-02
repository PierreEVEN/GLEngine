#include "inputManager.h"
#include <glfw3/glfw3.h>
#include <iostream>

InputManager::InputManager(GLFWwindow* inWindow)
	: window(inWindow)
{
	for (int i = 0; i < GLFW_KEY_LAST; ++i)
	{
		keys.push_back(new InputState(i));
	}
	for (int i = 0; i < GLFW_MOUSE_BUTTON_LAST; ++i)
	{
		keys.push_back(new InputState(Key(i, true)));
	}
	mousePositionCallback = new Event();
}

void InputManager::SetCaptureMouse(bool bSetCaptureMouse)
{
	bDoesCaptureMouse = bSetCaptureMouse;
	glfwSetInputMode(window, GLFW_CURSOR, bSetCaptureMouse ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
}

void InputManager::ProcessInputs()
{
	for (InputState* key : keys)
	{
		if (!key->GetKey().IsMouseButton())
		{
			if (glfwGetKey(window, key->GetKey().GetKeyCode()) == GLFW_PRESS)
			{
				key->Press();
			}
			else if (glfwGetKey(window, key->GetKey().GetKeyCode()) == GLFW_RELEASE)
			{
				key->Release();
			}
		}
		else
		{
			if (glfwGetMouseButton(window, key->GetKey().GetKeyCode()) == GLFW_PRESS)
			{
				key->Press();
			}
			else if (glfwGetMouseButton(window, key->GetKey().GetKeyCode()) == GLFW_RELEASE)
			{
				key->Release();
			}
		}
	}
	for (auto& keybind : keyBinds)
	{
		keybind->Update();
	}
}

void InputManager::AddInput(KeyBind* newKeyBind)
{
	keyBinds.push_back(newKeyBind);
}

void InputManager::SetMousePosition(double posX, double posY)
{
	mouseDeltaX = mouseLocationX - posX;
	mouseDeltaY = mouseLocationY - posY;
	mouseLocationX = posX;
	mouseLocationY = posY;
	mousePositionCallback->execute();
}

void KeyBind::Update()
{
	if (actionType == ActionType_JustPress && WasJustpressed())
	{
		callback->execute();
	}
	if (actionType == ActionType_JustRelease && WasJustReleased())
	{
		callback->execute();
	}
	if (actionType == ActionType_Press && IsPressed())
	{
		callback->execute();
	}
	if (actionType == ActionType_Release && IsReleased())
	{
		callback->execute();
	}
}

bool KeyBind::WasJustpressed()
{
	bool bFoundJustpressedKey = false;
	for (const auto& key : keyMap)
	{
		if (!key->bIsPressed) return false;
		if (key->bWasJustPressed)
		{
			if (key->bWasJustPressed)
			{
				bFoundJustpressedKey = true;
			}
		}
	}
	return bFoundJustpressedKey;
}

bool KeyBind::WasJustReleased()
{
	for (const auto& key : keyMap)
	{
		if (key->bWasJustReleased)
		{
			return true;
		}
	}
	return false;
}

bool KeyBind::IsPressed()
{
	for (const auto& key : keyMap)
	{
		if (!key->bIsPressed)
		{
			return false;
		}
	}
	return true;
}

bool KeyBind::IsReleased()
{
	return !IsPressed();
}

KeyBind::KeyBind(InputManager* inputManager, std::vector<Key> inKeys, ActionType inActionType)
	: actionType(inActionType)
{
	callback = new Event();
	keyMap = {};
	assert(inputManager);
	for (const auto& key : inKeys)
	{
		for (const auto& keyCode : inputManager->GetKeys())
		{
			if (keyCode->GetKey().GetKeyCode() == key.GetKeyCode() && key.IsMouseButton() == keyCode->GetKey().IsMouseButton())
			{
				keyMap.push_back(keyCode);
				break;
			}
		}
	}
	inputManager->AddInput(this);
}

void InputState::Press()
{
	if (bWasJustReleased) bWasJustReleased = false;
	if (!bIsPressed && !bWasJustPressed)
	{
		bWasJustPressed = true;
	}
	else if (bWasJustPressed)
	{
		bWasJustPressed = false;
	}
	if (!bIsPressed) bIsPressed = true;
}

void InputState::Release()
{
	if (bWasJustPressed) bWasJustPressed = false;
	if (bIsPressed && !bWasJustReleased)
	{
		bWasJustReleased = true;
	}
	else if (bWasJustReleased)
	{
		bWasJustReleased = false;
	}
	if (bIsPressed) bIsPressed = false;
}

int Key::GetScanCode() const
{
	return glfwGetKeyScancode(keyCode);
}

const char* Key::GetKeyName() const
{
	if (glfwGetKeyName(keyCode, GetScanCode()))
	{
		return glfwGetKeyName(keyCode, GetScanCode());
	}
	return "";
}