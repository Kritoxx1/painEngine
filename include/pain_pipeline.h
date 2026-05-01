#pragma once

#include "pain_device.h"
#include <vector>

namespace Pain {

struct PipeLineConfigInfo
{
  VkViewport viewport;
  VkRect2D scissor;

  VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo;
  VkPipelineRasterizationStateCreateInfo rasterizationInfo;
  VkPipelineMultisampleStateCreateInfo multisampleInfo;
  VkPipelineColorBlendAttachmentState colorBlendAttachment;
  VkPipelineColorBlendStateCreateInfo colorBlendInfo;
  VkPipelineDepthStencilStateCreateInfo depthStencilInfo;

  VkPipelineLayout pipelineLayout = nullptr;
  VkRenderPass renderPass = nullptr;
  uint32_t subpass = 0;
};

class PainPipeline {
  public:
    PainPipeline(PainDevice& device, const char* vertPath, const char* fragPath, const PipeLineConfigInfo& conf);
    ~PainPipeline();

    PainPipeline(const PainPipeline&) = delete;
    void operator=(const PainPipeline&) = delete;

    static PipeLineConfigInfo defaultPipelineConfigInfo(uint32_t w, uint32_t h);

  private:
    static std::vector<char> readFile(const char* filePath);

    void createGraphicsPipeline(const char* vertPath, const char* fragPath, const PipeLineConfigInfo& conf);
    void createShaderModule(const std::vector<char>& code, VkShaderModule* shaderModule);

    PainDevice& m_PainDevice;
    VkPipeline m_GraphicsPipeline;
    VkShaderModule m_vShaderModule, m_fShaderModule;
};
} // pain
