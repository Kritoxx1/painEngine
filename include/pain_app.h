#pragma once

#include "pain_window.h"
#include "pain_pipeline.h"
#include "pain_device.h"
#include "pain_swapchain.h"

namespace Pain {
  class TestApp {
    public:
      static constexpr int WIDTH = 800;
      static constexpr int HEIGHT = 600;

      void run();

    private:
      PainWindow painWindow{WIDTH, HEIGHT, "Vulkan"};
      PainDevice painDevice{painWindow};
      PainSwapchain painSwapchain{painDevice, painWindow.getExtent()};
      PainPipeline painPipeline{painDevice, "../src/shaders/default.vert.spv", "../src/shaders/default.frag.spv", PainPipeline::defaultPipelineConfigInfo(WIDTH, HEIGHT)};
  };
}
