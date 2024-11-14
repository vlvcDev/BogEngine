// Graphics.h
#pragma once

#include <d3d11.h>
#include <DirectXMath.h>
#include "Mesh.h"

using namespace DirectX;

class Graphics {
public:
    Graphics();
    ~Graphics();

    bool Initialize(HWND hwnd, int width, int height);
    void Update(float deltaTime);
    void DrawPyramid();
    void ClearScreen(float r, float g, float b, float a);
    void Present();
    void Draw();

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

    D3D11_VIEWPORT viewport = {};

    Mesh* pyramidMesh;

    DirectX::XMMATRIX viewMatrix;
    DirectX::XMMATRIX projMatrix;

    struct CBPerObject
    {
        DirectX::XMMATRIX world;
        DirectX::XMMATRIX worldViewProj;
    } cbPerObject;
};
