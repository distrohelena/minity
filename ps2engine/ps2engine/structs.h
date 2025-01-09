#pragma once

#include <cmath>
#include "MathUtil.h"

struct float2
{
    float x;
    float y;
};

struct float4
{
    float x;
    float y;
    float z;
    float w;

    void CreateFromYawPitchRoll(float yaw, float pitch, float roll)
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

        this->x = (cosYaw * sinPitch * cosRoll) + (sinYaw * cosPitch * sinRoll);
        this->y = (sinYaw * cosPitch * cosRoll) - (cosYaw * sinPitch * sinRoll);
        this->z = (cosYaw * cosPitch * sinRoll) - (sinYaw * sinPitch * cosRoll);
        this->w = (cosYaw * cosPitch * cosRoll) + (sinYaw * sinPitch * sinRoll);
    }

    float4 operator*(const float4 &q) const
    {
        return {w * q.w - x * q.x - y * q.y - z * q.z,
                w * q.x + x * q.w + y * q.z - z * q.y,
                w * q.y - x * q.z + y * q.w + z * q.x,
                w * q.z + x * q.y - y * q.x + z * q.w};
    }

    float3 operator*(const float3 &pos) const
    {
        float xx = 2 * (y * pos.z - z * pos.y);
        float yy = 2 * (z * pos.x - x * pos.z);
        float zz = 2 * (x * pos.y - y * pos.x);

        return float3(pos.x + xx * w + (y * zz - z * yy),
                    pos.y + yy * w + (z * xx - x * zz),
                    pos.z + zz * w + (x * yy - y * xx));
    }
};

struct float3
{
    float x;
    float y;
    float z;

    // Constructor
    float3(float x = 0, float y = 0, float z = 0) : x(x), y(y), z(z) {}

    // Addition
    float3 operator+(const float3 &rhs) const
    {
        return float3(x + rhs.x, y + rhs.y, z + rhs.z);
    }

    // Subtraction
    float3 operator-(const float3 &rhs) const
    {
        return float3(x - rhs.x, y - rhs.y, z - rhs.z);
    }

    // Multiplication by a scalar
    float3 operator*(float scalar) const
    {
        return float3(x * scalar, y * scalar, z * scalar);
    }

    // Division by a scalar
    float3 operator/(float scalar) const
    {
        if (scalar != 0)
        {
            return float3(x / scalar, y / scalar, z / scalar);
        }
        else
        {
            // Handle divide-by-zero according to your needs
            // This example just returns a zero vector
            return float3();
        }
    }

    void Transform(float4 rotation)
    {
        float x = 2 * (rotation.y * this->z - rotation.z * this->y);
        float y = 2 * (rotation.z * this->x - rotation.x * this->z);
        float z = 2 * (rotation.x * this->y - rotation.y * this->x);

        this->x = this->x + x * rotation.w + (rotation.y * z - rotation.z * y);
        this->y = this->y + y * rotation.w + (rotation.z * x - rotation.x * z);
        this->z = this->z + z * rotation.w + (rotation.x * y - rotation.y * x);
    }

    void Add(float3 another)
    {
        this->x = this->x + another.x;
        this->y = this->y + another.y;
        this->z = this->z + another.z;
    }

    static float degreesToRadians(float degrees)
    {
        const float PI = (float)3.14159265358979323846;
        return degrees * (PI / 180.0f);
    }

    void ToRadians()
    {
        this->x = degreesToRadians(this->x);
        this->y = degreesToRadians(this->y);
        this->z = degreesToRadians(this->z);
    }

    static float3 ToRadians(const float3 &euler)
    {
        float3 rad;
        rad.x = degreesToRadians(euler.x);
        rad.y = degreesToRadians(euler.y);
        rad.z = degreesToRadians(euler.z);

        return rad;
    }

    static float4 EulerToQuaternion(const float3 &euler)
    {
        float eulerX = degreesToRadians(euler.x);
        float eulerY = degreesToRadians(euler.y);
        float eulerZ = degreesToRadians(euler.z);

        float cy = (float)cos(eulerZ * 0.5);
        float sy = (float)sin(eulerZ * 0.5);
        float cp = (float)cos(eulerY * 0.5);
        float sp = (float)sin(eulerY * 0.5);
        float cr = (float)cos(eulerX * 0.5);
        float sr = (float)sin(eulerX * 0.5);

        float4 q;
        q.w = cr * cp * cy + sr * sp * sy;
        q.x = sr * cp * cy - cr * sp * sy;
        q.y = cr * sp * cy + sr * cp * sy;
        q.z = cr * cp * sy - sr * sp * cy;

        return q;
    }

    static float3 Lerp(float3 value1, float3 value2, float amount)
    {
        return float3(
            MathUtil::Lerp(value1.x, value2.x, amount),
            MathUtil::Lerp(value1.y, value2.y, amount),
            MathUtil::Lerp(value1.z, value2.z, amount));
    }
};

struct VERTEX
{
    float4 pos;
    float4 uv;
    float4 normal;
};