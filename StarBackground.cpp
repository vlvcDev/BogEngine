// StarBackground.cpp
#include "StarBackground.h"
#include <d3dcompiler.h>

StarBackground::StarBackground() : vertexBuffer(nullptr), indexBuffer(nullptr),
vertexShader(nullptr), pixelShader(nullptr), inputLayout(nullptr) {
}

StarBackground::~StarBackground() {
    if (vertexBuffer) vertexBuffer->Release();
    if (indexBuffer) indexBuffer->Release();
    if (vertexShader) vertexShader->Release();
    if (pixelShader) pixelShader->Release();
    if (inputLayout) inputLayout->Release();
}

bool StarBackground::Initialize(ID3D11Device* device, ID3D11DeviceContext* context) {
    // Create fullscreen quad vertices
    Vertex vertices[] = {
        { DirectX::XMFLOAT3(-1.0f, -1.0f, 0.0f), DirectX::XMFLOAT2(0.0f, 1.0f) },
        { DirectX::XMFLOAT3(-1.0f,  1.0f, 0.0f), DirectX::XMFLOAT2(0.0f, 0.0f) },
        { DirectX::XMFLOAT3(1.0f,  1.0f, 0.0f), DirectX::XMFLOAT2(1.0f, 0.0f) },
        { DirectX::XMFLOAT3(1.0f, -1.0f, 0.0f), DirectX::XMFLOAT2(1.0f, 1.0f) }
    };

    UINT indices[] = { 0, 1, 2, 0, 2, 3 };

    // Create vertex buffer
    D3D11_BUFFER_DESC vbDesc = {};
    vbDesc.ByteWidth = sizeof(vertices);
    vbDesc.Usage = D3D11_USAGE_IMMUTABLE;
    vbDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

    D3D11_SUBRESOURCE_DATA vbData = {};
    vbData.pSysMem = vertices;

    if (FAILED(device->CreateBuffer(&vbDesc, &vbData, &vertexBuffer)))
        return false;

    // Create index buffer
    D3D11_BUFFER_DESC ibDesc = {};
    ibDesc.ByteWidth = sizeof(indices);
    ibDesc.Usage = D3D11_USAGE_IMMUTABLE;
    ibDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;

    D3D11_SUBRESOURCE_DATA ibData = {};
    ibData.pSysMem = indices;

    if (FAILED(device->CreateBuffer(&ibDesc, &ibData, &indexBuffer)))
        return false;

    // Compile shaders using D3DCompile directly
    const char* pixelShaderCode = R"(
        struct PSInput
        {
            float4 position : SV_POSITION;
            float2 texCoord : TEXCOORD;
        };

        float rand(float2 co)
        {
            return frac(cos(dot(co, float2(12.345, 67.890))) * 123.456);
        }

        float4 main(PSInput input) : SV_TARGET
        {
            float2 uv = input.texCoord;
            float2 grid = floor(uv * 40.0);
            float2 pos = grid / 40.0;
            float brightness = rand(grid);
            float dist = length(uv - pos);
            float star = brightness * 0.02 / dist;
            float3 color = star * float3(0.8, 0.9, 1.0);
            return float4(saturate(color), 1.0);
        }
    )";

    const char* vertexShaderCode = R"(
        struct VSInput
        {
            float3 position : POSITION;
            float2 texCoord : TEXCOORD;
        };

        struct PSInput
        {
            float4 position : SV_POSITION;
            float2 texCoord : TEXCOORD;
        };

        PSInput main(VSInput input)
        {
            PSInput output;
            output.position = float4(input.position, 1.0f);
            output.texCoord = input.texCoord;
            return output;
        }
    )";

    // Compile vertex shader
    ID3DBlob* vsBlob = nullptr;
    ID3DBlob* errorBlob = nullptr;
    HRESULT hr = D3DCompile(
        vertexShaderCode,
        strlen(vertexShaderCode),
        "VertexShader",
        nullptr,
        nullptr,
        "main",
        "vs_5_0",
        D3DCOMPILE_ENABLE_STRICTNESS | D3DCOMPILE_OPTIMIZATION_LEVEL3,
        0,
        &vsBlob,
        &errorBlob
    );

    if (FAILED(hr)) {
        if (errorBlob) {
            OutputDebugStringA((char*)errorBlob->GetBufferPointer());
            errorBlob->Release();
        }
        return false;
    }

    // Create vertex shader
    hr = device->CreateVertexShader(vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), nullptr, &vertexShader);
    if (FAILED(hr)) {
        vsBlob->Release();
        return false;
    }

    // Create input layout
    D3D11_INPUT_ELEMENT_DESC layout[] = {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 }
    };

    hr = device->CreateInputLayout(layout, 2, vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), &inputLayout);
    vsBlob->Release();
    if (FAILED(hr)) return false;

    // Compile pixel shader
    ID3DBlob* psBlob = nullptr;
    hr = D3DCompile(
        pixelShaderCode,
        strlen(pixelShaderCode),
        "PixelShader",
        nullptr,
        nullptr,
        "main",
        "ps_5_0",
        D3DCOMPILE_ENABLE_STRICTNESS | D3DCOMPILE_OPTIMIZATION_LEVEL3,
        0,
        &psBlob,
        &errorBlob
    );

    if (FAILED(hr)) {
        if (errorBlob) {
            OutputDebugStringA((char*)errorBlob->GetBufferPointer());
            errorBlob->Release();
        }
        return false;
    }

    // Create pixel shader
    hr = device->CreatePixelShader(psBlob->GetBufferPointer(), psBlob->GetBufferSize(), nullptr, &pixelShader);
    psBlob->Release();
    if (FAILED(hr)) return false;

    return true;
}
void StarBackground::Render(ID3D11DeviceContext* context) {
    // Set the vertex buffer
    UINT stride = sizeof(Vertex);
    UINT offset = 0;
    context->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
    context->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);
    context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    context->IASetInputLayout(inputLayout);

    // Set the shaders
    context->VSSetShader(vertexShader, nullptr, 0);
    context->PSSetShader(pixelShader, nullptr, 0);

    // Draw the fullscreen quad
    context->DrawIndexed(6, 0, 0);
}