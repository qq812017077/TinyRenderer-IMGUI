#pragma once 
struct Matrix3x3;
extern const double uZero;  
struct Vector4;

struct alignas(4) Vector3  
{  
    float x, y, z; 
    Vector3():x(0), y(0), z(0){}  
    Vector3(float x1, float y1, float z1):x(x1), y(y1), z(z1){}  
    Vector3(const Vector3 &v); 
    Vector3(const Vector4 &v);
    ~Vector3();
    void operator=(const Vector3 &v);  

    Vector3 operator+(const Vector3& v) const;
    Vector3 operator+(const Vector3 &v);
    Vector3 operator+(float f) const;
    Vector3& operator+=(const Vector3 &v);
    Vector3& operator+=(float f);

    Vector3 operator-() const;
    Vector3 operator-();//overload operator - , make it possible to use -Vector3
    Vector3 operator-(const Vector3 &v);
    Vector3 operator-(const Vector3 &v) const { return Vector3(x - v.x, y - v.y, z - v.z); }
    Vector3 operator-(float f);
    Vector3& operator-=(float f);
    
    Vector3 operator*(const Vector3 &v)  const;
    Vector3 operator*(float f) const;
    Vector3& operator*=(float f);
    Vector3 operator*(float f);

    Vector3 operator/(const Vector3 &v);
    Vector3 operator/(float f);  
    Vector3& operator/=(float f);

    bool operator==(const Vector3 &v) const
    {
        float num = x - v.x;
        float num2 = y - v.y;
        float num3 = z - v.z;
        float num4 = num * num + num2 * num2 + num3 * num3;
        return num4 < 9.99999944E-11f;
    }

    float& operator[](int index);
    float dot(const Vector3 &v);  
    float length();
    void normalize();
    Vector3 normalized() const;
    Vector3 crossProduct(const Vector3 &v);
    Vector3 crossProduct(const Vector3 &v) const;
    void printVec3();  

    static const Vector3 zero;
    static const Vector3 one;
    static const Vector3 up;
    static const Vector3 right;
    static const Vector3 forward;
    static Vector3 Normalize(const Vector3& v);
    static Vector3 Cross(const Vector3& v1, const Vector3& v2);
    static bool Vector3::Approximately(const Vector3& v1, const Vector3& v2);
    static Vector3 Max(const Vector3& v1, const Vector3& v2);
    static Vector3 Min(const Vector3& v1, const Vector3& v2);
};

struct alignas(4) Vector4
{
    float x, y, z, w;
    Vector4():x(0), y(0), z(0), w(0){}
    Vector4(float x1, float y1, float z1, float w1):x(x1), y(y1), z(z1), w(w1){}
    Vector4(const Vector3 &v, float w);
    Vector4(const Vector4 &v);
    ~Vector4();
    void operator=(const Vector4 &v);
    Vector4 operator+(const Vector4 &v);
    Vector4 operator-(const Vector4 &v);
    Vector4 operator/(const Vector4 &v);
    Vector4 operator*(const Vector4 &v);
    Vector4 operator+(float f);
    Vector4 operator-(float f);
    Vector4 operator/(float f);
    Vector4 operator*(float f);
    float& operator[](int index);
    float dot(const Vector4 &v);
    float length();
    void normalize();
    Vector4 crossProduct(const Vector4 &v);
    void printVec4();
};