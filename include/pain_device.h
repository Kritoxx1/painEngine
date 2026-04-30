#pragma once

#include "pain_window.h"

#include <vector>
#include <iostream>
#include <optional>
#include <vulkan/vulkan_core.h>
#include <string>

namespace Pain {

  struct QueueInfo {
    std::optional<uint32_t> FamilyIndex;
    std::vector<float> Priorities;

    bool isComplete() { return FamilyIndex.has_value(); }
  };

  struct SwapchainSupportDetails {
    VkSurfaceCapabilitiesKHR capabilities;
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> presentMode;
  };

  // TODO create surface without glfw.
  // struct WindowParams {
  // #ifdef VK_USE_PLATFORM_WIN32_KHR
  //   HINSTANCE HInstance;
  //   HWND HWnd;
  // #elif defined VK_USE_PLATFORM_XLIB_KHR
  //   Display* Dpy;
  //   Window Window;
  // #elif defined VK_USE_PLATFORM_XCB_KHR
  //   xcb_connection_t* Connection;
  //   xcb_window_t window;
  // #endif
  // };

  class PainDevice {
    public:
      PainDevice(PainWindow& window);
      ~PainDevice();
    private:
      void createInstance();

      void pickPhysicalDevice();
      bool isDeviceGud(VkPhysicalDevice device);
      bool isDeviceSupportingDesiredExtensions(VkPhysicalDevice device);
      QueueInfo pickQueueFamily();

      void setupDebugMessenger();
      void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);

      void createLogicalDevice();
      bool checkDeviceExtensionSupport();

      void createCommandPool();
      void createSurface();

      void createSwapchain();
      SwapchainSupportDetails querySwapchainSupport();
      VkSurfaceFormatKHR chooseSwapSurfaceFormat();
      VkPresentModeKHR chooseSwapPresentMode();
      VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);
      VkSurfaceCapabilitiesKHR getSurfaceCapabilities();

      void createImageViews();

      // DBG
      static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
        VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
        VkDebugUtilsMessageTypeFlagsEXT messageType,
        const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
        void* UserData
      ) 
      {
        std::cerr << "Validation Layer: " << pCallbackData->pMessage << std::endl;
        return VK_FALSE;
      }

      VkResult CreateDebugUtilsMessengerEXT(
          VkInstance instance,
          const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
          const VkAllocationCallbacks* pAllocator,
          VkDebugUtilsMessengerEXT* pDebugMessenger
      );

      void DestroyDebugUtilsMessengerEXT(
          VkInstance instance,
          VkDebugUtilsMessengerEXT debugMessenger,
          const VkAllocationCallbacks* pAllocator
      );

// === Members ===
      PainWindow& m_Window;

      // DBG
      VkDebugUtilsMessengerEXT debugMessenger;

      // Important Vulkan Context members
      const char* m_Layers[1] = {"VK_LAYER_KHRONOS_validation"};
      std::vector<std::string> m_DeviceExtensions = {"VK_KHR_swapchain"};
      VkInstance m_Instance{};
      VkPhysicalDevice m_PhysicalDevice{}; // The device which vulkan is going to use.
      VkDevice m_Device{};
      VkQueue m_GraphicsQueue{};
      VkSurfaceKHR m_Surface{};
      VkSwapchainKHR m_Swapchain{};

      std::vector<VkImage> m_SwapchainImages;
      VkFormat m_SwapchainImageFormat;
      VkExtent2D m_SwapchainExtent;

      std::vector<VkImageView> m_ImageViews{};

      VkCommandPool m_CommandPool{};
  };
}
