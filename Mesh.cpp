// Mesh.cpp

#include "Mesh.h"
#include <d3dcompiler.h>
#include <DirectXMath.h>
using namespace DirectX;

#include <fstream>
#include <sstream>
#include <string>
#include <vector>

Mesh::Mesh(ID3D11Device* device, ID3D11DeviceContext* context)
    : device(device), context(context),
    vertexBuffer(nullptr), indexBuffer(nullptr), constantBuffer(nullptr),
    posX(0.0f), posY(0.0f), posZ(0.0f),
    rotX(0.0f), rotY(0.0f), rotZ(0.0f),
    scaleX(1.0f), scaleY(1.0f), scaleZ(1.0f),
    indexCount(0)
{
}

Mesh::~Mesh() {
    if (constantBuffer) constantBuffer->Release();
    if (indexBuffer) indexBuffer->Release();
    if (vertexBuffer) vertexBuffer->Release();
}

bool Mesh::Initialize(const std::vector<Vertex>& vertices, const std::vector<UINT>& indices) {
    // Store the index count
    indexCount = static_cast<UINT>(indices.size());

    // Create the vertex buffer
    D3D11_BUFFER_DESC vbDesc = {};
    vbDesc.Usage = D3D11_USAGE_DEFAULT;
    vbDesc.ByteWidth = sizeof(Vertex) * static_cast<UINT>(vertices.size());
    vbDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

    D3D11_SUBRESOURCE_DATA vbData = {};
    vbData.pSysMem = vertices.data();

    HRESULT hr = device->CreateBuffer(&vbDesc, &vbData, &vertexBuffer);
    if (FAILED(hr)) {
        // Handle error
        return false;
    }

    // Create the index buffer
    D3D11_BUFFER_DESC ibDesc = {};
    ibDesc.Usage = D3D11_USAGE_DEFAULT;
    ibDesc.ByteWidth = sizeof(UINT) * indexCount;
    ibDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;

    D3D11_SUBRESOURCE_DATA ibData = {};
    ibData.pSysMem = indices.data();

    hr = device->CreateBuffer(&ibDesc, &ibData, &indexBuffer);
    if (FAILED(hr)) {
        // Handle error
        return false;
    }

    // Create the constant buffer
    D3D11_BUFFER_DESC cbDesc = {};
    cbDesc.Usage = D3D11_USAGE_DEFAULT;
    cbDesc.ByteWidth = sizeof(CBPerObject);
    cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

    hr = device->CreateBuffer(&cbDesc, nullptr, &constantBuffer);
    if (FAILED(hr)) {
        // Handle error
        return false;
    }

    return true;
}

void Mesh::Update(float deltaTime) {
    // Update world matrix
    rotationMatrix = XMMatrixRotationRollPitchYaw(rotX, rotY, rotZ);
    scaleMatrix = XMMatrixScaling(scaleX, scaleY, scaleZ);
    translationMatrix = XMMatrixTranslation(posX, posY, posZ);

    worldMatrix = scaleMatrix * rotationMatrix * translationMatrix;
}

void Mesh::Draw(const DirectX::XMMATRIX& viewProjMatrix) {
    // Bind the vertex buffer
    UINT stride = sizeof(Vertex);
    UINT offset = 0;
    context->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);

    // Bind the index buffer
    context->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);

    // Set the primitive topology
    context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    // Update constant buffer
    CBPerObject cb{};
    cb.worldViewProj = XMMatrixTranspose(worldMatrix * viewProjMatrix);
    cb.world = XMMatrixTranspose(worldMatrix);
    context->UpdateSubresource(constantBuffer, 0, NULL, &cb, 0, 0);

    // Bind the constant buffer
    context->VSSetConstantBuffers(0, 1, &constantBuffer);

    // Draw the indexed vertices
    context->DrawIndexed(indexCount, 0, 0);
}

bool Mesh::LoadFromOBJFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        return false;
    }

    std::vector<XMFLOAT3> tempVertices;  // Temporary vertex array to store positions
    std::vector<UINT> tempIndices;       // Temporary index array

    std::string line;
    while (std::getline(file, line)) {
        std::istringstream s(line);
        std::string prefix;
        s >> prefix;

        if (prefix == "v") {
            // Parse vertex positions
            XMFLOAT3 vertex{};
            s >> vertex.x >> vertex.y >> vertex.z;
            tempVertices.push_back(vertex);
        }
        else if (prefix == "f") {
            // Parse face indices (supports various formats)
            std::string vertexStr;
            std::vector<UINT> faceIndices;

            while (s >> vertexStr) {
                std::istringstream vertexData(vertexStr);
                std::string indexStr;
                std::getline(vertexData, indexStr, '/');
                UINT index = static_cast<UINT>(std::stoi(indexStr));
                faceIndices.push_back(index - 1);  // Convert to 0-based index
            }

            // Triangulate face if necessary
            for (size_t i = 1; i + 1 < faceIndices.size(); ++i) {
                tempIndices.push_back(faceIndices[0]);
                tempIndices.push_back(faceIndices[i]);
                tempIndices.push_back(faceIndices[i + 1]);
            }
        }
    }

    // Convert to vertices including default color
    std::vector<Vertex> vertices;
    for (const auto& v : tempVertices) {
        Vertex vertex = { v.x, v.y, v.z, 1.0f, 1.0f, 1.0f };  // Default color: white
        vertices.push_back(vertex);
    }

    return Initialize(vertices, tempIndices);
}


void Mesh::SetPosition(float x, float y, float z) {
    posX = x;
    posY = y;
    posZ = z;
}

void Mesh::SetRotation(float pitch, float yaw, float roll) {
    rotX = pitch;
    rotY = yaw;
    rotZ = roll;
}

void Mesh::SetScale(float x, float y, float z) {
    scaleX = x;
    scaleY = y;
    scaleZ = z;
}
