#pragma once
#include "Vector.h"

struct Matrix4x4
{
private:
    float data[4][4];

public:
    Matrix4x4();
    Matrix4x4(float* data);
    Matrix4x4(float data[4][4]);
    ~Matrix4x4() = default;
    Matrix4x4(Matrix4x4 const& other) = default;
    Matrix4x4& operator=(Matrix4x4& other) noexcept;

    Matrix4x4 operator*(float val) const;
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
        
    void Matrix4x4::printMat4x4();


    static Matrix4x4 Identity();
    static Matrix4x4 Translation(const Vector3& translation);
    static Matrix4x4 Rotation(const Vector3& rotation);
    static Matrix4x4 Scale(const Vector3& scale);

    static Matrix4x4 LookAt(const Vector3& eye, const Vector3& target, const Vector3& up);
    static Matrix4x4 Perspective(float fov, float aspect, float near, float far);
    static Matrix4x4 Orthographic(float left, float right, float bottom, float top, float near, float far);
};


struct Matrix3x3
{
private:
    float data[3][3];
public:
    Matrix3x3();
    Matrix3x3(float* data);
    Matrix3x3(float data[3][3]);
    ~Matrix3x3() = default;
    Matrix3x3(Matrix3x3 const& other) = default;
    Matrix3x3& operator=(Matrix3x3& other) noexcept;

    Matrix3x3 operator+(Matrix3x3& other) const;
    Matrix3x3& operator+=(Matrix3x3& other);
    Matrix3x3 operator*(float val) const;
    Matrix3x3 operator*(Matrix3x3& other) const;
    Vector3 operator*(Vector3& other) const;
    float* operator[](int index);

    void SetData(float* data);
    void SetData(float data[3][3]);
    float* GetData();
    float GetData(int row, int column) const;
    void SetData(int row, int column, float value);

    Matrix3x3 Transpose() const;
    Matrix3x3 Inverse() const;
    float Determinant() const;

    void Matrix3x3::printMat3x3();


    static Matrix3x3 Identity();
    /**
     * @brief Rotation Matrix: rotate by an angle around an axis(Rodrigues' rotation formula)
     * 
     * @param angle 
     * @param axis 
     * @return Matrix3x3
     */
    static Matrix3x3 Rotation(const Vector3& eulerAngle);
    static Matrix3x3 RotateByAxis(float angle, const Vector3& axis);
    static Matrix3x3 VectorProduct(const Vector3& vec1, const Vector3& vec2);
};