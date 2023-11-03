#include "Matrix.h"
#include <cmath>
#include <iostream>
#include "EngineMath.h"
#include "Quaternion.h"
#include <DirectXMath.h>
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

Matrix4x4::Matrix4x4(float a1, float a2, float a3, float a4,
        float b1, float b2, float b3, float b4,
        float c1, float c2, float c3, float c4,
        float d1, float d2, float d3, float d4)
{
    data[0][0]=a1;
    data[0][1]=a2;
    data[0][2]=a3;
    data[0][3]=a4;
    data[1][0]=b1;
    data[1][1]=b2;
    data[1][2]=b3;
    data[1][3]=b4;
    data[2][0]=c1;
    data[2][1]=c2;
    data[2][2]=c3;
    data[2][3]=c4;
    data[3][0]=d1;
    data[3][1]=d2;
    data[3][2]=d3;
    data[3][3]=d4;
}
Matrix4x4::Matrix4x4(float a[4], float b[4], float c[4], float d[4])
{
    for(int i = 0; i < 4; i++)
    {
        data[0][i]=a[i];
        data[1][i]=b[i];
        data[2][i]=c[i];
        data[3][i]=d[i];
    }
}

Matrix4x4::Matrix4x4(Vector4& a, Vector4& b, Vector4& c, Vector4& d)
{
    for(int i = 0; i < 4; i++)
    {
        data[0][i]=a[i];
        data[1][i]=b[i];
        data[2][i]=c[i];
        data[3][i]=d[i];
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
    return reinterpret_cast<float*>(data);
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
    auto det = Determinant();
    try 
    {
        float inv_det = 1.0f / det;
        Matrix4x4 inv_mat;
        inv_mat[0][0] = data[1][2] * data[2][3] * data[3][1] - data[1][3] * data[2][2] * data[3][1] + data[1][3] * data[2][1] * data[3][2] - data[1][1] * data[2][3] * data[3][2] - data[1][2] * data[2][1] * data[3][3] + data[1][1] * data[2][2] * data[3][3];
        inv_mat[0][1] = data[0][3] * data[2][2] * data[3][1] - data[0][2] * data[2][3] * data[3][1] - data[0][3] * data[2][1] * data[3][2] + data[0][1] * data[2][3] * data[3][2] + data[0][2] * data[2][1] * data[3][3] - data[0][1] * data[2][2] * data[3][3];
        inv_mat[0][2] = data[0][2] * data[1][3] * data[3][1] - data[0][3] * data[1][2] * data[3][1] + data[0][3] * data[1][1] * data[3][2] - data[0][1] * data[1][3] * data[3][2] - data[0][2] * data[1][1] * data[3][3] + data[0][1] * data[1][2] * data[3][3];
        inv_mat[0][3] = data[0][3] * data[1][2] * data[2][1] - data[0][2] * data[1][3] * data[2][1] - data[0][3] * data[1][1] * data[2][2] + data[0][1] * data[1][3] * data[2][2] + data[0][2] * data[1][1] * data[2][3] - data[0][1] * data[1][2] * data[2][3];
        inv_mat[1][0] = data[1][3] * data[2][2] * data[3][0] - data[1][2] * data[2][3] * data[3][0] - data[1][3] * data[2][0] * data[3][2] + data[1][0] * data[2][3] * data[3][2] + data[1][2] * data[2][0] * data[3][3] - data[1][0] * data[2][2] * data[3][3];
        inv_mat[1][1] = data[0][2] * data[2][3] * data[3][0] - data[0][3] * data[2][2] * data[3][0] + data[0][3] * data[2][0] * data[3][2] - data[0][0] * data[2][3] * data[3][2] - data[0][2] * data[2][0] * data[3][3] + data[0][0] * data[2][2] * data[3][3];
        inv_mat[1][2] = data[0][3] * data[1][2] * data[3][0] - data[0][2] * data[1][3] * data[3][0] - data[0][3] * data[1][0] * data[3][2] + data[0][0] * data[1][3] * data[3][2] + data[0][2] * data[1][0] * data[3][3] - data[0][0] * data[1][2] * data[3][3];
        inv_mat[1][3] = data[0][2] * data[1][3] * data[2][0] - data[0][3] * data[1][2] * data[2][0] + data[0][3] * data[1][0] * data[2][2] - data[0][0] * data[1][3] * data[2][2] - data[0][2] * data[1][0] * data[2][3] + data[0][0] * data[1][2] * data[2][3];
        inv_mat[2][0] = data[1][1] * data[2][3] * data[3][0] - data[1][3] * data[2][1] * data[3][0] + data[1][3] * data[2][0] * data[3][1] - data[1][0] * data[2][3] * data[3][1] - data[1][1] * data[2][0] * data[3][3] + data[1][0] * data[2][1] * data[3][3];
        inv_mat[2][1] = data[0][3] * data[2][1] * data[3][0] - data[0][1] * data[2][3] * data[3][0] - data[0][3] * data[2][0] * data[3][1] + data[0][0] * data[2][3] * data[3][1] + data[0][1] * data[2][0] * data[3][3] - data[0][0] * data[2][1] * data[3][3];
        inv_mat[2][2] = data[0][1] * data[1][3] * data[3][0] - data[0][3] * data[1][1] * data[3][0] + data[0][3] * data[1][0] * data[3][1] - data[0][0] * data[1][3] * data[3][1] - data[0][1] * data[1][0] * data[3][3] + data[0][0] * data[1][1] * data[3][3];
        inv_mat[2][3] = data[0][3] * data[1][1] * data[2][0] - data[0][1] * data[1][3] * data[2][0] - data[0][3] * data[1][0] * data[2][1] + data[0][0] * data[1][3] * data[2][1] + data[0][1] * data[1][0] * data[2][3] - data[0][0] * data[1][1] * data[2][3];
        inv_mat[3][0] = data[1][2] * data[2][1] * data[3][0] - data[1][1] * data[2][2] * data[3][0] - data[1][2] * data[2][0] * data[3][1] + data[1][0] * data[2][2] * data[3][1] + data[1][1] * data[2][0] * data[3][2] - data[1][0] * data[2][1] * data[3][2];
        inv_mat[3][1] = data[0][1] * data[2][2] * data[3][0] - data[0][2] * data[2][1] * data[3][0] + data[0][2] * data[2][0] * data[3][1] - data[0][0] * data[2][2] * data[3][1] - data[0][1] * data[2][0] * data[3][2] + data[0][0] * data[2][1] * data[3][2];
        inv_mat[3][2] = data[0][2] * data[1][1] * data[3][0] - data[0][1] * data[1][2] * data[3][0] - data[0][2] * data[1][0] * data[3][1] + data[0][0] * data[1][2] * data[3][1] + data[0][1] * data[1][0] * data[3][2] - data[0][0] * data[1][1] * data[3][2];
        inv_mat[3][3] = data[0][1] * data[1][2] * data[2][0] - data[0][2] * data[1][1] * data[2][0] + data[0][2] * data[1][0] * data[2][1] - data[0][0] * data[1][2] * data[2][1] - data[0][1] * data[1][0] * data[2][2] + data[0][0] * data[1][1] * data[2][2];

        
        for (int row_index = 0; row_index < 4; row_index++)
        {
            for (int col_index = 0; col_index < 4; col_index++)
                inv_mat[row_index][col_index] *= inv_det;
        }

        return inv_mat;
    }catch(const std::exception& e)
    {
        std::cout << "Matrix4x4::Inverse() : determinat is too small, however we use float to store the data, it cannot express the exact value of the determinant, so we think it is zero. and it caused this error." << std::endl;
        std::cout << e.what() << std::endl;
        return Matrix4x4::Zero();
    }
    
    
    return Matrix4x4::Zero();
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

Matrix4x4 Matrix4x4::Rotation(Vector3 eulerAngle)
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
/**
 * @brief 
 *      rotation matrix is a matrix composed of three orthogonal unit vectors, 
 *      each of which is the direction of the x, y, and z axes after rotation
 * 
 * @param forward 
 * @param up 
 * @return Matrix4x4 
 */
Matrix4x4 Matrix4x4::LookRotation(Vector3 forward, Vector3 up)
{
    Vector3 zaxis = forward.normalized();
    Vector3 xaxis = up.crossProduct(zaxis).normalized();
    Vector3 yaxis = zaxis.crossProduct(xaxis);

    Matrix4x4 result{
        xaxis.x, xaxis.y, xaxis.z, 0,
        yaxis.x, yaxis.y, yaxis.z, 0,
        zaxis.x, zaxis.y, zaxis.z, 0,
        0, 0, 0, 1
    };
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

Matrix4x4 Matrix4x4::RotationX(float radian)
{
    Matrix4x4 result;
    float cosX = std::cos(radian);
    float sinX = std::sin(radian);
    result.data[0][0]=1;
    result.data[1][1]=cosX;
    result.data[1][2]=-sinX;
    result.data[2][1]=sinX;
    result.data[2][2]=cosX;
    result.data[3][3]=1;
    return result;
}
Matrix4x4 Matrix4x4::RotationY(float radian)
{
    Matrix4x4 result;
    float cosY = std::cos(radian);
    float sinY = std::sin(radian);
    result.data[0][0]=cosY;
    result.data[0][2]=sinY;
    result.data[1][1]=1;
    result.data[2][0]=-sinY;
    result.data[2][2]=cosY;
    result.data[3][3]=1;
    return result;
}
Matrix4x4 Matrix4x4::RotationZ(float radian)
{
    Matrix4x4 result;
    float cosZ = std::cos(radian);
    float sinZ = std::sin(radian);
    result.data[0][0]=cosZ;
    result.data[0][1]=-sinZ;
    result.data[1][0]=sinZ;
    result.data[1][1]=cosZ;
    result.data[2][2]=1;
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

Matrix4x4 Matrix4x4::LookAtLH(Vector3 eye, Vector3 target, Vector3 up)
{
    // auto dxresult = DirectX::XMMatrixLookAtLH(
    //     DirectX::XMVectorSet(eye.x, eye.y, eye.z, 1.0f),
    //     DirectX::XMVectorSet(target.x, target.y, target.z, 1.0f),
    //     DirectX::XMVectorSet(up.x, up.y, up.z, 1.0f)
    // );
    Vector3 zaxis = (target - eye).normalized();
    Vector3 xaxis = up.crossProduct(zaxis).normalized();
    Vector3 yaxis = zaxis.crossProduct(xaxis);

    Vector3 negEyePos = -eye;
    
    Matrix4x4 result{
        xaxis.x, xaxis.y, xaxis.z, negEyePos.dot(xaxis),
        yaxis.x, yaxis.y, yaxis.z, negEyePos.dot(yaxis),
        zaxis.x, zaxis.y, zaxis.z, negEyePos.dot(zaxis),
        0, 0, 0, 1
    };
    return result;
}

Matrix4x4 Matrix4x4::LookAtRH(Vector3 eye, Vector3 target, Vector3 up)
{
    Vector3 zaxis = (eye - target).normalized();    //eye direction is opposite to zaxis
    Vector3 xaxis = up.crossProduct(zaxis).normalized();
    Vector3 yaxis = zaxis.crossProduct(xaxis);

    Vector3 negEyePos = -eye;
    
    Matrix4x4 result{
        xaxis.x, xaxis.y, xaxis.z, negEyePos.dot(xaxis),
        yaxis.x, yaxis.y, yaxis.z, negEyePos.dot(yaxis),
        zaxis.x, zaxis.y, zaxis.z, negEyePos.dot(zaxis),
        0, 0, 0, 1
    };
    return result;
}

Matrix4x4 Matrix4x4::TRS(const Vector3& translation, const Quaternion& rotation, const Vector3& scale)
{
    auto transMat = Matrix4x4::Translation(translation);
    auto rotMat = Matrix4x4::Rotation(rotation);
    auto scaleMat = Matrix4x4::Scale(scale);
    return transMat * rotMat * scaleMat;
}


Matrix4x4 Matrix4x4::Perspective(float fov, float aspect, float near, float far)
{
    float fRange = 1.0f / (far - near);
    auto fovradian = fov * 3.1415926f / 180.0f;
    float tan_half_fov = std::tan(fovradian / 2);
    auto height = 2 * near * tan_half_fov;
    auto width = height * aspect;

    float pers2orth[4][4] = {
        {near, 0, 0, 0},
        {0, near, 0, 0},
        {0, 0, near + far, -near * far},
        {0, 0, 1, 0}
    };
    auto projMatrix = OrthographicLH(width, height, near, far) * Matrix4x4(pers2orth);
    return projMatrix;
}




Matrix4x4 Matrix4x4::OrthographicLH(float width, float height, float zNear, float zFar)
{
    float fRange = 1.0f / (zFar - zNear);
    float orth[4][4] ={
        {2.0f / width, 0, 0, 0},
        {0, 2.0f / height, 0, 0},
        {0, 0, fRange, -zNear * fRange},
        {0, 0, 0, 1}
    };

    return Matrix4x4(orth);
}
/**
 * @brief 
 *  NOTE: it if different to openGL, in D3D, NDC is [0,1], but in openGL, NDC is [-1,1].
 *         D3D is left-hand, so : 0 is near plane, 1 is far plane.
 *               near and far are positive.
 *          if far and near are positive, we use far - near.
 *          Morth = S * T.
 *              S = (   2/(right-left)  0    00               0                   0
 *                      0               2/(top-bottom)      0                   0
 *                      0               0                   2/(far-near)        0
 *                      0               0                   0                   1)
 *              T = (   1               0                   0               -(left+right)/2
 *                  0               1                   0                   -(top+bottom)/2
 *                  0               0                   1                   -near
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
Matrix4x4 Matrix4x4::OrthographicLH(float left, float right, float bottom, float top, float near, float far)
{
    float fRange = 1.0f / (far - near);
    auto viewWidth = right - left;
    auto viewHeight = top - bottom;
    return OrthographicLH(viewWidth, viewHeight, near, far);
    // XMMATRIX M;
    // M.m[0][0] = 2.0f / ViewWidth;
    // M.m[0][1] = 0.0f;
    // M.m[0][2] = 0.0f;
    // M.m[0][3] = 0.0f;

    // M.m[1][0] = 0.0f;
    // M.m[1][1] = 2.0f / ViewHeight;
    // M.m[1][2] = 0.0f;
    // M.m[1][3] = 0.0f;

    // M.m[2][0] = 0.0f;
    // M.m[2][1] = 0.0f;
    // M.m[2][2] = fRange;
    // M.m[2][3] = 0.0f;

    // M.m[3][0] = 0.0f;
    // M.m[3][1] = 0.0f;
    // M.m[3][2] = -fRange * NearZ;
    // M.m[3][3] = 1.0f;
    // return M;0
    // auto trans = Matrix4x4::Translation({-(left+right)/2, -(top+bottom)/2, -(near+far)/2});
    // auto scale = Matrix4x4::Scale({2/(right-left), 2/(top-bottom), 2/(far-near)});
    // return scale * trans;
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

Vector3 Matrix3x3::operator*(const Vector3& other) const
{
    auto right = other;
    Vector3 result;
    for(int i = 0; i < 3; i++) result[i]=0;
    for(int i = 0; i < 3; i++)
    {
        for(int k = 0; k < 3; k++)
        {
            result[i]+=data[i][k]*right[k];
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
    float det = Determinant();
    if (std::fabs(det) <= 1e-06)
        return Matrix3x3::Zero();
    Matrix3x3 inv_mat;
    inv_mat[0][0] = data[1][1] * data[2][2] - data[1][2] * data[2][1];
    inv_mat[0][1] = data[0][2] * data[2][1] - data[0][1] * data[2][2];
    inv_mat[0][2] = data[0][1] * data[1][2] - data[0][2] * data[1][1];
    inv_mat[1][0] = data[1][2] * data[2][0] - data[1][0] * data[2][2];
    inv_mat[1][1] = data[0][0] * data[2][2] - data[0][2] * data[2][0];
    inv_mat[1][2] = data[0][2] * data[1][0] - data[0][0] * data[1][2];
    inv_mat[2][0] = data[1][0] * data[2][1] - data[1][1] * data[2][0];
    inv_mat[2][1] = data[0][1] * data[2][0] - data[0][0] * data[2][1];
    inv_mat[2][2] = data[0][0] * data[1][1] - data[0][1] * data[1][0];

    float inv_det = 1.0f / det;
    for (int row_index = 0; row_index < 3; row_index++)
    {
        for (int col_index = 0; col_index < 3; col_index++)
            inv_mat[row_index][col_index] *= inv_det;
    }
    return inv_mat;
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
    result.data[0][0]=1-2*y*y-2*z*z;    // 1-2y^2-2z^2 -> cos(yaw)*cos(pitch)
    result.data[0][1]=2*x*y-2*z*w;      // 2xy-2zw -> sin(yaw)*cos(pitch)
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