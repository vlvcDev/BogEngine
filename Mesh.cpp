// Mesh.cpp

#include "Mesh.h"
#include <d3dcompiler.h>
#include <DirectXMath.h>
using namespace DirectX;

#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <iostream>

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

void Mesh::Draw(const DirectX::XMMATRIX& viewProjMatrix, const DirectX::XMFLOAT4& lightDirection) {
    // Bind the vertex buffer
    UINT stride = sizeof(Vertex);
    UINT offset = 0;
    context->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);

    // Bind the index buffer
    context->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);

    // Set the primitive topology
    context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    XMMATRIX normalMatrix = XMMatrixInverse(nullptr, worldMatrix);
    normalMatrix = XMMatrixTranspose(normalMatrix);

    // Update constant buffer
    CBPerObject cb{};
    cb.worldViewProj = XMMatrixTranspose(worldMatrix * viewProjMatrix);
    cb.world = XMMatrixTranspose(worldMatrix);
    cb.normalMatrix = normalMatrix;  // Pass the normal matrix to the shader
    cb.lightDirection = lightDirection;


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

    std::vector<XMFLOAT3> tempVertices;  // Vertex positions
    std::vector<UINT> tempIndices;       // Indices
    std::vector<XMFLOAT3> normals;       // Normals per vertex

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

    // Initialize normals array
    normals.resize(tempVertices.size(), XMFLOAT3(0.0f, 0.0f, 0.0f));

    // Compute normals
    for (size_t i = 0; i < tempIndices.size(); i += 3) {
        UINT index0 = tempIndices[i];
        UINT index1 = tempIndices[i + 1];
        UINT index2 = tempIndices[i + 2];

        XMFLOAT3& p0 = tempVertices[index0];
        XMFLOAT3& p1 = tempVertices[index1];
        XMFLOAT3& p2 = tempVertices[index2];

        // Compute face normal
        XMFLOAT3 faceNormal = CalculateNormal(p0, p1, p2);

        // Add the face normal to each vertex normal
        normals[index0].x += faceNormal.x;
        normals[index0].y += faceNormal.y;
        normals[index0].z += faceNormal.z;

        normals[index1].x += faceNormal.x;
        normals[index1].y += faceNormal.y;
        normals[index1].z += faceNormal.z;

        normals[index2].x += faceNormal.x;
        normals[index2].y += faceNormal.y;
        normals[index2].z += faceNormal.z;
    }

    // Normalize the normals
    for (size_t i = 0; i < normals.size(); ++i) {
        XMVECTOR n = XMLoadFloat3(&normals[i]);
        n = XMVector3Normalize(n);
        XMStoreFloat3(&normals[i], n);
    }

    // Create the vertices with positions, normals, and colors
    std::vector<Vertex> vertices;
    for (size_t i = 0; i < tempVertices.size(); ++i) {
        Vertex vertex;
        vertex.x = tempVertices[i].x;
        vertex.y = tempVertices[i].y;
        vertex.z = tempVertices[i].z;

        vertex.nx = normals[i].x;
        vertex.ny = normals[i].y;
        vertex.nz = normals[i].z;

        // Set default color (white)
        vertex.r = 0.7f;
        vertex.g = 0.7f;
        vertex.b = 0.7f;

        vertices.push_back(vertex);
    }

    return Initialize(vertices, tempIndices);
}


XMFLOAT3 Mesh::CalculateNormal(XMFLOAT3 p0, XMFLOAT3 p1, XMFLOAT3 p2) {
    XMVECTOR v0 = XMLoadFloat3(&p0);
    XMVECTOR v1 = XMLoadFloat3(&p1);
    XMVECTOR v2 = XMLoadFloat3(&p2);

    XMVECTOR edge1 = XMVectorSubtract(v1, v0);
    XMVECTOR edge2 = XMVectorSubtract(v2, v0);
    XMVECTOR normal = XMVector3Cross(edge1, edge2);
    normal = XMVector3Normalize(normal);

    XMFLOAT3 n;
    XMStoreFloat3(&n, normal);
    return n;
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
