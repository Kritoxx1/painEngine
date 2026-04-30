#include "pain_app.h"

#include <iostream>
#include <cstdlib>
#include <vulkan/vulkan_core.h>

int main() {
  Pain::TestApp app{};

  try {
    app.run();
  } catch(std::exception& e) {
    std::cerr << e.what() << "\n";
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}