#pragma once
#include "GameComponent.h"
#include "float4.h"

using namespace std;

class LightComponent : public GameComponent
{
public:
	float4 color;
	float intensity;
	char type;

	void OnEnable(void) override;

};

