#include "MeshComponent.h"
#include <windows.h>
#include "Core.h"

void MeshComponent::OnEnabled(void) {
	// enabled, add ourselves to render list
	Core::instance->GetRenderManager()->RegisterMesh(this);
}