// Mesh.h

#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
#include <vector>
#include <string>

class Mesh {
public:
    struct Vertex {
        float x, y, z;      // Position
        float r, g, b;      // Color
    };

    Mesh(ID3D11Device* device, ID3D11DeviceContext* context);
    ~Mesh();

    bool Initialize(const std::vector<Vertex>& vertices, const std::vector<UINT>& indices);
    void Update(float deltaTime);
    void Draw(const DirectX::XMMATRIX& viewProjMatrix);

    // Transformation methods
    void SetPosition(float x, float y, float z);
    void SetRotation(float pitch, float yaw, float roll);
    void SetScale(float x, float y, float z);

    bool LoadFromOBJFile(const std::string& filename);

private:
    struct CBPerObject {
        DirectX::XMMATRIX worldViewProj;
        DirectX::XMMATRIX world;
    };

    // Buffers
    ID3D11Buffer* vertexBuffer;
    ID3D11Buffer* indexBuffer;
    ID3D11Buffer* constantBuffer;

    // Transformation matrices
    DirectX::XMMATRIX worldMatrix;
    DirectX::XMMATRIX rotationMatrix;
    DirectX::XMMATRIX scaleMatrix;
    DirectX::XMMATRIX translationMatrix;

    // Transformation parameters
    float posX, posY, posZ;
    float rotX, rotY, rotZ;
    float scaleX, scaleY, scaleZ;

    // Device and context
    ID3D11Device* device;
    ID3D11DeviceContext* context;

    // Index count
    UINT indexCount;

};
