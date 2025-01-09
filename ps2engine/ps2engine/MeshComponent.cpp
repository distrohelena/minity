#include "MeshComponent.h"
#include "Core.h"

void MeshComponent::OnEnable(void) {
	// enabled, add ourselves to render list
	Core::instance->GetRenderManager()->RegisterMesh(this);
}