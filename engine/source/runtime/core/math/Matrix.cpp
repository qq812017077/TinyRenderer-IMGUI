#include "Matrix.h"
#include <cmath>
#include <iostream>
#include "EngineMath.h"
#include "Quaternion.h"
/**************************************************************************************************/
/*                                        Matrix4x4 Part                                          */
/**************************************************************************************************/
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

Matrix4x4& Matrix4x4::operator=(Matrix4x4& other) noexcept
{
    for(int i = 0; i < 16; i++)
    {
        data[i/4][i%4]=other.data[i/4][i%4];
    }
    return *this;
}


Matrix4x4 Matrix4x4::operator*(float val) const
{
    Matrix4x4 result;
    for(int i = 0; i < 16; i++)
    {
        result.data[i/4][i%4]=data[i/4][i%4]*val;
    }
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
float* Matrix4x4::operator[](int index) const
{
    return (float*)data[index];
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
    Matrix4x4 result = *this;
    for(int i = 0; i < 4; i++)
        for(int j = i + 1; j < 4; j++)
            std::swap(result.data[i][j], result.data[j][i]);
    return result;
}

Matrix4x4 Matrix4x4::Inverse() const
{
    throw "Not implemented";
    return Matrix4x4::Identity();
}

float Matrix4x4::Determinant() const
{
    float det = 0;
    return
        data[0][3] * data[1][2] * data[2][1] * data[3][0] - data[0][2] * data[1][3] * data[2][1] * data[3][0] -
        data[0][3] * data[1][1] * data[2][2] * data[3][0] + data[0][1] * data[1][3] * data[2][2] * data[3][0] +
        data[0][2] * data[1][1] * data[2][3] * data[3][0] - data[0][1] * data[1][2] * data[2][3] * data[3][0] -
        data[0][3] * data[1][2] * data[2][0] * data[3][1] + data[0][2] * data[1][3] * data[2][0] * data[3][1] +
        data[0][3] * data[1][0] * data[2][2] * data[3][1] - data[0][0] * data[1][3] * data[2][2] * data[3][1] -
        data[0][2] * data[1][0] * data[2][3] * data[3][1] + data[0][0] * data[1][2] * data[2][3] * data[3][1] +
        data[0][3] * data[1][1] * data[2][0] * data[3][2] - data[0][1] * data[1][3] * data[2][0] * data[3][2] -
        data[0][3] * data[1][0] * data[2][1] * data[3][2] + data[0][0] * data[1][3] * data[2][1] * data[3][2] +
        data[0][1] * data[1][0] * data[2][3] * data[3][2] - data[0][0] * data[1][1] * data[2][3] * data[3][2] -
        data[0][2] * data[1][1] * data[2][0] * data[3][3] + data[0][1] * data[1][2] * data[2][0] * data[3][3] +
        data[0][2] * data[1][0] * data[2][1] * data[3][3] - data[0][0] * data[1][2] * data[2][1] * data[3][3] -
        data[0][1] * data[1][0] * data[2][2] * data[3][3] + data[0][0] * data[1][1] * data[2][2] * data[3][3];
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

/************************************************************************/
/*                         Static functions                             */
/************************************************************************/

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

Matrix4x4 Matrix4x4::Rotation(const Vector3& eulerAngle)
{
    Matrix4x4 result = Identity();
    // angle to radian
    auto xradian = eulerAngle.x * 3.1415926f / 180.0f;
    auto yradian = eulerAngle.y * 3.1415926f / 180.0f;
    auto zradian = eulerAngle.z * 3.1415926f / 180.0f;
    float cosX = std::cos(xradian);
    float sinX = std::sin(xradian);
    float cosY = std::cos(yradian);
    float sinY = std::sin(yradian);
    float cosZ = std::cos(zradian);
    float sinZ = std::sin(zradian);
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

Matrix4x4 Matrix4x4::Rotation(const Quaternion& rotation)
{
    Matrix4x4 result;
    float x = rotation.x;
    float y = rotation.y;
    float z = rotation.z;
    float w = rotation.w;
    result.data[0][0]=1-2*y*y-2*z*z;
    result.data[0][1]=2*x*y-2*z*w;
    result.data[0][2]=2*x*z+2*y*w;
    result.data[0][3]=0;
    result.data[1][0]=2*x*y+2*z*w;
    result.data[1][1]=1-2*x*x-2*z*z;
    result.data[1][2]=2*y*z-2*x*w;
    result.data[1][3]=0;
    result.data[2][0]=2*x*z-2*y*w;
    result.data[2][1]=2*y*z+2*x*w;
    result.data[2][2]=1-2*x*x-2*y*y;
    result.data[2][3]=0;
    result.data[3][0]=0;
    result.data[3][1]=0;
    result.data[3][2]=0;
    result.data[3][3]=1;
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

/**
 * @brief (Rodrigues' rotation formula)
 * 
 * @param angle 
 * @param axis 
 * @return Matrix4x4 
 */



Matrix4x4 Matrix4x4::Perspective(float fov, float aspect, float near, float far)
{
    auto fovradian = fov * 3.1415926f / 180.0f;
    auto bottom = -near * std::tan(fovradian / 2); // notice that the fov is the vertical fov
    auto top = -bottom;
    auto right = aspect * top;
    auto left = -right;
    auto orth = Matrix4x4::Orthographic(left, right, bottom, top, near, far);
    
    float pers2orth[4][4] = {
        {near, 0, 0, 0},
        {0, near, 0, 0},
        {0, 0, near + far, -near * far},
        {0, 0, 1, 0}
    };
    
    auto projMatrix = orth * Matrix4x4(pers2orth);
    // auto test = DirectX::XMMatrixPerspectiveLH( 1.0f, 1 / aspect ,near, far );
    return projMatrix;
}


/**
 * @brief 
 *          if far and near are negative, we use near - far.
 *          if far and near are positive, we use far - near.
 *          hear, we assume far and near are positive.(but for D3D, far and near are ?)
 *           
 *          Morth = S * T.
 *              S = (   2/(right-left)  0    00               0                   0
 *                      0               2/(top-bottom)      0                   0
 *                      0               0                   2/(far-near)        0
 *                      0               0                   0                   1)
 *              T = (   1               0                   0               -(left+right)/2
 *                  0               1                   0                   -(top+bottom)/2
 *                  0               0                   1                   -(near+far)/2
 *                  0               0                   0                   1)
 * 
 * @param left 
 * @param right 
 * @param bottom 
 * @param top 
 * @param near 
 * @param far 
 * @return Matrix4x4 
 */
Matrix4x4 Matrix4x4::Orthographic(float left, float right, float bottom, float top, float near, float far)
{
    auto trans = Matrix4x4::Translation({-(left+right)/2, -(top+bottom)/2, -(near+far)/2});
    auto scale = Matrix4x4::Scale({2/(right-left), 2/(top-bottom), 2/(far-near)});
    return scale * trans;
}

/**************************************************************************************************/
/*                                        Matrix3x3 Part                                          */
/**************************************************************************************************/
Matrix3x3::Matrix3x3()
{
    for(int i = 0; i < 9; i++)
    {
        data[i/3][i%3]=0;
    }
}

Matrix3x3::Matrix3x3(float* data)
{
    for(int i = 0; i < 9; i++)
    {
        this->data[i/3][i%3]=data[i];
    }
}

Matrix3x3::Matrix3x3(float data[3][3])
{
    for(int i = 0; i < 9; i++)
    {
        this->data[i/3][i%3]=data[i/3][i%3];
    }
}

Matrix3x3& Matrix3x3::operator=(Matrix3x3& other) noexcept
{
    for(int i = 0; i < 9; i++)
    {
        data[i/3][i%3]=other.data[i/3][i%3];
    }
    return *this;
}

Matrix3x3 Matrix3x3::operator+(Matrix3x3& other) const
{
    Matrix3x3 result;
    for(int i = 0; i < 9; i++)
    {
        result.data[i/3][i%3]=data[i/3][i%3]+other.data[i/3][i%3];
    }
    return result;
}

Matrix3x3& Matrix3x3::operator+=(Matrix3x3& other)
{
    for(int i = 0; i < 9; i++)
    {
        data[i/3][i%3]+=other.data[i/3][i%3];
    }
    return *this;
}

Matrix3x3 Matrix3x3::operator*(float val) const
{
    Matrix3x3 result;
    for(int i = 0; i < 9; i++)
    {
        result.data[i/3][i%3]=data[i/3][i%3]*val;
    }
    return result;
}

Matrix3x3 Matrix3x3::operator*(Matrix3x3& other) const
{
    Matrix3x3 result;
    for(int i = 0; i < 9; i++)
    {
        result.data[i/3][i%3]=0;
    }
    for(int i = 0; i < 3; i++)
    {
        for(int j = 0; j < 3;j++)
        {
            for(int k = 0; k < 3; k++)
            {
                result.data[i][j]+=data[i][k]*other.data[k][j];
            }
        }
    }
    return result;
}

Vector3 Matrix3x3::operator*(Vector3& other) const
{
    Vector3 result;
    for(int i = 0; i < 3; i++)
    {
        result[i]=0;
    }
    for(int i = 0; i < 3; i++)
    {
        for(int k = 0; k < 3; k++)
        {
            result[i]+=data[i][k]*other[k];
        }
    }
    return result;
}

float* Matrix3x3::operator[](int index)
{
    return data[index];
}
float* Matrix3x3::operator[](int index) const
{
    return (float*)data[index];
}
void Matrix3x3::SetData(float* data)
{
    for(int i = 0; i < 9; i++)
    {
        this->data[i/3][i%3]=data[i];
    }
}

void Matrix3x3::SetData(float data[3][3])
{
    for(int i = 0; i < 9; i++)
    {
        this->data[i/3][i%3]=data[i/3][i%3];
    }
}

float* Matrix3x3::GetData()
{
    float* result = new float[9];
    for(int i = 0; i < 9; i++)
    {
        result[i]=data[i/3][i%3];
    }
    return result;
}

float Matrix3x3::GetData(int row, int column) const
{
    return data[row][column];
}

void Matrix3x3::SetData(int row, int column, float value)
{
    data[row][column]=value;
}

Matrix3x3 Matrix3x3::Transpose() const
{
    Matrix3x3 result;
    for(int i = 0; i < 3; i++)
        for(int j = i + 1; j < 3; j++)
            std::swap(result.data[i][j], result.data[j][i]);
    return result;
}

Matrix3x3 Matrix3x3::Inverse() const
{
    throw "Not implemented";
    return Matrix3x3::Identity();
}

float Matrix3x3::Determinant() const
{
    // calculate the determinant of a 3x3 matrix
    float det = 0;
    for(int i = 0; i < 3; i++)
    {
        det += data[0][i] * (data[1][(i+1)%3] * data[2][(i+2)%3] - data[1][(i+2)%3] * data[2][(i+1)%3]);
    }
    return det;
}

void Matrix3x3::printMat3x3()
{
    for(int i = 0; i < 3; i++)
    {
        std::cout<<"| ";
        for(int j = 0; j < 3; j++)
        {
            std::cout<<data[i][j]<<" ";
        }
        std::cout<<"|"<<std::endl;
    }
}

/************************************************************************/
/*                         Static functions                             */
/************************************************************************/
Matrix3x3 Matrix3x3::Identity()
{
    Matrix3x3 result;
    for(int i = 0; i < 9; i++)
    {
        result.data[i/3][i%3]=0;
    }
    result.data[0][0]=1;
    result.data[1][1]=1;
    result.data[2][2]=1;
    return result;
}

Matrix3x3 Matrix3x3::Rotation(const Vector3& eulerAngle)
{
    // calculate the rotation matrix from euler angle
    Matrix3x3 result;
    // angle to radian
    auto xradian = eulerAngle.x * kDeg2Rad;
    auto yradian = eulerAngle.y * kDeg2Rad;
    auto zradian = eulerAngle.z * kDeg2Rad;

    float cosX = std::cos(xradian);
    float sinX = std::sin(xradian);
    float cosY = std::cos(yradian);
    float sinY = std::sin(yradian);
    float cosZ = std::cos(zradian);
    float sinZ = std::sin(zradian);
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

Matrix3x3 Matrix3x3::Rotation(const Quaternion& rotation)
{
    Matrix3x3 result;
    float x = rotation.x;
    float y = rotation.y;
    float z = rotation.z;
    float w = rotation.w;
    result.data[0][0]=1-2*y*y-2*z*z;
    result.data[0][1]=2*x*y-2*z*w;
    result.data[0][2]=2*x*z+2*y*w;
    result.data[1][0]=2*x*y+2*z*w;
    result.data[1][1]=1-2*x*x-2*z*z;
    result.data[1][2]=2*y*z-2*x*w;
    result.data[2][0]=2*x*z-2*y*w;
    result.data[2][1]=2*y*z+2*x*w;
    result.data[2][2]=1-2*x*x-2*y*y;
    return result;
}

/***
 * @brief Rotation Matrix: rotate by an angle around an axis(Rodrigues' rotation formula)
*/
Matrix3x3 Matrix3x3::RotateByAxis(float angle, const Vector3& axis)
{
    auto result = Identity() * std::cos(angle);
    result += Matrix3x3::VectorProduct(axis, axis) * (1 - std::cos(angle));
    //use Matrix3x3(float data[3][3])
    float data[3][3]=
    {
        {0, -axis.z, axis.y},
        {axis.z, 0, -axis.x},
        {-axis.y, axis.x, 0}
    };
    result += Matrix3x3(data) * std::sin(angle);
    return result;
}

Matrix3x3 Matrix3x3::VectorProduct(const Vector3& vec1, const Vector3& vec2)
{
    Matrix3x3 result;
    result.data[0][0]=vec1.x*vec2.x;
    result.data[0][1]=vec1.x*vec2.y;
    result.data[0][2]=vec1.x*vec2.z;
    result.data[1][0]=vec1.y*vec2.x;
    result.data[1][1]=vec1.y*vec2.y;
    result.data[1][2]=vec1.y*vec2.z;
    result.data[2][0]=vec1.z*vec2.x;
    result.data[2][1]=vec1.z*vec2.y;
    result.data[2][2]=vec1.z*vec2.z;
    return result;
}