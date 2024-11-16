#pragma once

struct InputState
{
    // Movement keys
    bool moveForward;
    bool moveBackward;
    bool moveLeft;
    bool moveRight;

    // Mouse movement
    float deltaX;
    float deltaY;

    InputState()
        : moveForward(false), moveBackward(false), moveLeft(false), moveRight(false),
        deltaX(0.0f), deltaY(0.0f)
    {
    }
};

class InputManager
{
public:
    void Update(); // Update the input state each frame

    InputState GetInputState() const;

    // Callbacks for input events
    void OnKeyDown(int key);
    void OnKeyUp(int key);
    void OnMouseMove(float deltaX, float deltaY);

private:
    InputState inputState;
};
