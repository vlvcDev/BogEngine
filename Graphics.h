// Graphics.h
#pragma once

#include <d3d11.h>
#include <DirectXMath.h>
#include "Mesh.h"
#include "StarBackground.h"

using namespace DirectX;

class Graphics {
public:
    Graphics();
    ~Graphics();

    bool Initialize(HWND hwnd, int width, int height);
    void Update(float deltaTime);
    void ClearScreen(float r, float g, float b, float a);
    void Present();
    void Draw();
    void SetViewMatrix(const DirectX::XMMATRIX& viewMatrix);
private:
    ID3D11Device* device = nullptr;
    ID3D11DeviceContext* context = nullptr;
    IDXGISwapChain* swapChain = nullptr;
    ID3D11RenderTargetView* renderTargetView = nullptr;

    ID3D11Buffer* vertexBuffer = nullptr;
    ID3D11Buffer* indexBuffer = nullptr;
    ID3D11VertexShader* vertexShader = nullptr;
    ID3D11PixelShader* pixelShader = nullptr;
    ID3D11InputLayout* inputLayout = nullptr;

    ID3D11Buffer* cbPerObjectBuffer = nullptr;

    // Depth buffer components
    ID3D11Texture2D* depthStencilBuffer = nullptr;
    ID3D11DepthStencilView* depthStencilView = nullptr;
    ID3D11DepthStencilState* depthStencilState = nullptr;

    DirectX::XMMATRIX cameraViewMatrix;

    D3D11_VIEWPORT viewport = {};

    Mesh* pyramidMesh;
    Mesh* icosphere;

    DirectX::XMMATRIX viewMatrix;
    DirectX::XMMATRIX projMatrix;

    // Ensure this matches the HLSL structure
    struct CBPerObject {
        DirectX::XMMATRIX worldViewProj; // 64 bytes
        DirectX::XMMATRIX world;         // 64 bytes
        DirectX::XMMATRIX normalMatrix;  // 64 bytes
        DirectX::XMFLOAT4 lightDirection;// 16 bytes
    }cbPerObject;
    // Total size: 64 + 64 + 64 + 16 + 16 + 16 = 240 bytes
    
    StarBackground* starBackground;
};
