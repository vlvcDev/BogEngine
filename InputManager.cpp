#include "InputManager.h"

void InputManager::Update()
{
    // Reset mouse deltas after processing
    inputState.deltaX = 0.0f;
    inputState.deltaY = 0.0f;
}

InputState InputManager::GetInputState() const
{
    return inputState;
}

void InputManager::OnKeyDown(int key)
{
    // Map keys to actions
    switch (key)
    {
    case 'W':
        inputState.moveForward = true;
        break;
    case 'S':
        inputState.moveBackward = true;
        break;
    case 'A':
        inputState.moveLeft = true;
        break;
    case 'D':
        inputState.moveRight = true;
        break;
    default:
        break;
    }
}

void InputManager::OnKeyUp(int key)
{
    // Map keys to actions
    switch (key)
    {
    case 'W':
        inputState.moveForward = false;
        break;
    case 'S':
        inputState.moveBackward = false;
        break;
    case 'A':
        inputState.moveLeft = false;
        break;
    case 'D':
        inputState.moveRight = false;
        break;
    default:
        break;
    }
}

void InputManager::OnMouseMove(float deltaX, float deltaY)
{
    inputState.deltaX += deltaX;
    inputState.deltaY += deltaY;
}
