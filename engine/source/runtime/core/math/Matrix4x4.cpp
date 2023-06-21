#include "Matrix4x4.h"
#include <cmath>
#include <iostream>

Matrix4x4::Matrix4x4()
{
    for(int i = 0; i < 16; i++)
    {
        data[i/4][i%4]=0;
    }
}

Matrix4x4::Matrix4x4(float* data)
{
    for(int i = 0; i < 16; i++)
    {
        this->data[i/4][i%4]=data[i];
    }
}

Matrix4x4::Matrix4x4(float data[4][4])
{
    for(int i = 0; i < 16; i++)
    {
        this->data[i/4][i%4]=data[i/4][i%4];
    }
}

Matrix4x4 Matrix4x4::Identity()
{
    Matrix4x4 result;
    for(int i = 0; i < 16; i++)
    {
        result.data[i/4][i%4]=0;
    }
    result.data[0][0]=1;
    result.data[1][1]=1;
    result.data[2][2]=1;
    result.data[3][3]=1;
    return result;
}

Matrix4x4 Matrix4x4::Translation(const Vector3& translation)
{
    Matrix4x4 result = Identity();
    result.data[0][3]=translation.x;
    result.data[1][3]=translation.y;
    result.data[2][3]=translation.z;
    return result;
}

Matrix4x4 Matrix4x4::Rotation(const Vector3& rotation)
{
    Matrix4x4 result = Identity();
    float cosX = std::cos(rotation.x);
    float sinX = std::sin(rotation.x);
    float cosY = std::cos(rotation.y);
    float sinY = std::sin(rotation.y);
    float cosZ = std::cos(rotation.z);
    float sinZ = std::sin(rotation.z);
    result.data[0][0]=cosY*cosZ;
    result.data[0][1]=-cosY*sinZ;
    result.data[0][2]=sinY;
    result.data[1][0]=sinX*sinY*cosZ+cosX*sinZ;
    result.data[1][1]=-sinX*sinY*sinZ+cosX*cosZ;
    result.data[1][2]=-sinX*cosY;
    result.data[2][0]=-cosX*sinY*cosZ+sinX*sinZ;
    result.data[2][1]=cosX*sinY*sinZ+sinX*cosZ;
    result.data[2][2]=cosX*cosY;
    return result;
}

Matrix4x4 Matrix4x4::Scale(const Vector3& scale)
{
    Matrix4x4 result = Identity();
    result.data[0][0]=scale.x;
    result.data[1][1]=scale.y;
    result.data[2][2]=scale.z;
    return result;
}

Matrix4x4 Matrix4x4::operator*(Matrix4x4& other) const
{
    Matrix4x4 result;
    for(int i = 0; i < 16; i++)
    {
        result.data[i/4][i%4]=0;
    }
    for(int i = 0; i < 4; i++)
    {
        for(int j = 0; j < 4;j++)
        {
            for(int k = 0; k < 4; k++)
            {
                result.data[i][j]+=data[i][k]*other.data[k][j];
            }
        }
    }
    return result;
}

Vector4 Matrix4x4::operator*(Vector4& other) const
{
    Vector4 result;
    for(int i = 0; i < 4; i++)
    {
        result[i]=0;
    }
    for(int i = 0; i < 4; i++)
    {
        for(int k = 0; k < 4; k++)
        {
            result[i]+=data[i][k]*other[k];
        }
    }
    return result;
}

float* Matrix4x4::operator[](int index)
{
    return data[index];
}

void Matrix4x4::SetData(float* data)
{
    for(int i = 0; i < 16; i++)
    {
        this->data[i/4][i%4]=data[i];
    }
}

void Matrix4x4::SetData(float data[4][4])
{
    for(int i = 0; i < 16; i++)
    {
        this->data[i/4][i%4]=data[i/4][i%4];
    }
}

float* Matrix4x4::GetData()
{
    float* result = new float[16];
    for(int i = 0; i < 16; i++)
    {
        result[i]=data[i/4][i%4];
    }
    return result;
}

float Matrix4x4::GetData(int row, int column) const
{
    return data[row][column];
}

void Matrix4x4::SetData(int row, int column, float value)
{
    data[row][column]=value;
}

Matrix4x4 Matrix4x4::Transpose() const
{
    Matrix4x4 result;
    for(int i = 0; i < 16; i++)
    {
        result.data[i/4][i%4]=data[i%4][i/4];
    }
    return result;
}

Matrix4x4 Matrix4x4::Inverse() const
{
    throw "Not implemented";
    return Matrix4x4::Identity();
}

float Matrix4x4::Determinant() const
{
    throw "Not implemented";
    return 0;
}

Matrix4x4 Matrix4x4::LookAt(const Vector3& eye, const Vector3& target, const Vector3& up)
{
    throw "Not implemented";
    return Matrix4x4::Identity();
}

Matrix4x4 Matrix4x4::Perspective(float fov, float aspect, float near, float far)
{
    throw "Not implemented";
    return Matrix4x4::Identity();
}

Matrix4x4 Matrix4x4::Orthographic(float left, float right, float bottom, float top, float near, float far)
{
    throw "Not implemented";
    return Matrix4x4::Identity();
}

void Matrix4x4::printMat4x4()
{
    for(int i = 0; i < 4; i++)
    {
        std::cout<<"| ";
        for(int j = 0; j < 4; j++)
        {
            std::cout<<data[i][j]<<" ";
        }
        std::cout<<"|"<<std::endl;
    }
}