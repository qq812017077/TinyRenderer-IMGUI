#pragma once
#include "Vector.h"

class Matrix4x4
{
public:
    Matrix4x4();
    Matrix4x4(float* data);
    Matrix4x4(float data[4][4]);
    ~Matrix4x4() = default;
    Matrix4x4(Matrix4x4 const& other) = default;
    Matrix4x4& operator=(Matrix4x4& other) noexcept;

    
    void Matrix4x4::printMat4x4();
    

    Matrix4x4 operator*(Matrix4x4& other) const;
    Vector4 operator*(Vector4& other) const;
    float* operator[](int index);

    void SetData(float* data);
    void SetData(float data[4][4]);
    float* GetData();
    float GetData(int row, int column) const;
    void SetData(int row, int column, float value);

    Matrix4x4 Transpose() const;
    Matrix4x4 Inverse() const;
    float Determinant() const;
    
    static Matrix4x4 LookAt(const Vector3& eye, const Vector3& target, const Vector3& up);
    static Matrix4x4 Perspective(float fov, float aspect, float near, float far);
    static Matrix4x4 Orthographic(float left, float right, float bottom, float top, float near, float far);
    
    static Matrix4x4 Identity();
    static Matrix4x4 Translation(const Vector3& translation);
    static Matrix4x4 Rotation(const Vector3& rotation);
    static Matrix4x4 Scale(const Vector3& scale);
private:
    float data[4][4];
    
};