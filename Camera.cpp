#include "Camera.h"
#include "Player.h"  // Include the Player class header if the camera directly accesses player data

using namespace DirectX;

Camera::Camera()
    : position(0.0f, 2.0f, -5.0f),
    pitch(0.0f),
    yaw(0.0f),
    mouseSensitivity(0.1f)
{
    UpdateViewMatrix();
}

Camera::~Camera()
{
}

DirectX::XMFLOAT3 Camera::GetPosition() {
    return position;
}

void Camera::Update(const Player& player, float deltaTime)
{


    UpdateViewMatrix();
}

void Camera::UpdateViewMatrix()
{
    // Calculate the new forward vector from pitch and yaw
    float cosPitch = cosf(XMConvertToRadians(pitch));
    float sinPitch = sinf(XMConvertToRadians(pitch));
    float cosYaw = cosf(XMConvertToRadians(yaw));
    float sinYaw = sinf(XMConvertToRadians(yaw));

    XMFLOAT3 forward;
    forward.x = cosPitch * sinYaw;
    forward.y = sinPitch;
    forward.z = cosPitch * cosYaw;

    XMVECTOR forwardVec = XMVector3Normalize(XMLoadFloat3(&forward));
    XMVECTOR positionVec = XMLoadFloat3(&position);
    XMVECTOR targetVec = positionVec + forwardVec;
    XMVECTOR upVec = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

    viewMatrix = XMMatrixLookAtLH(positionVec, targetVec, upVec);
}

XMMATRIX Camera::GetViewMatrix() const
{
    return viewMatrix;
}

XMVECTOR Camera::GetDirection() const {
    XMFLOAT3 direction(
        cosf(XMConvertToRadians(pitch)) * sinf(XMConvertToRadians(yaw)),
        sinf(XMConvertToRadians(pitch)),
        cosf(XMConvertToRadians(pitch)) * cosf(XMConvertToRadians(yaw))
    );

    return XMVector3Normalize(XMLoadFloat3(&direction));
}

void Camera::SetPosition(const XMFLOAT3& pos) {
    position = pos;  // Update the internal position
    UpdateViewMatrix();  // Recalculate the view matrix based on the new position
}

