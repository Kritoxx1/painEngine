#pragma once
#include <vulkan/vulkan.h>

#define ensure(result, msg) if ((result) != VK_SUCCESS) throw std::runtime_error((msg));