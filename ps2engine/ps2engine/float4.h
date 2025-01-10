#pragma once

struct float3;

struct float4
{
    float x, y, z, w;

    // Constructor
    float4(float x = 0, float y = 0, float z = 0, float w = 1);

    // Create quaternion from yaw, pitch, and roll
    void CreateFromYawPitchRoll(float yaw, float pitch, float roll);

    // Quaternion multiplication
    float4 operator*(const float4& q) const;

    // Rotate a vector using the quaternion
    float3 operator*(const float3& pos) const;
};
