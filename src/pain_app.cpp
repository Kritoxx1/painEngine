#include "pain_app.h"
#include "pain_debug.h"
#include "pain_input.h"
#include <GLFW/glfw3.h>
#include <array>

namespace Pain {

TestApp::TestApp()
{
  loadModels();
  createPipelineLayout();
  createPipeline();
  createCommandBuffers();
}

  void TestApp::run() {
  // return; // Skip loop for now.
  while (!painWindow.shouldClose()) {
    glfwPollEvents();
    Input::Update();
    painWindow.inputs();
    draw();
  }

  vkDeviceWaitIdle(painDevice.device());
}

void TestApp::loadModels() {
  std::vector<PainModel::Vertex> vertices = {
    {{0.0f, -0.5f}},
    {{0.5f, 0.5f}},
    {{-0.5f, 0.5f}}
  };
  painModel = std::make_unique<PainModel>(painDevice, vertices);
}

  void TestApp::createPipelineLayout()
{
  VkResult result;
  VkPipelineLayoutCreateInfo pipelineLayoutInfo = {};
  pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
  pipelineLayoutInfo.setLayoutCount = 0;
  pipelineLayoutInfo.pSetLayouts = nullptr;
  pipelineLayoutInfo.pushConstantRangeCount = 0;
  pipelineLayoutInfo.pPushConstantRanges = nullptr;

  result = vkCreatePipelineLayout(painDevice.device(), &pipelineLayoutInfo, nullptr, &pipelineLayout);
  ensure(result, "failed to create pipeline layout");
}

void TestApp::createPipeline()
{
  auto pipelineConf = PainPipeline::defaultPipelineConfigInfo(painSwapchain.width(), painSwapchain.height());
  pipelineConf.colorBlendInfo.pAttachments = &pipelineConf.colorBlendAttachment;
  pipelineConf.renderPass = painSwapchain.getRenderPass();
  pipelineConf.pipelineLayout = pipelineLayout;
  painPipeline = std::make_unique<PainPipeline>(painDevice, "../bin/default.vert.spv", "../bin/default.frag.spv", pipelineConf);
}

void TestApp::createCommandBuffers()
{
  VkResult result;

  commandBuffers.resize(painSwapchain.imageCount());
  VkCommandBufferAllocateInfo allocInfo = {};
  allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
  allocInfo.commandPool = painDevice.getCommandPool();
  allocInfo.commandBufferCount = static_cast<uint32_t>(commandBuffers.size());

  result = vkAllocateCommandBuffers(painDevice.device(), &allocInfo, commandBuffers.data());
  ensure(result, "Failed to allocate command buffers!");

  for (int i = 0; i < commandBuffers.size(); i++)
  {
    VkCommandBufferBeginInfo beginInfo = {};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

    result = vkBeginCommandBuffer(commandBuffers[i], &beginInfo);
    ensure(result, "failed ot begin record commandbuffer");

    VkRenderPassBeginInfo renderPassInfo = {};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = painSwapchain.getRenderPass();
    renderPassInfo.framebuffer = painSwapchain.getFrameBuffer(i);

    renderPassInfo.renderArea.offset = {0,0};
    renderPassInfo.renderArea.extent = painSwapchain.getSwapChainExtent();

    std::array<VkClearValue, 2> clearValues{};
    clearValues[0].color = {0.1f, 0.1f, 0.1f, 1.0f};
    clearValues[1].depthStencil = {1.0f, 0};
    renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
    renderPassInfo.pClearValues = clearValues.data();

    vkCmdBeginRenderPass(commandBuffers[i], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

    painPipeline->bind(commandBuffers[i]);
    painModel->bind(commandBuffers[i]);
    painModel->draw(commandBuffers[i]);

    vkCmdEndRenderPass(commandBuffers[i]);

    result = vkEndCommandBuffer(commandBuffers[i]);
    ensure(result, "failed to record cmdbuffer!");
  }
}

void TestApp::draw()
{
  uint32_t imageIndex;
  auto result = painSwapchain.acquireNextImage(&imageIndex);

  if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
  {
    throw std::runtime_error("Failed to acquire swap chain image");
  }

  result = painSwapchain.submitCommandBuffers(&commandBuffers[imageIndex], &imageIndex);
  ensure(result, "Failed to submit command buffers!");
}

TestApp::~TestApp()
{
  vkDestroyPipelineLayout(painDevice.device(), pipelineLayout, nullptr);
}
}
