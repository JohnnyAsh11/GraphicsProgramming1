#include "Transform.h"

using namespace DirectX;

Transform::Transform() :
    m_v3Position(0.0f, 0.0f, 0.0f),
    m_v3Rotation(0.0f, 0.0f, 0.0f),
    m_v3Scale(1.0f, 1.0f, 1.0f),
    m_bIsDirty(false)
{
    XMStoreFloat4x4(&m_m4WorldMatrix, XMMatrixIdentity());
}

void Transform::SetPosition(float a_fX, float a_fY, float a_fZ)
{
    m_v3Position.x = a_fX;
    m_v3Position.y = a_fY;
    m_v3Position.z = a_fZ;

    m_bIsDirty = true;
}

void Transform::SetPosition(DirectX::XMFLOAT3 a_v3Position)
{
    m_v3Position.x = a_v3Position.x;
    m_v3Position.y = a_v3Position.y;
    m_v3Position.z = a_v3Position.z;

    m_bIsDirty = true;
}

void Transform::SetRotation(float a_fX, float a_fY, float a_fZ)
{
    m_v3Rotation.x = a_fX;
    m_v3Rotation.y = a_fY;
    m_v3Rotation.z = a_fZ;

    m_bIsDirty = true;
}

void Transform::SetRotation(DirectX::XMFLOAT3 a_v3Rotation)
{
    m_v3Rotation.x = a_v3Rotation.x;
    m_v3Rotation.y = a_v3Rotation.y;
    m_v3Rotation.z = a_v3Rotation.z;

    m_bIsDirty = true;
}

void Transform::SetScale(float a_fX, float a_fY, float a_fZ)
{
    m_v3Scale.x = a_fX;
    m_v3Scale.y = a_fY;
    m_v3Scale.z = a_fZ;

    m_bIsDirty = true;
}

void Transform::SetScale(DirectX::XMFLOAT3 a_v3Scale)
{
    m_v3Scale.x = a_v3Scale.x;
    m_v3Scale.y = a_v3Scale.y;
    m_v3Scale.z = a_v3Scale.z;

    m_bIsDirty = true;
}

void Transform::MoveAbsolute(float a_fX, float a_fY, float a_fZ)
{
    // Using the super optimized math functions to move the position.
    XMStoreFloat3(
        &m_v3Position,
        XMLoadFloat3(&m_v3Position) + XMVectorSet(a_fX, a_fY, a_fZ, 0.0f));

    m_bIsDirty = true;
}

void Transform::MoveRelative(float a_fX, float a_fY, float a_fZ)
{
}

void Transform::Rotate(float a_fP, float a_fY, float a_fR)
{
}

void Transform::Scale(float a_fX, float a_fY, float a_fZ)
{
}

DirectX::XMFLOAT3 Transform::GetPosition()
{
    return m_v3Position;
}

DirectX::XMFLOAT3 Transform::GetRotation()
{
    return m_v3Rotation;
}

DirectX::XMFLOAT3 Transform::GetScale()
{
    return m_v3Scale;
}

DirectX::XMFLOAT4X4 Transform::GetWorldMatrix()
{
    if (!m_bIsDirty) return m_m4WorldMatrix;

    m_bIsDirty = false;

    // Calculating matrices based on the rotation/scale/translation vectors.
    XMMATRIX trMatrix = XMMatrixTranslation(m_v3Position.x, m_v3Position.y, m_v3Position.z);
    XMMATRIX roMatrix = XMMatrixRotationRollPitchYaw(m_v3Rotation.x, m_v3Rotation.y, m_v3Rotation.z);
    XMMATRIX scMatrix = XMMatrixScaling(m_v3Scale.x, m_v3Scale.y, m_v3Scale.z);

    // Storing the product of those matrices in the world matrix.
    XMStoreFloat4x4(&m_m4WorldMatrix, scMatrix * roMatrix * trMatrix);
    
    // Returning the resulting value.
    return m_m4WorldMatrix;
}
