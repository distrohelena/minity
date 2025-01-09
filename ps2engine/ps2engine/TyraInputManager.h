#pragma once

#ifdef PS2
#include "InputManager.h"
#include <tyra>

using namespace Tyra;

class TyraInputManager : public InputManager
{
public:
	void Init(void) override;
	void Tick(void) override;
	float2 GetLeftThumbstick() override;
	float2 GetRightThumbstick() override;

private:
	Pad* pad;
};

#endif