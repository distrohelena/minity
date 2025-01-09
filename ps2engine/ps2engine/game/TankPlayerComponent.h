#pragma once
#include "GameObject.h"
#include "GameComponent.h"
class TankPlayerComponent : public GameComponent
{
public:
    GameObject* body;

    void Tick(void) override;
};
