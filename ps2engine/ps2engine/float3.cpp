#include "float3.h"
#include "float4.h"
#include "MathUtil.h"
#include <cmath>

float3::float3(float x, float y, float z) : x(x), y(y), z(z) {}

float3 float3::operator+(const float3& rhs) const
{
    return float3(x + rhs.x, y + rhs.y, z + rhs.z);
}

float3 float3::operator-(const float3& rhs) const
{
    return float3(x - rhs.x, y - rhs.y, z - rhs.z);
}

float3 float3::operator*(float scalar) const
{
    return float3(x * scalar, y * scalar, z * scalar);
}

float3 float3::operator/(float scalar) const
{
    return scalar != 0 ? float3(x / scalar, y / scalar, z / scalar) : float3();
}

void float3::Transform(const float4& rotation)
{
    float rx = 2 * (rotation.y * z - rotation.z * y);
    float ry = 2 * (rotation.z * x - rotation.x * z);
    float rz = 2 * (rotation.x * y - rotation.y * x);

    x += rx * rotation.w + (rotation.y * rz - rotation.z * ry);
    y += ry * rotation.w + (rotation.z * rx - rotation.x * rz);
    z += rz * rotation.w + (rotation.x * ry - rotation.y * rx);
}

void float3::Add(const float3& another)
{
    x += another.x;
    y += another.y;
    z += another.z;
}

float float3::degreesToRadians(float degrees)
{
    constexpr float PI = 3.14159265358979323846f;
    return degrees * (PI / 180.0f);
}

void float3::ToRadians()
{
    x = degreesToRadians(x);
    y = degreesToRadians(y);
    z = degreesToRadians(z);
}

float3 float3::ToRadians(const float3& euler)
{
    return float3(
        degreesToRadians(euler.x),
        degreesToRadians(euler.y),
        degreesToRadians(euler.z)
    );
}

float4 float3::EulerToQuaternion(const float3& euler)
{
    float eulerX = degreesToRadians(euler.x);
    float eulerY = degreesToRadians(euler.y);
    float eulerZ = degreesToRadians(euler.z);

    float cy = std::cos(eulerZ * 0.5f);
    float sy = std::sin(eulerZ * 0.5f);
    float cp = std::cos(eulerY * 0.5f);
    float sp = std::sin(eulerY * 0.5f);
    float cr = std::cos(eulerX * 0.5f);
    float sr = std::sin(eulerX * 0.5f);

    return float4(
        sr * cp * cy - cr * sp * sy,
        cr * sp * cy + sr * cp * sy,
        cr * cp * sy - sr * sp * cy,
        cr * cp * cy + sr * sp * sy
    );
}

float3 float3::Lerp(const float3& value1, const float3& value2, float amount)
{
    return float3(
        MathUtil::Lerp(value1.x, value2.x, amount),
        MathUtil::Lerp(value1.y, value2.y, amount),
        MathUtil::Lerp(value1.z, value2.z, amount)
    );
}
