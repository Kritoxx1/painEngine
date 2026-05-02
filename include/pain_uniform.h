#pragma once

#include <gtc/type_ptr.hpp>

#include "pain_device.h"

namespace Pain {
class PainUniform {
public:
  PainUniform(PainDevice& device);
  ~PainUniform();

  PainUniform(const PainUniform&) = delete;
  PainUniform &operator=(const PainUniform) = delete;

  void create();
  void update();

private:
  class PainDevice& painDevice;

};
}
