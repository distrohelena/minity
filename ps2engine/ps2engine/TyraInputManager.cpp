#ifdef PS2

#include "TyraInputManager.h"
#include "Core.h"


void TyraInputManager::Init()
{
	Engine* engine = Core::instance->GetEngine();
	pad = &engine->pad;
}

void TyraInputManager::Tick()
{
}

float2 TyraInputManager::GetLeftThumbstick()
{
	float2 vec = { 0, 0 };
	const auto& leftJoy = pad->getLeftJoyPad();

	if (leftJoy.v <= 100) {
		vec.y += 1;
	}
	else if (leftJoy.v >= 200) {
		vec.y -= 1;
	}

	if (leftJoy.h <= 100) {
		vec.x += 1;
	}
	else if (leftJoy.h >= 200) {
		vec.x -= 1;
	}

	return vec;
}

float2 TyraInputManager::GetRightThumbstick()
{
	float2 vec = { 0, 0 };
	const auto& rightJoy = pad->getRightJoyPad();

	if (rightJoy.v <= 100) {
		vec.y += 1;
	}
	else if (rightJoy.v >= 200) {
		vec.y -= 1;
	}

	if (rightJoy.h <= 100) {
		vec.x += 1;
	}
	else if (rightJoy.h >= 200) {
		vec.x -= 1;
	}

	return vec;
}

#endif