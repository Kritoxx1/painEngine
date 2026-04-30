#pragma once

#include "pain_window.h"
#include "pain_pipeline.h"
#include "pain_device.h"

namespace Pain {
  class TestApp {
    public:
      static constexpr int WIDTH = 800;
      static constexpr int HEIGHT = 600;

      void run();

    private:
      PainWindow painWindow{WIDTH, HEIGHT, "Vulkan"};
      PainPipeline painPipeline{"../src/shaders/default.vert.spv", "../src/shaders/default.frag.spv"};
      PainDevice painDevice{painWindow};
  };
}
