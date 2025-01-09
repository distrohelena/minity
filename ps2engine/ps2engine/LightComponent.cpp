#include "LightComponent.h"
#include "Core.h"

void LightComponent::OnEnable(void) {
	// enabled, add ourselves to render list
	Core::instance->GetRenderManager()->RegisterLight(this);
}