#include "pain_app.h"

#include <GLFW/glfw3.h>

namespace Pain {

void TestApp::run() {
  // return; // Skip loop for now.
  while (!painWindow.shouldClose()) {
    glfwPollEvents();
    painWindow.inputs();
  } 
}

}
