#pragma once
#include "TankPlayerComponent.h"
#include "GameObject.h"
#include "structs.h"
#include "GameComponent.h"
class TankCameraComponent : public GameComponent
{
public:
    TankPlayerComponent* player;
    GameObject* lookAt;
    float positionSmoothSpeed = 0.125f;
    float rotationSmoothSpeed = 0.125f;
    float3 offset;

};
