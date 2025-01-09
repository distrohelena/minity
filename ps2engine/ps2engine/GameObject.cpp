#include "GameObject.h"

void GameObject::AddComponent(GameComponent *comp)
{
	components.push_back(comp);
	comp->parent = this;

	comp->OnEnable();
}

void GameObject::AddChild(GameObject *child)
{
	children.push_back(child);
	child->parent = this;
}

void GameObject::Tick()
{
	for (GameComponent *comp : components)
	{
		if (!comp->enabled)
		{
			continue;
		}

		comp->Tick();
	}

	for (GameObject *child : children)
	{
		child->Tick();
	}
}

float3 GameObject::GetPosition()
{
	float3 pos = this->position;
	if (parent)
	{
		float3 parentPos = parent->GetPosition();
		pos.x += parentPos.x;
		pos.y += parentPos.y;
		pos.z += parentPos.z;
	}
	return pos;
}

float3 GameObject::GetScale()
{
	float3 scale = this->scale;
	if (parent)
	{
		float3 parentScale = parent->GetScale();
		scale.x *= parentScale.x;
		scale.y *= parentScale.y;
		scale.z *= parentScale.z;
	}
	return scale;
}

float3 GameObject::GetRotation()
{
	float3 rot = this->rotation;
	if (parent)
	{
		float3 parentRot = parent->GetRotation();
		rot.x += parentRot.x;
		rot.y += parentRot.y;
		rot.z += parentRot.z;
	}

	return rot;
}

void GameObject::SetPosition(float3 newPos)
{
	if (parent)
	{
		float3 parentPos = parent->GetPosition();
		position = parentPos - newPos;
	}
	else
	{
		position = newPos;
	}
}

float4 GameObject::GetOrientation()
{
	float3 rot = this->GetRotation();
	return float3::EulerToQuaternion(rot);
}

void GameObject::LookAt(GameObject *obj)
{
	rotation.x = 0;
	rotation.y = 0;
	rotation.z = 0;

	float3 target = obj->GetPosition();
	float3 direction = target - GetPosition();
}