#include "Vector3d.h"

Vector3d::Vector3d(const Vector3d& vec)
{
    x = vec.x;
    y = vec.y;
    z = vec.z;
}

Vector3d Vector3d ::operator+(const Vector3d& vec)
{
    return Vector3d(x + vec.x, y + vec.y, z + vec.z);
}

Vector3d& Vector3d ::operator+=(const Vector3d& vec)
{
    x += vec.x;
    y += vec.y;
    z += vec.z;
    return *this;
}

Vector3d Vector3d ::operator-(const Vector3d& vec)
{
    return Vector3d(x - vec.x, y - vec.y, z - vec.z);
}

Vector3d& Vector3d::operator-=(const Vector3d& vec)
{
    x -= vec.x;
    y -= vec.y;
    z -= vec.z;
    return *this;
}

Vector3d Vector3d ::operator*(float value)
{
    return Vector3d(x * value, y * value, z * value);
}

Vector3d& Vector3d::operator*=(float value)
{
    x *= value;
    y *= value;
    z *= value;
    return *this;
}

Vector3d Vector3d ::operator/(float value)
{
    if (value < 0.000001f) return *this;
    return Vector3d(x / value, y / value, z / value);
}

Vector3d& Vector3d ::operator/=(float value)
{
    if (value < 0.000001f) return *this;
    x /= value;
    y /= value;
    z /= value;
    return *this;
}

Vector3d& Vector3d::operator=(const Vector3d& vec)
{
    x = vec.x;
    y = vec.y;
    z = vec.z;
    return *this;
}

float Vector3d::dot(const Vector3d& vec)
{
    return x * vec.x + vec.y * y + vec.z * z;
}

Vector3d Vector3d::cross(const Vector3d& vec)
{
    return Vector3d(y * vec.z - z * vec.y, z * vec.x - x * vec.z, x * vec.y - y * vec.x);
}

float Vector3d::length()
{
    return sqrt(square());
}

float Vector3d::square()
{
    return x * x + y * y + z * z;
}

Vector3d Vector3d::normalize()
{
    if (isZero()) return *this;
    *this /= length();
    return *this;
}

float Vector3d::distance(const Vector3d& vec)
{
    Vector3d dist = *this - vec;
    return dist.length();
}

bool Vector3d::isZero() { return length() < 0.000001f; }

Vector3d Vector3d::rotY(float degree)
{
    float rad = degree * (float)M_PI / 180.0f;
    return Vector3d(cos(rad) * x - sin(rad) * z, y, sin(rad) * x + cos(rad) * z);
}

float Vector3d::get_rotY()
{
    return atan2(z, x) * 180.0f / M_PI + -270.0f;
}

XMFLOAT3 Vector3d::ToXMFloat3()
{
	return XMFLOAT3(x, y, z);
}

FXMVECTOR Vector3d::ToFXMVector()
{
    XMFLOAT3 xmfloat3 = ToXMFloat3();
    return XMLoadFloat3(&xmfloat3);
}

Vector3d Vector3d::lerp(Vector3d src, Vector3d dst, float t)
{
    Vector3d result;
    t = (t > 1.0f) ? 1.0f : ((t < 0.0f) ? 0.0f : t);
    result.x = src.x + t * (dst.x - src.x);
    result.y = src.y + t * (dst.y - src.y);
    result.z = src.z + t * (dst.z - src.z);
    return result;
}
