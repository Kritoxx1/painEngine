#include "pain_window.h"
#include <GLFW/glfw3.h>
#include <iostream>
#include <stdexcept>
#include "pain_debug.h"

namespace Pain {

PainWindow::PainWindow(int w, int h, const char* n) 
                    : width(w), height(h), title(n) 
{
  init();
}

void PainWindow::init() {
  glfwInit();
  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
  glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

  m_pWindow = glfwCreateWindow(width, height, title, nullptr, nullptr);
}

void PainWindow::inputs() {
  if (glfwGetKey(m_pWindow, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
    glfwSetWindowShouldClose(m_pWindow, GLFW_TRUE);
  }
}

void PainWindow::createWindowSurface(VkInstance instance, VkSurfaceKHR* surface) {
  VkResult result;

  result = glfwCreateWindowSurface(instance, m_pWindow, nullptr, surface);
  ensure(result, "Failed to create Window surface");
  std::cout << "Succesfully created Window Surface!\n";
}

PainWindow::~PainWindow() {
  glfwDestroyWindow(m_pWindow);
  glfwTerminate();
}

} // pain
