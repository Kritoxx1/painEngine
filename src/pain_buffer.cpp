#include "pain_buffer.h"

namespace Pain {
PainUniform::PainUniform(PainDevice& device) : painDevice(device) {
  createDescriptorSet();
}

void PainUniform::createDescriptorSet() {

}

PainUniform::~PainUniform() {

}
}
