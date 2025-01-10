#pragma once
#include "float3.h"
#include "float4.h"
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
	float3 rotation;
	unsigned int index;

	std::list<GameComponent*> components;
	std::list<GameObject*> children;

	void AddComponent(GameComponent* comp);
	void AddChild(GameObject* child);
	void Tick(void);

	float3 GetPosition();
	float3 GetScale();
	float3 GetRotation();

	float3 GetLocalRotation();
	void SetLocalRotation(float3 newRot);

	float4 GetOrientation();
	float3 GetForward();

	void SetPosition(float3 newPos);
	void LookAt(GameObject* obj);
};

