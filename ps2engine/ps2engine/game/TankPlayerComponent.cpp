#include "TankPlayerComponent.h"
#include "Time.h"

void TankPlayerComponent::Tick(void) {
float dt = Time::DeltaTime;
float speed = 15;
float rotSpeed = 100;
float ver = Input::GetAxisRaw("Vertical");
parent->SetPosition(parent->forward * ver * speed * dt);
float hor = Input::GetAxisRaw("Horizontal");
float3 euler = parent->localEulerAngles;
euler->y = hor * rotSpeed * dt;
parent->localEulerAngles = euler;


}

