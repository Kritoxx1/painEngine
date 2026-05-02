// Input code was heavily inspired from the Hell2025 engine by Chris Burrows (@livinamuk)
#pragma once

namespace Pain {
  namespace Input {
    void Init(void* glfwWinodw);
    void Update();
    void ClearKeyStates();
    bool KeyPressed(unsigned int keycode);
    bool KeyDown(unsigned int keycode);
    float GetMouseOffsetX();
    float GetMouseOffsetY();
    bool LeftMouseDown();
    bool RightMouseDown();
    bool MiddleMouseDown();
    bool LeftMousePressed();
    bool MiddleMousePressed();
    bool RightMousePressed();
    bool MouseWheelUp();
    bool MouseWheelDown();
    int GetMouseX();
    int GetMouseY();
    void PreventRightMouseHold();
    void DisableCursor();
    void HideCursor();
    void ShowCursor();
    int GetCursorScreenX();
    int GetCursorScreenY();
    void SetCursorPosition(int x, int y);
    int GetMouseXPreviousFrame();
    int GetMouseYPreviousFrame();
  }
}