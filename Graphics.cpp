// Graphics.cpp

#include "Graphics.h"
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <dxgi.h>
#include <dxgi1_2.h>
#pragma comment(lib, "dxgi.lib")

using namespace DirectX;

#pragma comment(lib, "d3dcompiler.lib")

#include "Mesh.h"
#include "ShapeGenerator.h"
#include <vector>
#include "Window.h"


Graphics::Graphics() {}

Graphics::~Graphics() {
    if (cbPerObjectBuffer) cbPerObjectBuffer->Release();
    if (inputLayout) inputLayout->Release();
    if (pixelShader) pixelShader->Release();
    if (vertexShader) vertexShader->Release();
    if (indexBuffer) indexBuffer->Release();
    if (vertexBuffer) vertexBuffer->Release();
    if (depthStencilState) depthStencilState->Release();
    if (depthStencilView) depthStencilView->Release();
    if (depthStencilBuffer) depthStencilBuffer->Release();
    if (renderTargetView) renderTargetView->Release();
    if (swapChain) swapChain->Release();
    if (context) context->Release();
    if (device) device->Release();
}

ID3DBlob* CompileShader(const wchar_t* filename, const char* entryPoint, const char* target) {
    ID3DBlob* shaderBlob = nullptr;
    ID3DBlob* errorBlob = nullptr;
    HRESULT hr = D3DCompileFromFile(filename, nullptr, nullptr, entryPoint, target, 0, 0, &shaderBlob, &errorBlob);
    if (FAILED(hr)) {
        if (errorBlob) {
            OutputDebugStringA((char*)errorBlob->GetBufferPointer());
            errorBlob->Release();
        }
        return nullptr; // Return nullptr on failure
    }
    return shaderBlob;
}


bool Graphics::Initialize(HWND hwnd, int width, int height) {
    // Calculate the aspect ratio
    float aspectRatio = static_cast<float>(width) / static_cast<float>(height);

    IDXGIFactory1* pFactory = nullptr;
    HRESULT hr = CreateDXGIFactory1(__uuidof(IDXGIFactory1), (void**)&pFactory);
    if (FAILED(hr)) {
        MessageBox(hwnd, L"Failed to create DXGI Factory!", L"Error", MB_OK);
        return false;
    }

    // Enumerate adapters to find the discrete GPU
    IDXGIAdapter1* pAdapter = nullptr;
    IDXGIAdapter1* selectedAdapter = nullptr;
    UINT adapterIndex = 0;
    SIZE_T maxDedicatedVideoMemory = 0;

    while (pFactory->EnumAdapters1(adapterIndex, &pAdapter) != DXGI_ERROR_NOT_FOUND) {
        DXGI_ADAPTER_DESC1 adapterDesc;
        pAdapter->GetDesc1(&adapterDesc);

        // Skip software adapters if necessary
        if (!(adapterDesc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)) {
            // Prefer the adapter with the most dedicated video memory
            if (adapterDesc.DedicatedVideoMemory > maxDedicatedVideoMemory) {
                maxDedicatedVideoMemory = adapterDesc.DedicatedVideoMemory;
                if (selectedAdapter) selectedAdapter->Release();
                selectedAdapter = pAdapter;
                // Do not release pAdapter here since we keep it as selectedAdapter
            }
            else {
                pAdapter->Release();
            }
        }
        else {
            pAdapter->Release();
        }

        adapterIndex++;
    }

    if (!selectedAdapter) {
        MessageBox(hwnd, L"No suitable graphics adapter found!", L"Error", MB_OK);
        pFactory->Release();
        return false;
    }

    DXGI_SWAP_CHAIN_DESC scd = {};
    scd.BufferCount = 1;
    scd.BufferDesc.Width = width;
    scd.BufferDesc.Height = height;
    scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    scd.BufferDesc.RefreshRate.Numerator = 60;
    scd.BufferDesc.RefreshRate.Denominator = 1;
    scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    scd.OutputWindow = hwnd;
    scd.SampleDesc.Count = 1;
    scd.SampleDesc.Quality = 0;
    scd.Windowed = TRUE;
    scd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

    UINT createDeviceFlags = D3D11_CREATE_DEVICE_DEBUG;

    hr = D3D11CreateDeviceAndSwapChain(
        selectedAdapter, D3D_DRIVER_TYPE_UNKNOWN, nullptr, createDeviceFlags, nullptr, 0,
        D3D11_SDK_VERSION, &scd, &swapChain, &device, nullptr, &context);

    selectedAdapter->Release();
    pFactory->Release();

    if (FAILED(hr)) {
        MessageBox(hwnd, L"Failed to create Direct3D device and swap chain!", L"Error", MB_OK);
        return false;
    }

    // Create render target view
    ID3D11Texture2D* backBuffer = nullptr;
    hr = swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&backBuffer);
    if (FAILED(hr)) {
        MessageBox(hwnd, L"Failed to get back buffer!", L"Error", MB_OK);
        return false;
    }

    hr = device->CreateRenderTargetView(backBuffer, nullptr, &renderTargetView);
    backBuffer->Release();
    if (FAILED(hr)) {
        MessageBox(hwnd, L"Failed to create render target view!", L"Error", MB_OK);
        return false;
    }

    // Create depth-stencil buffer
    D3D11_TEXTURE2D_DESC depthStencilDesc = {};
    depthStencilDesc.Width = width;
    depthStencilDesc.Height = height;
    depthStencilDesc.MipLevels = 1;
    depthStencilDesc.ArraySize = 1;
    depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    depthStencilDesc.SampleDesc.Count = 1;
    depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;

    hr = device->CreateTexture2D(&depthStencilDesc, nullptr, &depthStencilBuffer);
    if (FAILED(hr)) {
        MessageBox(hwnd, L"Failed to create depth stencil buffer!", L"Error", MB_OK);
        return false;
    }

    // Create depth-stencil view
    hr = device->CreateDepthStencilView(depthStencilBuffer, nullptr, &depthStencilView);
    if (FAILED(hr)) {
        MessageBox(hwnd, L"Failed to create depth stencil view!", L"Error", MB_OK);
        return false;
    }

    // Bind the render target view and depth stencil view
    context->OMSetRenderTargets(1, &renderTargetView, depthStencilView);

    // Set up the viewport
    viewport.Width = static_cast<float>(width);
    viewport.Height = static_cast<float>(height);
    viewport.MinDepth = 0.0f;
    viewport.MaxDepth = 1.0f;
    viewport.TopLeftX = 0.0f;
    viewport.TopLeftY = 0.0f;
    context->RSSetViewports(1, &viewport);

    // Compile vertex shader
    ID3DBlob* vsBlob = CompileShader(L"VertexShader.hlsl", "main", "vs_5_0");
    if (!vsBlob) {
        MessageBox(hwnd, L"Failed to compile vertex shader!", L"Error", MB_OK);
        return false;
    }

    hr = device->CreateVertexShader(vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), nullptr, &vertexShader);
    if (FAILED(hr)) {
        vsBlob->Release();
        MessageBox(hwnd, L"Failed to create vertex shader!", L"Error", MB_OK);
        return false;
    }

    context->VSSetShader(vertexShader, nullptr, 0);

    // Compile pixel shader
    ID3DBlob* psBlob = CompileShader(L"PixelShader.hlsl", "main", "ps_5_0");
    if (!psBlob) {
        vsBlob->Release();
        MessageBox(hwnd, L"Failed to compile pixel shader!", L"Error", MB_OK);
        return false;
    }

    hr = device->CreatePixelShader(psBlob->GetBufferPointer(), psBlob->GetBufferSize(), nullptr, &pixelShader);
    if (FAILED(hr)) {
        vsBlob->Release();
        psBlob->Release();
        MessageBox(hwnd, L"Failed to create pixel shader!", L"Error", MB_OK);
        return false;
    }

    context->PSSetShader(pixelShader, nullptr, 0);

    projMatrix = XMMatrixPerspectiveFovLH(
        XM_PIDIV4,          // Field of view angle (45 degrees)
        aspectRatio,        // Aspect ratio
        0.1f,               // Near clipping plane
        200.0f              // Far clipping plane
    );

    // Create the pyramid mesh
    pyramidMesh = new Mesh(device, context);

    // Generate pyramid geometry
    std::vector<Mesh::Vertex> vertices;
    std::vector<UINT> indices;
    ShapeGenerator::CreatePyramid(vertices, indices);

    // Initialize the mesh
    if (!pyramidMesh->Initialize(vertices, indices)) {
        MessageBox(hwnd, L"Failed to initialize pyramid mesh!", L"Error", MB_OK);
        return false;
    }

    // Set initial transformation if needed
    pyramidMesh->SetPosition(0.0f, 0.0f, -0.9f);
    pyramidMesh->SetScale(0.1f, 0.1f, 0.1f);

    icosphere = new Mesh(device, context);


    if (!icosphere->LoadFromOBJFile("fella.obj")) {
        MessageBox(hwnd, L"Failed to initialize icosphere mesh!", L"Error", MB_OK);
        return false;
    }

    icosphere->SetPosition(0.0f, -0.5f, -0.3f);
    icosphere->SetScale(0.8f, 0.8f, 0.8f);

    // Define the input layout
    D3D11_INPUT_ELEMENT_DESC layoutDesc[] = {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,   D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12,  D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "COLOR",    0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 24,  D3D11_INPUT_PER_VERTEX_DATA, 0 }
    };

    hr = device->CreateInputLayout(layoutDesc, ARRAYSIZE(layoutDesc), vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), &inputLayout);
    vsBlob->Release();
    psBlob->Release();
    if (FAILED(hr)) {
        MessageBox(hwnd, L"Failed to create input layout!", L"Error", MB_OK);
        return false;
    }

    context->IASetInputLayout(inputLayout);

    // Create the constant buffer
    D3D11_BUFFER_DESC cbd = {};
    cbd.Usage = D3D11_USAGE_DEFAULT;
    cbd.ByteWidth = sizeof(CBPerObject);
    cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

    hr = device->CreateBuffer(&cbd, nullptr, &cbPerObjectBuffer);
    if (FAILED(hr)) {
        MessageBox(hwnd, L"Failed to create constant buffer!", L"Error", MB_OK);
        return false;
    }

    // Set up the depth-stencil state
    D3D11_DEPTH_STENCIL_DESC dsDesc = {};
    dsDesc.DepthEnable = TRUE;
    dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
    dsDesc.DepthFunc = D3D11_COMPARISON_LESS;

    hr = device->CreateDepthStencilState(&dsDesc, &depthStencilState);
    if (FAILED(hr)) {
        MessageBox(hwnd, L"Failed to create depth stencil state!", L"Error", MB_OK);
        return false;
    }

    context->OMSetDepthStencilState(depthStencilState, 0);

    return true;
}

void Graphics::SetViewMatrix(const DirectX::XMMATRIX& viewMatrix) {
    this->viewMatrix = viewMatrix;
}


void Graphics::Update(float deltaTime) {
    //// Update view and projection matrices
    //viewMatrix = XMMatrixLookAtLH(
    //    XMVectorSet(0.0f, 1.0f, -5.0f, 1.0f), // Camera position
    //    XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f),  // Focus point
    //    XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f)   // Up direction
    //);

    float aspectRatio = viewport.Width / viewport.Height;


    // Rotate the pyramid over time
    static float angle = 0.0f;
    float rotationSpeed = XM_PI / 4; // 90 degrees per second
    angle += deltaTime * rotationSpeed;
    pyramidMesh->SetRotation(angle, angle, 0.0f);

    // Update the pyramid mesh
    //pyramidMesh->Update(deltaTime);

    icosphere->SetRotation(0.0f, angle, 0.0f);

    icosphere->Update(deltaTime);
}


void Graphics::ClearScreen(float r, float g, float b, float a) {
    float color[4] = { r, g, b, a };
    context->ClearRenderTargetView(renderTargetView, color);
    context->ClearDepthStencilView(depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
}

void Graphics::Present() {
    swapChain->Present(1, 0); // 1 to enable VSync, 0 to disable
}

void Graphics::Draw() {
    // Clear the screen
    ClearScreen(0.0f, 0.1f, 0.2f, 1.0f);

    // Set the input layout
    context->IASetInputLayout(inputLayout);

    // Set shaders
    context->VSSetShader(vertexShader, nullptr, 0);
    context->PSSetShader(pixelShader, nullptr, 0);

    // Calculate view-projection matrix
    XMMATRIX viewProjMatrix = Window::player->GetViewMatrix() * projMatrix;

    // Define the light direction movement parameters
    float lightSpeed = 0.1f;
    static float lightAngle = 0.0f;
    lightAngle += lightSpeed;

    // Calculate the moving light direction
    XMFLOAT4 lightDirection;
    lightDirection.x = 0.0f;
    lightDirection.y = 0.0f;
    lightDirection.z = 0.0f;
    lightDirection.w = 1.0f;
    // None of this lightDirection stuff works :)))

    // Draw the pyramid mesh
    pyramidMesh->Draw(viewProjMatrix, lightDirection);
    icosphere->Draw(viewProjMatrix, lightDirection);

    // Present the frame
    Present();
}
