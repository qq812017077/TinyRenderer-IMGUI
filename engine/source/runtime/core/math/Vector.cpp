//Vector3.cpp 
#include "Vector.h"
#include "Matrix.h"
#include<iostream>
  
const double uZero = 1e-6;  

/****************************************************************************************/
//Vector3.cpp
/****************************************************************************************/
//复制构造函数，必须为常量引用参数，否则编译不通过  
Vector3::Vector3(const Vector3 &v):x(v.x), y(v.y), z(v.z)  
{  
}  
  
Vector3::~Vector3()
{  
}  
  
void Vector3::operator=(const Vector3 &v)  
{  
    x = v.x;  
    y = v.y;  
    z = v.z;  
}  
  
Vector3 Vector3::operator+(const Vector3 &v)  
{  
    return Vector3(x+v.x, y+v.y, z+v.z);  
}

Vector3 Vector3::operator+=(const Vector3 &v)
{
    x += v.x;
    y += v.y;
    z += v.z;
    return *this;
}

Vector3 Vector3::operator-()  
{  
    return Vector3(-x, -y, -z);  
}

Vector3 Vector3::operator-(const Vector3 &v)  
{  
    return Vector3(x-v.x, y-v.y, z-v.z);  
}  
  
Vector3 Vector3::operator/(const Vector3 &v)  
{  
    if (fabsf(v.x) <= uZero || fabsf(v.y) <= uZero || fabsf(v.z) <= uZero)  
    {  
        std::cerr<<"Over flow!\n";  
        return *this;  
    }  
    return Vector3(x/v.x, y/v.y, z/v.z);  
}  
  
Vector3 Vector3::operator*(const Vector3 &v) const
{  
    return Vector3(x*v.x, y*v.y, z*v.z);  
}

Vector3 Vector3::operator*(float f) const
{
    return Vector3(x*f, y*f, z*f);
}


Vector3 Vector3::operator+(float f)  
{  
    return Vector3(x+f, y+f, z+f);  
}  
  
Vector3 Vector3::operator-(float f)  
{  
    return Vector3(x-f, y-f, z-f);  
}  
  
Vector3 Vector3::operator/(float f)  
{  
    if (fabsf(f) < uZero)  
    {  
        std::cerr<<"Over flow!\n";  
        return *this;  
    }  
    return Vector3(x/f, y/f, z/f);  
}  
  
Vector3 Vector3::operator*(float f)  
{  
    return Vector3(x*f, y*f, z*f);  
}  

float& Vector3::operator[](int index)
{
    return (&x)[index];
}

float Vector3::dot(const Vector3 &v)  
{  
    return x*v.x + y*v.y + z*v.z;  
}  
  
float Vector3::length()  
{  
    return sqrtf(dot(*this));  
}  
  
void Vector3::normalize()  
{  
    float len = length();  
    if (len < uZero) len = 1;  
    len = 1/len;  
  
    x *= len;  
    y *= len;  
    z *= len;  
}  
  
/* 
Cross Product叉乘公式 
aXb = | i,  j,  k  | 
     | a.x a.y a.z| 
     | b.x b.y b.z| = (a.x*b.z -a.z*b.y)i + (a.z*b.x - a.x*b.z)j + (a.x+b.y - a.y*b.x)k  
*/  
Vector3 Vector3::crossProduct(const Vector3 &v)  
{  
    return Vector3(y * v.z - z * v.y,  
                z * v.x - x * v.z,  
                x * v.y - y * v.x);  
}  
  
void Vector3::printVec3()  
{  
    std::cout<<"("<<x<<", "<<y<<", "<<z<<")"<<std::endl;  
}  


/****************************************************************************************/
//Vector4.cpp
/****************************************************************************************/
//复制构造函数，必须为常量引用参数，否则编译不通过
Vector4::Vector4(const Vector4 &v):x(v.x), y(v.y), z(v.z), w(v.w)
{
}

Vector4::~Vector4()
{
}

void Vector4::operator=(const Vector4 &v)
{
    x = v.x;
    y = v.y;
    z = v.z;
    w = v.w;
}

Vector4 Vector4::operator+(const Vector4 &v)
{
    return Vector4(x+v.x, y+v.y, z+v.z, w+v.w);
}

Vector4 Vector4::operator-(const Vector4 &v)
{
    return Vector4(x-v.x, y-v.y, z-v.z, w-v.w);
}

Vector4 Vector4::operator/(const Vector4 &v)
{
    if (fabsf(v.x) <= uZero || fabsf(v.y) <= uZero || fabsf(v.z) <= uZero || fabsf(v.w) <= uZero)
    {
        std::cerr<<"Over flow!\n";
        return *this;
    }
    return Vector4(x/v.x, y/v.y, z/v.z, w/v.w);
}

Vector4 Vector4::operator*(const Vector4 &v)
{
    return Vector4(x*v.x, y*v.y, z*v.z, w*v.w);
}

Vector4 Vector4::operator+(float f)
{
    return Vector4(x+f, y+f, z+f, w+f);
}

Vector4 Vector4::operator-(float f)
{
    return Vector4(x-f, y-f, z-f, w-f);
}

Vector4 Vector4::operator/(float f)
{
    if (fabsf(f) < uZero)
    {
        std::cerr<<"Over flow!\n";
        return *this;
    }
    return Vector4(x/f, y/f, z/f, w/f);
}

Vector4 Vector4::operator*(float f)
{
    return Vector4(x*f, y*f, z*f, w*f);
}


float& Vector4::operator[](int index)
{
    return (&x)[index];
}

float Vector4::dot(const Vector4 &v)
{
    return x*v.x + y*v.y + z*v.z + w*v.w;
}

float Vector4::length()
{
    return sqrtf(dot(*this));
}

void Vector4::normalize()
{
    float len = length();
    if (len < uZero) len = 1;
    len = 1/len;

    x *= len;
    y *= len;
    z *= len;
    w *= len;
}

/*
Cross Product叉乘公式
aXb = | i,  j,  k  |
     | a.x a.y a.z|
     | b.x b.y b.z| = (a.x*b.z -a.z*b.y)i + (a.z*b.x - a.x*b.z)j + (a.x+b.y - a.y*b.x)k
*/

Vector4 Vector4::crossProduct(const Vector4 &v)
{
    return Vector4(y * v.z - z * v.y,
                z * v.x - x * v.z,
                x * v.y - y * v.x,
                0);
}

void Vector4::printVec4()
{
    std::cout<<"("<<x<<", "<<y<<", "<<z<<", "<<w<<")"<<std::endl;
}
