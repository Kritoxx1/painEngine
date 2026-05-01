#include "pain_pipeline.h"
#include "pain_device.h"
#include "pain_debug.h"
#include <fstream>
#include <stdexcept>
#include <iostream>
#include <filesystem>
#include <cassert>

namespace Pain {
PainPipeline::PainPipeline(PainDevice& device, const char* vertPath, const char* fragPath, const PipeLineConfigInfo& conf)
  : m_PainDevice(device)
{
  createGraphicsPipeline(vertPath, fragPath, conf);
}

std::vector<char> PainPipeline::readFile(const char* filePath) {
  std::cout << "Trying to open: " << filePath << std::endl;
  if (!std::filesystem::exists(filePath)) {
    std::cerr << "Failed to find: " << filePath << ". Did you perhaps forget to run compile.sh?\n";
    throw std::runtime_error("ERROR, Look above");
  } else {
    std::cout << "Found file\n";
  }
  std::ifstream file(filePath, std::ios::ate | std::ios::binary);

  if (!file.is_open()) {
    throw std::runtime_error("Failed to open File");
  }

  size_t fileSize = static_cast<size_t>(file.tellg());
  std::vector<char> buffer(fileSize);

  file.seekg(0);
  file.read(buffer.data(), fileSize);

  file.close();
  
  return buffer;
}

void PainPipeline::createGraphicsPipeline(const char* vertPath, const char* fragPath, const PipeLineConfigInfo& conf)
{
  VkResult result;

  pain_assert(conf.pipelineLayout != VK_NULL_HANDLE, "Cannot create graphics pipeline:: no pipelineLayout provided in conf");
  pain_assert(conf.renderPass != VK_NULL_HANDLE, "Cannot create graphics pipeline:: no renderPass provided in conf");

  auto vertCode = readFile(vertPath);
  auto fragCode = readFile(fragPath);

  createShaderModule(vertCode, &m_vShaderModule);
  createShaderModule(fragCode, &m_fShaderModule);

  VkPipelineShaderStageCreateInfo shaderStage[2];
  shaderStage[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
  shaderStage[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
  shaderStage[0].module = m_vShaderModule;
  shaderStage[0].pName = "main";
  shaderStage[0].flags = 0;
  shaderStage[0].pNext = nullptr;
  shaderStage[0].pSpecializationInfo = nullptr;

  shaderStage[1].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
  shaderStage[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
  shaderStage[1].module = m_fShaderModule;
  shaderStage[1].pName = "main";
  shaderStage[1].flags = 0;
  shaderStage[1].pNext = nullptr;
  shaderStage[1].pSpecializationInfo = nullptr;

  VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
  vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
  vertexInputInfo.vertexAttributeDescriptionCount = 0;
  vertexInputInfo.vertexBindingDescriptionCount = 0;
  vertexInputInfo.pVertexAttributeDescriptions = nullptr;
  vertexInputInfo.pVertexBindingDescriptions = nullptr;

  VkPipelineViewportStateCreateInfo viewportInfo{};
  viewportInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
  viewportInfo.viewportCount = 1;
  viewportInfo.pViewports = &conf.viewport;
  viewportInfo.scissorCount = 1;
  viewportInfo.pScissors = &conf.scissor;

  VkGraphicsPipelineCreateInfo pipelineInfo{};
  pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
  pipelineInfo.stageCount = 2;
  pipelineInfo.pStages = shaderStage;
  pipelineInfo.pVertexInputState = &vertexInputInfo;
  pipelineInfo.pInputAssemblyState = &conf.inputAssemblyInfo;
  pipelineInfo.pViewportState = &viewportInfo;
  pipelineInfo.pRasterizationState = &conf.rasterizationInfo;
  pipelineInfo.pMultisampleState = &conf.multisampleInfo;
  pipelineInfo.pColorBlendState = &conf.colorBlendInfo;
  pipelineInfo.pDepthStencilState = &conf.depthStencilInfo;
  pipelineInfo.pDynamicState = nullptr;

  pipelineInfo.layout = conf.pipelineLayout;
  pipelineInfo.renderPass = conf.renderPass;
  pipelineInfo.subpass = conf.subpass;

  pipelineInfo.basePipelineIndex = -1;
  pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;

  result = vkCreateGraphicsPipelines(m_PainDevice.device(), VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &m_GraphicsPipeline);
  ensure(result, "Failed to create graphics pipeline!");
}
void PainPipeline::createShaderModule(const std::vector<char>& code, VkShaderModule* shaderModule) {
  VkResult result;

  VkShaderModuleCreateInfo shaderModuleCreateInfo{};
  shaderModuleCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
  shaderModuleCreateInfo.codeSize = code.size();
  shaderModuleCreateInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

  result = vkCreateShaderModule(m_PainDevice.device(), &shaderModuleCreateInfo, nullptr, shaderModule);
  ensure(result, "Failed to create shader module!");

  std::cout << "Succesfully created shader module!" << std::endl;
}

void PainPipeline::bind(VkCommandBuffer cmdBuff)
{
  vkCmdBindPipeline(cmdBuff, VK_PIPELINE_BIND_POINT_GRAPHICS, m_GraphicsPipeline);
}


PipeLineConfigInfo PainPipeline::defaultPipelineConfigInfo(uint32_t w, uint32_t h) {
  PipeLineConfigInfo configInfo{};

  configInfo.inputAssemblyInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
  configInfo.inputAssemblyInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
  configInfo.inputAssemblyInfo.primitiveRestartEnable = VK_FALSE;

  configInfo.viewport = {0.0f, 0.0f};
  configInfo.viewport.width = static_cast<float>(w);
  configInfo.viewport.height = static_cast<float>(h);
  configInfo.viewport.minDepth = 0.0f;
  configInfo.viewport.maxDepth = 1.0f;

  configInfo.scissor.offset = {0, 0};
  configInfo.scissor.extent = {w, h};


  configInfo.rasterizationInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
  configInfo.rasterizationInfo.depthClampEnable = VK_FALSE;
  configInfo.rasterizationInfo.rasterizerDiscardEnable = VK_FALSE;
  configInfo.rasterizationInfo.polygonMode = VK_POLYGON_MODE_FILL;
  configInfo.rasterizationInfo.lineWidth = 1.0f;
  configInfo.rasterizationInfo.cullMode = VK_CULL_MODE_NONE;
  configInfo.rasterizationInfo.frontFace = VK_FRONT_FACE_CLOCKWISE;
  configInfo.rasterizationInfo.depthBiasEnable = VK_FALSE;
  configInfo.rasterizationInfo.depthBiasConstantFactor = 0.0f;
  configInfo.rasterizationInfo.depthBiasClamp = 0.0f;
  configInfo.rasterizationInfo.depthBiasSlopeFactor = 0.0f;

  configInfo.multisampleInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
  configInfo.multisampleInfo.sampleShadingEnable = VK_FALSE;
  configInfo.multisampleInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
  configInfo.multisampleInfo.minSampleShading = 1.0f;
  configInfo.multisampleInfo.pSampleMask = nullptr;
  configInfo.multisampleInfo.alphaToCoverageEnable = VK_FALSE;
  configInfo.multisampleInfo.alphaToOneEnable = VK_FALSE;

  configInfo.colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
  configInfo.colorBlendAttachment.blendEnable = VK_FALSE;
  configInfo.colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
  configInfo.colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
  configInfo.colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
  configInfo.colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
  configInfo.colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
  configInfo.colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;

  configInfo.colorBlendInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
  configInfo.colorBlendInfo.logicOpEnable = VK_FALSE;
  configInfo.colorBlendInfo.logicOp = VK_LOGIC_OP_COPY;
  configInfo.colorBlendInfo.attachmentCount = 1;
  configInfo.colorBlendInfo.pAttachments = &configInfo.colorBlendAttachment;
  configInfo.colorBlendInfo.blendConstants[0] = 0.0f;
  configInfo.colorBlendInfo.blendConstants[1] = 0.0f;
  configInfo.colorBlendInfo.blendConstants[2] = 0.0f;
  configInfo.colorBlendInfo.blendConstants[3] = 0.0f;

  configInfo.depthStencilInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
  configInfo.depthStencilInfo.depthTestEnable = VK_TRUE;
  configInfo.depthStencilInfo.depthWriteEnable = VK_TRUE;
  configInfo.depthStencilInfo.depthCompareOp = VK_COMPARE_OP_LESS;
  configInfo.depthStencilInfo.depthBoundsTestEnable = VK_FALSE;
  configInfo.depthStencilInfo.minDepthBounds = 0.0f;
  configInfo.depthStencilInfo.maxDepthBounds = 1.0f;
  configInfo.depthStencilInfo.stencilTestEnable = VK_FALSE;
  configInfo.depthStencilInfo.front = {};
  configInfo.depthStencilInfo.back = {};

  return configInfo;
}

PainPipeline::~PainPipeline() {
  vkDestroyShaderModule(m_PainDevice.device(), m_vShaderModule, nullptr);
  vkDestroyShaderModule(m_PainDevice.device(), m_fShaderModule, nullptr);
  vkDestroyPipeline(m_PainDevice.device(), m_GraphicsPipeline, nullptr);
}

}
