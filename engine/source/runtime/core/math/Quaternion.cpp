#include "Quaternion.h"
#include <cmath>
#include <algorithm>
#include "EngineMath.h"
const Quaternion Quaternion::identity = Quaternion(0.0f, 0.0f, 0.0f, 1.0f);
const Quaternion Quaternion::zero = Quaternion(0.0f, 0.0f, 0.0f, 0.0f);
// 57.29578f = 180 / PI

Quaternion& Quaternion::operator=(const Quaternion& q)
{
    x = q.x;
    y = q.y;
    z = q.z;
    w = q.w;
    return *this;
}

Quaternion Quaternion::operator*(const Quaternion& q) const
{
    return Quaternion(
        w * q.x + x * q.w + y * q.z - z * q.y,
        w * q.y + y * q.w + z * q.x - x * q.z,
        w * q.z + z * q.w + x * q.y - y * q.x,
        w * q.w - x * q.x - y * q.y - z * q.z
    );
}

Quaternion& Quaternion::operator*=(const Quaternion& q)
{
    *this = *this * q;
    return *this;
}
Vector3 Quaternion::operator*(const Vector3& v) const
{
    // nVidia SDK implementation
    Vector3 uv, uuv;
    Vector3 qvec(x, y, z);
    uv  = qvec.crossProduct(v);
    uuv = qvec.crossProduct(uv);
    uv*= (2.0f * w);
    uuv*= 2.0f;
    return v + uv + uuv;
}


bool Quaternion::operator==(const Quaternion& q) const
{
    return x == q.x && y == q.y && z == q.z && w == q.w;
}

bool Quaternion::operator!=(const Quaternion& q) const
{
    return x != q.x || y != q.y || z != q.z || w != q.w;
}

float Quaternion::operator[](int index) const
{
    switch (index)
    {
    case 0:
        return x;
    case 1:
        return y;
    case 2:
        return z;
    case 3:
        return w;
    default:
        return x;
    }
    return (&x)[index];
}

float& Quaternion::operator[](int index)
{
    switch (index)
    {
    case 0:
        return x;
    case 1:
        return y;
    case 2:
        return z;
    case 3:
        return w;
    default:
        return x;
    }
    return (&x)[index];
}

void Quaternion::Set(const Quaternion& q)
{
    x = q.x;
    y = q.y;
    z = q.z;
    w = q.w;
}

void Quaternion::Set(float x, float y, float z, float w)
{
    this->x = x;
    this->y = y;
    this->z = z;
    this->w = w;
}

Quaternion Quaternion::Normalized()
{
    return Normalize();
}
Quaternion Quaternion::Normalize()
{
    *this = Normalize(*this);
    return *this;
}

Vector3 Quaternion::EulerAngles() const
{
    return Internal_MakePositive(Internal_ToEulerRad(*this) * kRad2Deg);
}


void Quaternion::ToAxisAngle(Vector3& axis, float& angle) const
{
    float angleRadian = std::acos(w) * 2.f;
    float sinAngleRadian = std::sin(angleRadian * 0.5f);
    if (std::fabs(sinAngleRadian) < kEpsilon)
    {
        axis.x = 1.f;
        axis.y = 0.f;
        axis.z = 0.f;
    }
    else
    {
        axis.x = x / sinAngleRadian;
        axis.y = y / sinAngleRadian;
        axis.z = z / sinAngleRadian;
    }
    angle = angleRadian * kRad2Deg;
}

void Quaternion::FromAxisAngle(const Vector3& axis, float angle)
{
    Vector3 normalizedAxis = Vector3::Normalize(axis);
    float halfAngleRadian = angle * 0.5f * kDeg2Rad;
    
    float sinHalfAngleRadian = std::sin(halfAngleRadian);
    float cosHalfAngleRadian = std::cos(halfAngleRadian);

    x = normalizedAxis.x * sinHalfAngleRadian;
    y = normalizedAxis.y * sinHalfAngleRadian;
    z = normalizedAxis.z * sinHalfAngleRadian;
    w = cosHalfAngleRadian;
}


void Quaternion::SetEulerAngles(const Vector3& eularAngle)
{
    SetEulerAngles(eularAngle.x, eularAngle.y, eularAngle.z); 
}

void Quaternion::SetEulerAngles(float x, float y, float z)
{
    *this = Quaternion::Euler(x, y, z);
}



void Quaternion::SetLookRotation(const Vector3& view, const Vector3& up)
{
    *this = Quaternion::LookRotation(view, up);
}

void Quaternion::SetFromToRotation(const Vector3& from, const Vector3& to)
{
    *this = Quaternion::FromToRotation(from, to);
}

Quaternion Quaternion::Inverse() const
{
    float norm = w * w + x * x + y * y + z * z;
    if (norm > 0.0)
    {
        float inv_norm = 1.0f / norm;
        return Quaternion(w * inv_norm, -x * inv_norm, -y * inv_norm, -z * inv_norm);
    }
    // return an invalid result to flag the error
    return zero;
}

/**************************************************************/
//                      Static Methods                      //
/**************************************************************/
Quaternion Quaternion::Euler(float x, float y, float z)
{
    Quaternion q = Quaternion::identity;
    q *=  Quaternion::GetQuaternionFromAxisAngle(Vector3::Right(), x);
    q *=  Quaternion::GetQuaternionFromAxisAngle(Vector3::Up(), y);
    q *=  Quaternion::GetQuaternionFromAxisAngle(Vector3::Forward(), z);
    q.Normalize();
    // float cy = std::cos(z * 0.5f * kDeg2Rad);
    // float sy = std::sin(z * 0.5f * kDeg2Rad);
    // float cp = std::cos(y * 0.5f * kDeg2Rad);
    // float sp = std::sin(y * 0.5f * kDeg2Rad);
    // float cr = std::cos(x * 0.5f * kDeg2Rad);
    // float sr = std::sin(x * 0.5f * kDeg2Rad);
    // Quaternion q2;
    // q2.w = cr * cp * cy + sr * sp * sy;
    // q2.x = sr * cp * cy - cr * sp * sy;
    // q2.y = cr * sp * cy + sr * cp * sy;
    // q2.z = cr * cp * sy - sr * sp * cy;
    return q;
}
Quaternion Quaternion::Euler(const Vector3& eularAngle)
{
    return Euler(eularAngle.x, eularAngle.y, eularAngle.z);
}

float Quaternion::Angle(Quaternion a, Quaternion b)
{
    float num = std::min(std::fabs(Dot(a, b)), 1.f);
    return num > 0.999999f? 0.f : (std::acos(num) * 2.f * kRad2Deg);
}

Quaternion Quaternion::FromRotationMatrix(const Matrix3x3& rotation)
{
    Quaternion q;
    float trace = rotation[0][0] + rotation[1][1] + rotation[2][2];
    float root;
    if (trace > 0.0)
    {
        // |w| > 1/2, may as well choose w > 1/2
        root = sqrt(trace + 1.0f); // 2w
        q.w    = 0.5f * root;
        root = 0.5f / root; // 1/(4w)
        q.x    = (rotation[2][1] - rotation[1][2]) * root;
        q.y    = (rotation[0][2] - rotation[2][0]) * root;
        q.z    = (rotation[1][0] - rotation[0][1]) * root;
    }
    else
    {
        // |w| <= 1/2
        int s_iNext[3] = {1, 2, 0};
        int i          = 0;
        if (rotation[1][1] > rotation[0][0])
            i = 1;
        if (rotation[2][2] > rotation[i][i])
            i = 2;
        int j = s_iNext[i];
        int k = s_iNext[j];

        root              = sqrt(rotation[i][i] - rotation[j][j] - rotation[k][k] + 1.0f);
        float* apkQuat[3] = {&q.x, &q.y, &q.z};
        *apkQuat[i]       = 0.5f * root;
        root              = 0.5f / root;
        q.w                 = (rotation[k][j] - rotation[j][k]) * root;
        *apkQuat[j]       = (rotation[j][i] + rotation[i][j]) * root;
        *apkQuat[k]       = (rotation[k][i] + rotation[i][k]) * root;
    }
    return q;
}

Quaternion Quaternion::FromAxes(const Vector3& x_axis, const Vector3& y_axis, const Vector3& z_axis)
{
    Matrix3x3 rot;

    rot[0][0] = x_axis.x;
    rot[1][0] = x_axis.y;
    rot[2][0] = x_axis.z;

    rot[0][1] = y_axis.x;
    rot[1][1] = y_axis.y;
    rot[2][1] = y_axis.z;

    rot[0][2] = z_axis.x;
    rot[1][2] = z_axis.y;
    rot[2][2] = z_axis.z;
    return FromRotationMatrix(rot);
}

Quaternion Quaternion::GetQuaternionFromAxisAngle(const Vector3& axis,float angle)
{
    Quaternion q;
    q.FromAxisAngle(axis, angle);
    return q;
}

Quaternion Quaternion::Inverse(const Quaternion& q)
{
    return q.Inverse();
}

Quaternion Quaternion::Lerp(const Quaternion& kp, const Quaternion& kq, float t, bool shortest_path)
{
    Quaternion result;
    float      cos_value = kp.Dot(kq);
    if (cos_value < 0.0f && shortest_path)
    {
        result = kp + t * ((-kq) - kp);
    }
    else
    {
        result = kp + t * (kq - kp);
    }
    result.Normalize();
    return result;
}

Quaternion Quaternion::Slerp(const Quaternion& a, const Quaternion& b, float t, bool shortest_path)
{
    float cos_v = Dot(a, b);
    Quaternion kt;

    // Do we need to invert rotation?
    if (cos_v < 0.0f && shortest_path)
    {
        cos_v = -cos_v;
        kt    = -b;
    }
    else
    {
        kt = b;
    }

    if (std::abs(cos_v) < 1 - kEpsilon)
    {
        // Standard case (slerp)
        float  sin_v   = std::sqrt(1 - cos_v * cos_v);
        float angleRadian   = std::atan2(sin_v, cos_v);
        float  inv_sin = 1.0f / sin_v;
        float  coeff0  = std::sin((1.0f - t) * angleRadian) * inv_sin;
        float  coeff1  = std::sin(t * angleRadian) * inv_sin;
        return coeff0 * a + coeff1 * kt;
    }
    else
    {
        // There are two situations:
        // 1. "rkP" and "rkQ" are very close (fCos ~= +1), so we can do a linear
        //    interpolation safely.
        // 2. "rkP" and "rkQ" are almost inverse of each other (fCos ~= -1), there
        //    are an infinite number of possibilities interpolation. but we haven't
        //    have method to fix this case, so just use linear interpolation here.
        Quaternion r = (1.0f - t) * a + t * kt;
        // taking the complement requires renormalization
        r.Normalize();
        return r;
    }
}

Quaternion Quaternion::SlerpUnclamped(Quaternion a, Quaternion b, float t)
{
    return Quaternion::Slerp(a, b, t, false);
}

Quaternion Quaternion::LookRotation(const Vector3& forward, const Vector3& up)
{
    Vector3 _forward = forward;
    _forward.z = 0.0f;
    _forward.normalize();

    Vector3 left = Vector3::Cross(up, _forward);
    
    Quaternion q = FromAxes(left, -_forward, up);
    q.Normalize();
    return q;
}

Quaternion Quaternion::FromToRotation(const Vector3& from, const Vector3& to)
{
    Quaternion q;
    Vector3    v0 = from;
    Vector3    v1 = to;
    v0.normalize();
    v1.normalize();

    float d = v0.dot(v1);
    if (d >= 1.0f) return Quaternion::identity;
    

    return q;
}

Quaternion Quaternion::RotateTowards(const Quaternion& from, const Quaternion& to, float maxDegreesDelta)
{
    float num = Angle(from, to);
    if (std::fabs(num) < kEpsilon) return to;

    return SlerpUnclamped(from, to, std::min(1.f, maxDegreesDelta / num));
}

float Quaternion::Dot(const Quaternion& a, const Quaternion& b)
{
    return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
}

Quaternion Quaternion::Normalize(const Quaternion& q)
{
    float num = std::sqrt(Dot(q, q));
    if (num < kEpsilon)
    {
        return identity;
    }

    return Quaternion(q.x / num, q.y / num, q.z / num, q.w / num);
}

Quaternion Quaternion::Identity()
{
    return identity;
}

Vector3 Quaternion::Internal_MakePositive(Vector3 euler)
{
    float num = -0.005729578f;
    float num2 = 360.f + num;
    if (euler.x < num)
    {
        euler.x += 360.f;
    }
    else if (euler.x > num2)
    {
        euler.x -= 360.f;
    }

    if (euler.y < num)
    {
        euler.y += 360.f;
    }
    else if (euler.y > num2)
    {
        euler.y -= 360.f;
    }

    if (euler.z < num)
    {
        euler.z += 360.f;
    }
    else if (euler.z > num2)
    {
        euler.z -= 360.f;
    }

    return euler;
}

Vector3 Quaternion::Internal_ToEulerRad(const Quaternion& rotation)
{
    float pitch = rotation.Internal_GetPitchRad();
    float yaw = rotation.Internal_GetYawRad();
    float roll = rotation.Internal_GetRollRad();
    //check any one is nan
    if (std::isnan(pitch) || std::isnan(yaw) || std::isnan(roll))
    {
        return Vector3::Zero();
    }
    return Vector3(pitch, yaw, roll);
}

float Quaternion::Internal_GetPitchRad() const
{
    float sinr_cosp = 2 * (w * x + y * z);
    float cosr_cosp = 1 - 2 * (x * x + y * y);
    return std::atan2(sinr_cosp, cosr_cosp);
    // return std::atan2(2 * (w * x + y * z), 1 - 2 * (x * x + y * y));
}

float Quaternion::Internal_GetYawRad() const
{
    float sinp = std::sqrt(1 + 2 * (w * y - x * z));
    float cosp = std::sqrt(1 - 2 * (w * y - x * z));
    return 2 * std::atan2(sinp, cosp) - PI / 2;
}

float Quaternion::Internal_GetRollRad() const
{
    
    float siny_cosp = 2 * (w * z + x * y);
    float cosy_cosp = 1 - 2 * (y * y + z * z);
    return std::atan2(siny_cosp, cosy_cosp);
    // return std::atan2(2 * (x * y + w * z), 1 - 2 * (y * y + z * z));
}