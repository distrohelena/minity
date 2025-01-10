#pragma once
#include "CameraKeyboard.h"
#include "GameComponent.h"
#include "float3.h"

class CameraKeyboard : public GameComponent
{
public:
	void Tick(void) override;

private:
	float3 rotation;

};