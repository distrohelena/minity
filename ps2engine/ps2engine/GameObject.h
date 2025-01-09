#pragma once
#include "structs.h"
#include <iostream>
#include <list>
#include "GameComponent.h"

using namespace std;

class GameObject
{
public:
	GameObject* parent;
	float3 position;
	float3 scale;
	float4 orientation;

	std::list<GameComponent*> components;
	std::list<GameObject*> children;

	void AddComponent(GameComponent* comp);
	void AddChild(GameObject* child);
};

