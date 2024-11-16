#pragma once

#include <DirectXMath.h>

class Player
{
public:
    Player();
    ~Player();

    // Updates the player's position and orientation
    void Update(float deltaTime);

    // Handles mouse movement input
    void OnMouseMove(int dx, int dy);

    // Returns the current view matrix
    DirectX::XMMATRIX GetViewMatrix() const;

private:
    // Updates the view matrix based on the current position and orientation
    void UpdateViewMatrix();

    // Processes keyboard input for movement
    void ProcessKeyboardInput(float deltaTime);

    // Camera vectors
    DirectX::XMVECTOR mPosition;
    DirectX::XMVECTOR mRight;
    DirectX::XMVECTOR mUp;
    DirectX::XMVECTOR mLook;

    // Euler angles for rotation
    float mYaw;
    float mPitch;
    // Player.h
    float mTargetYaw;
    float mTargetPitch;
    float mRotationSpeed; // Speed at which camera interpolates to target angles

    bool mCursorLocked = true;
    void LockCursor();

    // Movement speeds
    float mMovementSpeed;
    float mMouseSensitivity;

    // View matrix
    DirectX::XMMATRIX mViewMatrix;
};
