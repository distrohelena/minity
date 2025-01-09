#pragma once

class GameObject;
class GameComponent
{
public:
	GameObject* parent;
	bool enabled = true;

	virtual void Start(void);
	virtual void OnEnable(void);
	virtual void Tick(void);
};

