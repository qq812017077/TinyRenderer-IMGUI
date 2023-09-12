#pragma once
#include "Vector.h"
struct Matrix3x3;
struct Quaternion
{
    float x;
    float y;
    float z;
    float w;

    Quaternion() : x(0), y(0), z(0), w(1) {}
    Quaternion(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) {}
    Quaternion(const Quaternion& q) : x(q.x), y(q.y), z(q.z), w(q.w) {}
    
    Quaternion& operator=(const Quaternion& q);
    Quaternion operator+(const Quaternion& q) const { return Quaternion(w + q.w, x + q.x, y + q.y, z + q.z); }
    Quaternion operator-(const Quaternion& q) const{ return Quaternion(w - q.w, x - q.x, y - q.y, z - q.z); }
    Quaternion operator-() const { return Quaternion(-w, -x, -y, -z); }
    Quaternion operator*(const Quaternion& q) const;
    Quaternion& operator*=(const Quaternion& q);
    Vector3 operator*(const Vector3& v) const;
    bool operator==(const Quaternion& q) const;
    bool operator!=(const Quaternion& q) const;
    float operator[](int index) const;
    float& operator[](int index);

    
    void Set(const Quaternion& q);
    void Set(float x, float y, float z, float w);
    
    Quaternion Normalized();
    Quaternion Normalize();
    float Dot(const Quaternion& q) const { return Quaternion::Dot(*this, q); }
    
    void SetEulerAngles(const Vector3& eularAngle);
    void SetEulerAngles(float x, float y, float z);
    Vector3 EulerAngles() const;
    
    void FromAxisAngle(const Vector3& axis, float angle);
    void SetLookRotation(const Vector3& view, const Vector3& up = Vector3::up);
    void SetFromToRotation(const Vector3& from, const Vector3& to);
    Quaternion Inverse() const;
        
    static const Quaternion identity;
    static const Quaternion zero;
    static Quaternion Euler(float x, float y, float z);
    static Quaternion Euler(const Vector3& eularAngle);
    static float Angle(Quaternion a, Quaternion b);
    static Quaternion FromRotationMatrix(const Matrix3x3& rotation);
    static Quaternion FromAxes(const Vector3& x_axis, const Vector3& y_axis, const Vector3& z_axis);
    static Quaternion GetQuaternionFromAxisAngle(const Vector3& axis, float angle);
    static Quaternion Inverse(const Quaternion& q);
    static Quaternion Lerp(const Quaternion& a, const Quaternion& b, float t, bool shortest_path = true);
    static Quaternion Slerp(const Quaternion& a, const Quaternion& b, float t, bool shortest_path = true);
    static Quaternion SlerpUnclamped(Quaternion a, Quaternion b, float t);
    static Quaternion LookRotation(const Vector3& forward, const Vector3& up = Vector3::up);
    static Quaternion FromToRotation(const Vector3& from, const Vector3& to);
    static Quaternion RotateTowards(const Quaternion& from, const Quaternion& to, float maxDegreesDelta);
    static float Dot(const Quaternion& a, const Quaternion& b);
    static Quaternion Normalize(const Quaternion& q);
    static Quaternion Identity();

private:
    static Vector3 Internal_MakePositive(Vector3 euler);
    static Vector3 Internal_ToEulerRad(const Quaternion& rotation);
    float Internal_GetRollRad() const;
    float Internal_GetPitchRad() const;
    float Internal_GetYawRad() const;

public:
    friend Quaternion operator*(const float f, const Quaternion& q)
    {
        return Quaternion(f * q.x, f * q.y, f * q.z, f * q.w);
    }
};