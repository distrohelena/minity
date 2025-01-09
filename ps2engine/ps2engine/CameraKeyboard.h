#pragma once
#include "CameraKeyboard.h"
#include "GameComponent.h"
#include "structs.h"

class CameraKeyboard : public GameComponent
{
public:
	void Tick(void) override;

private:
	float3 rotation;

};