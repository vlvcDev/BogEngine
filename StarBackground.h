// StarBackground.h
#pragma once
#include <d3d11.h>
#include <DirectXMath.h>

class StarBackground {
public:
    StarBackground();
    ~StarBackground();

    bool Initialize(ID3D11Device* device, ID3D11DeviceContext* context);
    void Render(ID3D11DeviceContext* context);

private:
    ID3D11Buffer* vertexBuffer;
    ID3D11Buffer* indexBuffer;
    ID3D11VertexShader* vertexShader;
    ID3D11PixelShader* pixelShader;
    ID3D11InputLayout* inputLayout;

    struct Vertex {
        DirectX::XMFLOAT3 position;
        DirectX::XMFLOAT2 texCoord;
    };
};
