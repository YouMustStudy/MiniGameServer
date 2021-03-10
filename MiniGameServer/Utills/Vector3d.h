#pragma once
#include <iostream>
#include <cmath>
#include <DirectXMath.h>

#define M_PI 3.14159265358979323846f
constexpr float TORAD = M_PI / 180.0f;

using namespace DirectX;

class Vector3d
{
public:
    float x, y, z;
    Vector3d() :  //constructor
        x(0),
        y(0),
        z(0)
    {
    }
    Vector3d(float x1, float y1, float z1 = 0) :    //initializing object with values.
        x(x1),
        y(y1),
        z(z1)
    {
    }
    Vector3d(const Vector3d& vec);
    Vector3d operator+(const Vector3d& vec);
    Vector3d& operator+=(const Vector3d& vec);
    Vector3d operator-(const Vector3d& vec);
    Vector3d& operator-=(const Vector3d& vec);
    Vector3d operator*(float value);
    Vector3d& operator*=(float value);
    Vector3d operator/(float value);
    Vector3d& operator/=(float value);
    Vector3d& operator=(const Vector3d& vec);
    float dot(const Vector3d& vec);
    Vector3d cross(const Vector3d& vec);
    float length();
    Vector3d normalize();
    float square();
    float distance(const Vector3d& vec);
    bool isZero();
    Vector3d rotY(float degree);
    float get_rotY();
    XMFLOAT3 ToXMFloat3();
    FXMVECTOR ToFXMVector();

    static Vector3d lerp(Vector3d src, Vector3d dst, float t);
};
