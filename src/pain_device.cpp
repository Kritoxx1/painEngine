#include "pain_device.h"
#include "pain_debug.h"
#include <GLFW/glfw3.h>
#include <cstdint>
#include <limits>
#include <stdexcept>
#include <vector>
#include <vulkan/vulkan_core.h>
#include <iostream>
#include <cstring>
#include <algorithm>
#include <set>

namespace Pain {

PainDevice::PainDevice(PainWindow& window) : m_Window(window) {
  createInstance();
  setupDebugMessenger();
  createSurface();
  pickPhysicalDevice();
  createLogicalDevice();
  createCommandPool();

  // Keeping both in the PainDevice class in case any errors occur
  // createSwapchain();
  // createImageViews();
}

VkFormat PainDevice::findSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features)
{
  for (VkFormat format : candidates) {
    VkFormatProperties props;
    vkGetPhysicalDeviceFormatProperties(m_PhysicalDevice, format, &props);

    if (tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features) {
      return format;
    } else if (tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & features) == features) {
      return format;
    }
  }
  throw std::runtime_error("failed to find supported format!");
}

void PainDevice::createImageWithInfo(const VkImageCreateInfo &imageInfo, VkMemoryPropertyFlags properties, VkImage &image, VkDeviceMemory &imageMemory)
{
  if (vkCreateImage(m_Device, &imageInfo, nullptr, &image) != VK_SUCCESS) {
    throw std::runtime_error("failed to create image!");
  }

  VkMemoryRequirements memRequirements;
  vkGetImageMemoryRequirements(m_Device, image, &memRequirements);

  VkMemoryAllocateInfo allocInfo{};
  allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
  allocInfo.allocationSize = memRequirements.size;
  allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, properties);

  if (vkAllocateMemory(m_Device, &allocInfo, nullptr, &imageMemory) != VK_SUCCESS) {
    throw std::runtime_error("failed to allocate image memory!");
  }

  if (vkBindImageMemory(m_Device, image, imageMemory, 0) != VK_SUCCESS) {
    throw std::runtime_error("failed to bind image memory!");
  }
}

uint32_t PainDevice::findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties) {
  VkPhysicalDeviceMemoryProperties memProperties;
  vkGetPhysicalDeviceMemoryProperties(m_PhysicalDevice, &memProperties);
  for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
    if ((typeFilter & (1 << i)) &&
      (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
      return i;
    }
  }

  throw std::runtime_error("failed to find suitable memory type!");
}

void PainDevice::createInstance() {
  VkResult result;
  VkApplicationInfo appCreateInfo;
  appCreateInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
  appCreateInfo.pNext = nullptr;
  appCreateInfo.pApplicationName = "Pain";
  appCreateInfo.applicationVersion = VK_MAKE_VERSION(0, 0, 1);
  appCreateInfo.pEngineName = "Pain Engine";
  appCreateInfo.engineVersion = VK_MAKE_VERSION(0, 0, 1);
  appCreateInfo.apiVersion = VK_API_VERSION_1_0;

  VkInstanceCreateInfo instanceCreateInfo;
  instanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
  instanceCreateInfo.pNext = nullptr;
  instanceCreateInfo.flags = 0;
  instanceCreateInfo.pApplicationInfo = &appCreateInfo;
  instanceCreateInfo.enabledLayerCount = 0;
  instanceCreateInfo.ppEnabledLayerNames = nullptr;
  instanceCreateInfo.enabledExtensionCount = 0;
  instanceCreateInfo.ppEnabledExtensionNames = nullptr;

  // needed extensions from glfw
  uint32_t glfwExtensionsCount = 0;
  const char** glfwExtensions;
  glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionsCount);

  // dbug print
  std::cout << "Required GLFW Extensions: " << glfwExtensions << std::endl;

  std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionsCount);
  extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

  if (glfwExtensionsCount) {
    instanceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
    instanceCreateInfo.ppEnabledExtensionNames = extensions.data();
    std::cout << "Registered extension\n";
  }
  
  uint32_t propertyCount = 0;
  vkEnumerateInstanceLayerProperties(&propertyCount, nullptr);
 
  // Enabling validation Layers
  // TODO make it optional (Release/Debug)
  uint32_t layerCount = 0;
  vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

  VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
  if(layerCount) {
    instanceCreateInfo.enabledLayerCount = (uint32_t)1;
    instanceCreateInfo.ppEnabledLayerNames = m_Layers;   

    populateDebugMessengerCreateInfo(debugCreateInfo);
    instanceCreateInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*) &debugCreateInfo;
    std::cout << "Registered layer\n";
  }
  
  // TODO Check if the extension is supported by our Vulkan drivers
  
  result = vkCreateInstance(&instanceCreateInfo, nullptr, &m_Instance);
  ensure(result, "Failed to create Instance");

  std::cout << "Succesfully created instance!\n";
}

void PainDevice::populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo) {
  createInfo = {}; // init with null
  createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
  createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
  createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
  createInfo.pfnUserCallback = debugCallback;
}


void PainDevice::setupDebugMessenger() {
  VkDebugUtilsMessengerCreateInfoEXT dbgMessengerCreateInfo = {
    .sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
    .pNext = nullptr,
    .flags = 0,
    .messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT,
    .messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT,
    .pfnUserCallback = debugCallback,
    .pUserData = nullptr
  };

  if (CreateDebugUtilsMessengerEXT(m_Instance, &dbgMessengerCreateInfo, nullptr, &debugMessenger) != VK_SUCCESS) {
    throw std::runtime_error("Failed to setup debug Messenger!");
  }
}

VkResult PainDevice::CreateDebugUtilsMessengerEXT(
    VkInstance instance,
    const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
    const VkAllocationCallbacks* pAllocator,
    VkDebugUtilsMessengerEXT* pDebugMessenger)
{
  auto func = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
  if (func != nullptr) {
    return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
  } else {
    return VK_ERROR_EXTENSION_NOT_PRESENT;
  }
}

void PainDevice::DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator) {
  auto func = (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
  if (func != nullptr) {
    func(instance, debugMessenger, pAllocator);
  }
}

void PainDevice::createSurface() {
  m_Window.createWindowSurface(m_Instance, &m_Surface);
}

void PainDevice::pickPhysicalDevice() {
  VkResult result;
  uint32_t deviceCount = 0;
  result = vkEnumeratePhysicalDevices(m_Instance, &deviceCount, nullptr);
  ensure(result, "Failed to get Device count");
  
  if (!deviceCount) {
    throw std::runtime_error("There are no devices that support vulkan!");
  }

  std::cout << "Found " << deviceCount << " device(s)\n";

  std::vector<VkPhysicalDevice> availableDevices(deviceCount);
  result = vkEnumeratePhysicalDevices(m_Instance, &deviceCount, availableDevices.data());
  ensure(result, "Failed to Enumerate Devices!");

  VkPhysicalDeviceProperties deviceProps{};
  VkPhysicalDeviceMemoryProperties deviceMemProps{};
  for (uint32_t i = 0; i < deviceCount; i++)  {
    vkGetPhysicalDeviceProperties(availableDevices[i], &deviceProps);
    vkGetPhysicalDeviceMemoryProperties(availableDevices[i], &deviceMemProps);

    std::cout << "Found Device: " << deviceProps.deviceName << "\n";
    std::cout << "Type: " << deviceProps.deviceType << "\n";
    std::cout << "ID: " << deviceProps.deviceID << "\n";
    std::cout << "Memory Type Count: " << deviceMemProps.memoryTypeCount << "\n";
    std::cout << "Memory Heap Count: " << deviceMemProps.memoryHeapCount << "\n";

    if (isDeviceGud(availableDevices[i])) {
      m_PhysicalDevice = availableDevices[i];
      break;
    }
  }
}

bool PainDevice::isDeviceGud(VkPhysicalDevice device) {
  VkResult result{};
  VkPhysicalDeviceProperties deviceProps{};
  vkGetPhysicalDeviceProperties(device, &deviceProps);
  if (deviceProps.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU && isDeviceSupportingDesiredExtensions(device)) {
    return true; // device is very gud
  }

  std::cout << "The device " << deviceProps.deviceName << " is not gud! ;[ \n";
  return false;
}

bool PainDevice::isDeviceSupportingDesiredExtensions(VkPhysicalDevice device) {
  VkResult result;
 
  uint32_t propertyCount = 0;
  result = vkEnumerateDeviceExtensionProperties(device, nullptr, &propertyCount, nullptr);
  ensure(result, "Failed to enumerate Properties count!");
  std::cout << "Extensions found: " << propertyCount << "\n";

  std::vector<VkExtensionProperties> extProperties(propertyCount);
  result = vkEnumerateDeviceExtensionProperties(device, nullptr, &propertyCount, extProperties.data());
  ensure(result, "Failed to enumerate Properties!");

  uint32_t requiredCount = static_cast<uint32_t>(m_DeviceExtensions.size());
  for (uint32_t i = 0; i < requiredCount; i++) {
    const char* required = m_DeviceExtensions[i].c_str();
    bool found = false;
 
    for (uint32_t j = 0; j < propertyCount; j++) {
      if (strcmp(extProperties[j].extensionName, required) == 0) {
        found = true;
        break;
      }  
    }
  
    if (!found) {
      std::cout << "Missing extension: " << required << "\n";
      return false;
    }
  }

  std::cout << "All desired device extensions are supported!\n";
  return true;
}

void PainDevice::createLogicalDevice() {
  VkResult result;

  QueueInfo indices = pickQueueFamily(m_PhysicalDevice);

  std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
  std::set<uint32_t> uniqueQueueFamilies = {indices.graphicsFamily, indices.presentFamily};

  float prio = 1.0f;
  for (uint32_t queueFamily : uniqueQueueFamilies)
  {
    VkDeviceQueueCreateInfo queueCreateInfo{};
    queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queueCreateInfo.pNext = nullptr;
    queueCreateInfo.flags = 0;
    queueCreateInfo.queueFamilyIndex = queueFamily;
    queueCreateInfo.queueCount = 1;
    queueCreateInfo.pQueuePriorities = &prio;
    queueCreateInfos.push_back(queueCreateInfo);
  }

  const char* ext = m_DeviceExtensions.data()->c_str();
  VkDeviceCreateInfo deviceCreateInfo = {
    .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
    .pNext = nullptr,
    .flags = 0,
    .queueCreateInfoCount = 1,
    .pQueueCreateInfos = nullptr,
    .enabledLayerCount = 1,
    .ppEnabledLayerNames = m_Layers,
    .enabledExtensionCount = 1,
    .ppEnabledExtensionNames = &ext,
    .pEnabledFeatures = nullptr
  };

  deviceCreateInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
  deviceCreateInfo.pQueueCreateInfos = queueCreateInfos.data();

  result = vkCreateDevice(m_PhysicalDevice, &deviceCreateInfo, nullptr, &m_Device);
  ensure(result, "Failed to create Logical Device!");
  std::cout << "Created Logical Device!\n";

  vkGetDeviceQueue(m_Device, indices.graphicsFamily, 0, &m_GraphicsQueue);
  vkGetDeviceQueue(m_Device, indices.presentFamily, 0, &m_PresentQueue);
  std::cout << "Queued device Queue!\n";
}

QueueInfo PainDevice::pickQueueFamily(VkPhysicalDevice device) {
  VkResult result;
  QueueInfo indices;

  uint32_t queueFamilyCount = 0;
  vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

  std::vector<VkQueueFamilyProperties> queueFamilyProps(queueFamilyCount);
  vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilyProps.data());

  int i = 0;
  for (const VkQueueFamilyProperties& queueFamily : queueFamilyProps) {
    if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
      indices.graphicsFamily = i;
    }

    VkBool32 presentSupport = false;
    vkGetPhysicalDeviceSurfaceSupportKHR(device, i, m_Surface, &presentSupport);
    if (presentSupport) {
      indices.presentFamily = i;
    }

    if (indices.isComplete()) break;

    i++;
  }

  return indices;
}

void PainDevice::createCommandPool() {
  VkResult result;
  QueueInfo indices = pickQueueFamily(m_PhysicalDevice);

  VkCommandPoolCreateInfo poolInfo = {
    .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
    .pNext = nullptr,
    .flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT | VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
    .queueFamilyIndex = indices.graphicsFamily,
  };

  result = vkCreateCommandPool(m_Device, &poolInfo, nullptr, &m_CommandPool);
  ensure(result, "Failed to create Command Pool!");
  std::cout << "Succesfully created command pool!\n";
}

void PainDevice::createSwapchain() {
  VkResult result{};
  SwapchainSupportDetails supportDetails = querySwapchainSupport();

  VkSurfaceCapabilitiesKHR surfaceCapabilities = getSurfaceCapabilities();
  VkSurfaceFormatKHR surfaceFormats = chooseSwapSurfaceFormat();
  VkExtent2D extent = chooseSwapExtent(surfaceCapabilities);
  VkPresentModeKHR presentMode = chooseSwapPresentMode();

  VkSwapchainCreateInfoKHR swapchainCreateInfo{};
  swapchainCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
  swapchainCreateInfo.pNext = nullptr;
  swapchainCreateInfo.flags = 0;

  swapchainCreateInfo.surface = m_Surface;

  uint32_t imageCount = supportDetails.capabilities.minImageCount + 1;
  if (supportDetails.capabilities.maxImageCount > 0 && imageCount > supportDetails.capabilities.maxImageCount) {
       imageCount = supportDetails.capabilities.maxImageCount;
  }
  swapchainCreateInfo.minImageCount = imageCount;
  swapchainCreateInfo.imageFormat = surfaceFormats.format;
  swapchainCreateInfo.imageColorSpace = surfaceFormats.colorSpace;
  swapchainCreateInfo.imageExtent = extent;
  swapchainCreateInfo.imageArrayLayers = 1;
  swapchainCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

  swapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE; // page 134
  swapchainCreateInfo.queueFamilyIndexCount = 0;
  swapchainCreateInfo.pQueueFamilyIndices = nullptr;
  swapchainCreateInfo.preTransform = supportDetails.capabilities.currentTransform;
  swapchainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
  swapchainCreateInfo.presentMode = chooseSwapPresentMode();
  swapchainCreateInfo.clipped = VK_TRUE;
  swapchainCreateInfo.oldSwapchain = VK_NULL_HANDLE; // no old swapchain

  result = vkCreateSwapchainKHR(m_Device, &swapchainCreateInfo,  nullptr, &m_Swapchain);
  ensure(result, "Failed to create Swapchain"); 

  vkGetSwapchainImagesKHR(m_Device, m_Swapchain, &imageCount, nullptr);
  m_SwapchainImages.resize(imageCount);
  vkGetSwapchainImagesKHR(m_Device, m_Swapchain, &imageCount, m_SwapchainImages.data());

  m_SwapchainImageFormat = surfaceFormats.format;
  m_SwapchainExtent = extent;

  std::cout << "Swapchain successfully created!\n";
}

SwapchainSupportDetails PainDevice::querySwapchainSupport() {
  VkResult result{};
  SwapchainSupportDetails details{}; 

  result = vkGetPhysicalDeviceSurfaceCapabilitiesKHR(m_PhysicalDevice, m_Surface, &details.capabilities);
  ensure(result, "Failed to retrieve Surface capabilities!");

  uint32_t formatCount = 0;
  result = vkGetPhysicalDeviceSurfaceFormatsKHR(m_PhysicalDevice, m_Surface, &formatCount, nullptr);
  ensure(result, "Failed to retrieve Surface Formats!");
  
  if (formatCount != 0) {
    details.formats.resize(formatCount);
    vkGetPhysicalDeviceSurfaceFormatsKHR(m_PhysicalDevice, m_Surface, &formatCount, details.formats.data());
  }

  uint32_t presentModeCount = 0;
  vkGetPhysicalDeviceSurfacePresentModesKHR(m_PhysicalDevice, m_Surface, &presentModeCount, nullptr);
  ensure(result, "Failed to retrieve Present Mode count!");

  if (presentModeCount != 0) {
    details.presentMode.resize(presentModeCount);
    result = vkGetPhysicalDeviceSurfacePresentModesKHR(m_PhysicalDevice, m_Surface, &presentModeCount, details.presentMode.data());
    ensure(result, "Failed to retrieve Present Mode details");
  }

  return details;
}

VkSurfaceFormatKHR PainDevice::chooseSwapSurfaceFormat() {
  VkResult result{};
  VkSurfaceFormatKHR ret{};

  uint32_t formatCount = 0;
  result = vkGetPhysicalDeviceSurfaceFormatsKHR(m_PhysicalDevice, m_Surface, &formatCount, nullptr);
  ensure(result, "Failed to get Surface Formats count!");

  std::vector<VkSurfaceFormatKHR> availableFormats(formatCount);
  result = vkGetPhysicalDeviceSurfaceFormatsKHR(m_PhysicalDevice, m_Surface, &formatCount, availableFormats.data());
  ensure(result, "Failed to get Surface Formats!");

  for (const VkSurfaceFormatKHR& availableFormat : availableFormats) {
    if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
        return availableFormat;
    }
  }

  return availableFormats[0];
}

VkPresentModeKHR PainDevice::chooseSwapPresentMode() {
  VkResult result;

  uint32_t presentModeCount = 0;
  result = vkGetPhysicalDeviceSurfacePresentModesKHR(m_PhysicalDevice, m_Surface, &presentModeCount, nullptr);
  ensure(result, "Failed to get surface present modes count!");
  
  std::vector<VkPresentModeKHR> presentModes(presentModeCount);

  result = vkGetPhysicalDeviceSurfacePresentModesKHR(m_PhysicalDevice, m_Surface, &presentModeCount, presentModes.data());
  ensure(result, "Failed to get surface present modes!");

  for (const VkPresentModeKHR& presentMode : presentModes) {
    if (presentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
      std::cout << "Physical device supports desired Present Mode\n";
      return presentMode;
    }
  }

  return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D PainDevice::chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities) {
  VkResult result;
  VkExtent2D extent{};

  if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
    return capabilities.currentExtent;
  } else {
    int width, height;
    m_Window.getFramebufferSize(&width, &height);

    extent = {
      static_cast<uint32_t>(width),
      static_cast<uint32_t>(height)
    };

    extent.width = std::clamp(extent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
    extent.height = std::clamp(extent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);
    return extent;
  }
}

VkSurfaceCapabilitiesKHR PainDevice::getSurfaceCapabilities() {
  VkSurfaceCapabilitiesKHR surfaceCapabilities{};
  vkGetPhysicalDeviceSurfaceCapabilitiesKHR(m_PhysicalDevice, m_Surface, &surfaceCapabilities);

  return surfaceCapabilities;
}

// Page 72
void PainDevice::createImageViews() {
  VkResult result;

  m_ImageViews.resize(m_SwapchainImages.size());
  for (size_t i = 0; i < m_SwapchainImages.size(); i++) {
    VkImageViewCreateInfo ivCreateInfo;
    ivCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    ivCreateInfo.pNext = nullptr;
    ivCreateInfo.flags = 0;

    ivCreateInfo.image = m_SwapchainImages[i];
    ivCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    ivCreateInfo.format = m_SwapchainImageFormat;

    ivCreateInfo.components = {
      VK_COMPONENT_SWIZZLE_IDENTITY,
      VK_COMPONENT_SWIZZLE_IDENTITY,
      VK_COMPONENT_SWIZZLE_IDENTITY,
      VK_COMPONENT_SWIZZLE_IDENTITY
    };

    ivCreateInfo.subresourceRange = {
      .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
      .baseMipLevel = 0,
      .levelCount = 1,
      .baseArrayLayer = 0,
      .layerCount = 1,
    };

    result = vkCreateImageView(m_Device, &ivCreateInfo, nullptr, &m_ImageViews[i]);
    ensure(result, "Failed to create Image View!");
  }
  std::cout << "Succesfully creates Image View!\n";
}

void PainDevice::createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory) {
  VkResult result;

  VkBufferCreateInfo bufferInfo = {};
  bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
  bufferInfo.size = size;
  bufferInfo.usage = usage;
  bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

  result = vkCreateBuffer(m_Device, &bufferInfo, nullptr, &buffer);
  ensure(result, "failed to create vertex buffer");

  VkMemoryRequirements memRequirements;
  vkGetBufferMemoryRequirements(m_Device, buffer, &memRequirements);

  VkMemoryAllocateInfo allocInfo = {};
  allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
  allocInfo.allocationSize = memRequirements.size;
  allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, properties);

  result = vkAllocateMemory(m_Device, &allocInfo, nullptr, &bufferMemory);
  ensure(result, "Failed to allocate vbuffer Memory!");

  vkBindBufferMemory(m_Device, buffer, bufferMemory, 0);
}


PainDevice::~PainDevice() {

  for (VkImageView imageView : m_ImageViews) {
    vkDestroyImageView(m_Device, imageView, nullptr);
  }

  vkDestroySwapchainKHR(m_Device, m_Swapchain, nullptr);
  vkDestroyCommandPool(m_Device, m_CommandPool, nullptr);
  vkDestroyDevice(m_Device, nullptr);
  DestroyDebugUtilsMessengerEXT(m_Instance, debugMessenger, nullptr);
  vkDestroySurfaceKHR(m_Instance, m_Surface, nullptr);
  vkDestroyInstance(m_Instance, nullptr);
}

} // NAMESPACE PAIN
