#include "GameObject.h"


void GameObject::AddComponent(GameComponent* comp) {
	components.push_back(comp);
	comp->parent = this;
}

void GameObject::AddChild(GameObject* child) {
	children.push_back(child);
	child->parent = this;
}