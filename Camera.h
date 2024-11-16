#pragma once

#include <DirectXMath.h>
#include "Player.h"

class Camera
{
public:
    Camera();
    ~Camera();

    void Update(const Player& player, float deltaTime);

    void SetPosition(const DirectX::XMFLOAT3& position);
    DirectX::XMFLOAT3 GetPosition();

    DirectX::XMMATRIX GetViewMatrix() const;
    DirectX::XMVECTOR GetDirection() const;

private:
    DirectX::XMFLOAT3 position;
    float pitch; // Rotation around the X-axis
    float yaw;   // Rotation around the Y-axis

    float mouseSensitivity;

    DirectX::XMMATRIX viewMatrix;

    void UpdateViewMatrix();
};
