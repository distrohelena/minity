#pragma once

class GameObject;
class GameComponent
{
public:
	GameObject* parent;
	bool enabled = true;

	virtual void OnEnabled(void);
};

