#pragma once

struct float4;

struct float3
{
    float x, y, z;

    float3(float x = 0, float y = 0, float z = 0);

    float3 operator+(const float3& rhs) const;
    float3 operator-(const float3& rhs) const;
    float3 operator*(float scalar) const;
    float3 operator/(float scalar) const;

    void Transform(const float4& rotation);
    void Add(const float3& another);

    static float degreesToRadians(float degrees);
    void ToRadians();
    static float3 ToRadians(const float3& euler);
    static float4 EulerToQuaternion(const float3& euler);
    static float3 Lerp(const float3& value1, const float3& value2, float amount);
};
