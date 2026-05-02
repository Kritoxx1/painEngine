#pragma once

#include "pain_window.h"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm.hpp>

namespace Pain {

class PainCamera {
public:
  PainCamera(PainWindow& window);
  ~PainCamera() = default;

  PainCamera(const PainCamera&) = delete;
  PainCamera &operator=(const PainCamera &) = delete;

  void update();

private:
  void updateUniformMatrix();

  glm::mat4 m_viewMatrix = glm::mat4(1.f);
  glm::mat4 m_projMatrix = glm::mat4(1.f);

  glm::vec3 m_position;
  glm::vec3 m_orientation = glm::vec3(0.0f, 0.0f, 1.0f);

  class PainWindow& painWindow;
};

}