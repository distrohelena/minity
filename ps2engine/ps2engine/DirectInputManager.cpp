#if DIRECTX

#include "DirectInputManager.h"
#include "Core.h"


void DirectInputManager::Init()
{
	DirectInput8Create(GetModuleHandle(NULL), DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&directInput, NULL);
	directInput->CreateDevice(GUID_SysKeyboard, &keyboardDevice, NULL);
	keyboardDevice->SetDataFormat(&c_dfDIKeyboard);
	//DISCL_EXCLUSIVE
	keyboardDevice->SetCooperativeLevel(Core::instance->GetHWND(), DISCL_FOREGROUND | DISCL_EXCLUSIVE);
	keyboardDevice->Acquire();
}

void DirectInputManager::Tick()
{
	keyboardDevice->GetDeviceState(sizeof(keyboardState), (LPVOID)&keyboardState);
}

float2 DirectInputManager::GetLeftThumbstick()
{
	float2 vec = { 0, 0 };
	if (keyboardState[DIK_W] & 0x80)
	{
		vec.y += 1;
	}
	if (keyboardState[DIK_S] & 0x80)
	{
		vec.y -= 1;
	}
	if (keyboardState[DIK_A] & 0x80)
	{
		vec.x -= 1;
	}
	if (keyboardState[DIK_D] & 0x80)
	{
		vec.x += 1;
	}

	return vec;
}

float2 DirectInputManager::GetRightThumbstick()
{
	float2 vec = { 0, 0 };
	if (keyboardState[DIK_Q] & 0x80)
	{
		vec.y += 1;
	}
	if (keyboardState[DIK_E] & 0x80)
	{
		vec.y -= 1;
	}
	if (keyboardState[DIK_K] & 0x80)
	{
		vec.x -= 1;
	}
	if (keyboardState[DIK_L] & 0x80)
	{
		vec.x += 1;
	}

	return vec;
}

#endif