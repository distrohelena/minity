#include "float4.h"
#include "float3.h"
#include <cmath>

// Constructor
float4::float4(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) {}

// Create quaternion from yaw, pitch, and roll
void float4::CreateFromYawPitchRoll(float yaw, float pitch, float roll)
{
    float halfRoll = roll * 0.5f;
    float halfPitch = pitch * 0.5f;
    float halfYaw = yaw * 0.5f;

    float sinRoll = std::sin(halfRoll);
    float cosRoll = std::cos(halfRoll);
    float sinPitch = std::sin(halfPitch);
    float cosPitch = std::cos(halfPitch);
    float sinYaw = std::sin(halfYaw);
    float cosYaw = std::cos(halfYaw);

    x = (cosYaw * sinPitch * cosRoll) + (sinYaw * cosPitch * sinRoll);
    y = (sinYaw * cosPitch * cosRoll) - (cosYaw * sinPitch * sinRoll);
    z = (cosYaw * cosPitch * sinRoll) - (sinYaw * sinPitch * cosRoll);
    w = (cosYaw * cosPitch * cosRoll) + (sinYaw * sinPitch * sinRoll);
}

// Quaternion multiplication
float4 float4::operator*(const float4& q) const
{
    return float4(
        w * q.x + x * q.w + y * q.z - z * q.y,
        w * q.y - x * q.z + y * q.w + z * q.x,
        w * q.z + x * q.y - y * q.x + z * q.w,
        w * q.w - x * q.x - y * q.y - z * q.z
    );
}

// Rotate a vector using the quaternion
float3 float4::operator*(const float3& pos) const
{
    float qx = x, qy = y, qz = z, qw = w;

    float xx = 2.0f * (qy * pos.z - qz * pos.y);
    float yy = 2.0f * (qz * pos.x - qx * pos.z);
    float zz = 2.0f * (qx * pos.y - qy * pos.x);

    return float3(
        pos.x + xx * qw + (qy * zz - qz * yy),
        pos.y + yy * qw + (qz * xx - qx * zz),
        pos.z + zz * qw + (qx * yy - qy * xx)
    );
}
