#include "CameraComponent.h"
#include "Core.h"

void CameraComponent::OnEnable(void) {
	// enabled, add ourselves to render list
	Core::instance->GetRenderManager()->RegisterCamera(this);
}