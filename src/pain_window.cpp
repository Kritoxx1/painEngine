#include "pain_window.h"
#include "pain_input.h"
#include "pain_debug.h"
#include <GLFW/glfw3.h>
#include <iostream>
#include <stdexcept>

namespace Pain {

PainWindow::PainWindow(int w, int h, const char* n) 
                    : width(w), height(h), title(n) 
{
  init();
  Input::Init(m_pWindow);
}

void PainWindow::init() {
  glfwInit();
  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
  glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

  m_pWindow = glfwCreateWindow(width, height, title, nullptr, nullptr);
}

void PainWindow::inputs() {
  if (Input::KeyPressed(GLFW_KEY_Q)) {
    glfwSetWindowShouldClose(m_pWindow, GLFW_TRUE);
  }
}

void PainWindow::createWindowSurface(VkInstance instance, VkSurfaceKHR* surface) {
  VkResult result;

  result = glfwCreateWindowSurface(instance, m_pWindow, nullptr, surface);
  ensure(result, "Failed to create Window surface");
}

PainWindow::~PainWindow() {
  glfwDestroyWindow(m_pWindow);
  glfwTerminate();
}

} // pain
