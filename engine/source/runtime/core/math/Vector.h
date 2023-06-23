#pragma once 
struct Matrix3x3;
extern const double uZero;  
  
struct Vector3  
{  
    float x, y, z; 
    Vector3():x(0), y(0), z(0){}  
    Vector3(float x1, float y1, float z1):x(x1), y(y1), z(z1){}  
    Vector3(const Vector3 &v);  
    ~Vector3();
    void operator=(const Vector3 &v);  
    Vector3 operator+(const Vector3 &v);  
    Vector3 operator+=(const Vector3 &v);
    
    Vector3 operator-();//overload operator - , make it possible to use -Vector3
    Vector3 operator-(const Vector3 &v);
    
    Vector3 operator/(const Vector3 &v);  
    Vector3 operator*(const Vector3 &v)  const;
    Vector3 operator*(float f) const;
    Vector3 operator+(float f);  
    Vector3 operator-(float f);  
    Vector3 operator/(float f);  
    Vector3 operator*(float f);
    float& operator[](int index);
    float dot(const Vector3 &v);  
    float length();
    void normalize();
    Vector3 crossProduct(const Vector3 &v);  
    void printVec3();  

    static Vector3 Zero() { return Vector3(0.0f, 0.0f, 0.0f); }
    static Vector3 Forward() { return Vector3(0.0f, 0.0f, 1.0f); }
    static Vector3 Up() { return Vector3(0.0f, 1.0f, 0.0f); }
    static Vector3 Right() { return Vector3(1.0f, 0.0f, 0.0f); }
};

struct Vector4
{
    float x, y, z, w;
    Vector4():x(0), y(0), z(0), w(0){}
    Vector4(float x1, float y1, float z1, float w1):x(x1), y(y1), z(z1), w(w1){}
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