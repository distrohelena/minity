#pragma once

#include "float2.h"

class InputManager
{
public:
	virtual void Init(void);
	virtual void Tick(void);

	virtual float2 GetLeftThumbstick();
	virtual float2 GetRightThumbstick();

};

