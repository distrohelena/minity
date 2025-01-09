#include "CameraKeyboard.h"
#include "Core.h"

void CameraKeyboard::Tick(void) {
    float deltaTime = 1 / 60.0f;
    const float moveSpeed = 5.0f * deltaTime;
    const float rotateSpeed = 0.5f * deltaTime;

    float delta = 1 / 60.0f;
    float move_speed = delta * 5;
    float2 leftThumb = Core::instance->GetInputManager()->GetLeftThumbstick();
    parent->position.x += leftThumb.x * move_speed;
    parent->position.z += leftThumb.y * move_speed;

    float2 rightThumb = Core::instance->GetInputManager()->GetRightThumbstick();
    rotation.x -= rightThumb.y * rotateSpeed;
    rotation.y += rightThumb.x * rotateSpeed;
    parent->position.y += rightThumb.y * move_speed;
    
    //parent->orientation.CreateFromYawPitchRoll(rotation.y, rotation.x, rotation.z);
    
}