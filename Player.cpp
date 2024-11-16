#define NOMINMAX
#include "Player.h"
#include <Windows.h>
#include <algorithm>

using namespace DirectX;

Player::Player()
    : mPosition(XMVectorSet(0.0f, 0.0f, -5.0f, 1.0f))
    , mRight(XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f))
    , mUp(XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f))
    , mLook(XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f))
    , mYaw(0.0f)
    , mPitch(0.0f)
    , mMovementSpeed(20.0f)
    , mMouseSensitivity(0.005f)  // Adjusted for direct input
{
    UpdateViewMatrix();
}

Player::~Player()
{
    // Empty destructor implementation
}

void Player::Update(float deltaTime)
{
    // Process movement
    ProcessKeyboardInput(deltaTime);

    // Update view matrix after all changes
    UpdateViewMatrix();
}

void Player::OnMouseMove(int dx, int dy)
{
    // Direct application of mouse movement to rotation
    float deltaYaw = dx * mMouseSensitivity;
    float deltaPitch = dy * mMouseSensitivity;

    mYaw += deltaYaw;
    mPitch += deltaPitch;  // Inverted for natural feel

    // Clamp pitch to prevent flipping
    mPitch = std::max(-XM_PIDIV2 + 0.01f, std::min(XM_PIDIV2 - 0.01f, mPitch));
}

void Player::ProcessKeyboardInput(float deltaTime)
{
    XMVECTOR moveDirection = XMVectorZero();

    if (GetAsyncKeyState('W') & 0x8000)
        moveDirection += mLook;
    if (GetAsyncKeyState('S') & 0x8000)
        moveDirection -= mLook;
    if (GetAsyncKeyState('A') & 0x8000)
        moveDirection -= mRight;
    if (GetAsyncKeyState('D') & 0x8000)
        moveDirection += mRight;

    // Only move if we have input
    if (!XMVector3Equal(moveDirection, XMVectorZero()))
    {
        moveDirection = XMVector3Normalize(moveDirection);
        mPosition += moveDirection * mMovementSpeed * deltaTime;
    }
}

void Player::UpdateViewMatrix()
{
    // Create rotation matrix from yaw and pitch
    XMMATRIX rotationMatrix = XMMatrixRotationRollPitchYaw(mPitch, mYaw, 0.0f);

    // Update camera vectors
    mLook = XMVector3TransformCoord(XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f), rotationMatrix);
    mRight = XMVector3TransformCoord(XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f), rotationMatrix);
    mUp = XMVector3Cross(mLook, mRight);

    // Update view matrix
    mViewMatrix = XMMatrixLookAtLH(mPosition, mPosition + mLook, mUp);
}

XMMATRIX Player::GetViewMatrix() const
{
    return mViewMatrix;
}