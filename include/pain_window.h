#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

namespace Pain {
  class PainWindow {
    public:
      PainWindow(int w, int h, const char* n);

      PainWindow(const PainWindow &) = delete;
      PainWindow &operator=(const PainWindow &) = delete;

      const int getWidth() {return width;}
      const int getHeight() {return height;}

      bool shouldClose() { return glfwWindowShouldClose(this->m_Window); }
      void inputs();

      GLFWwindow* m_Window = nullptr;
      ~PainWindow();
    private:
      void init();

      const int width;
      const int height;
      const char* title;
  };
}
