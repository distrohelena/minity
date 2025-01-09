#pragma once

#if DIRECTX
#include <dinput.h>
#include <DirectXMath.h>
#include <windows.h>
#include "InputManager.h"
using namespace DirectX;

class DirectInputManager : public InputManager
{
public:
	void Init(void) override;
	void Tick(void) override;
	float2 GetLeftThumbstick() override;
	float2 GetRightThumbstick() override;

private:
	// DirectInput variables
	IDirectInput8* directInput = nullptr;
	IDirectInputDevice8* keyboardDevice = nullptr;
	const DWORD numKeys = 256;
	BYTE keyboardState[256];
};

#endif