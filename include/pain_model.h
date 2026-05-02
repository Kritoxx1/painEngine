#pragma once

#include  "pain_device.h"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE

#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <vector>

namespace Pain {
class PainModel {
  public:

    struct Vertex {
      glm::vec2 pos;

      static std::vector<VkVertexInputBindingDescription> getBindingDescription();
      static std::vector<VkVertexInputAttributeDescription> getAttributeDescription();
    };

    PainModel(PainDevice& device, const std::vector<Vertex> &vertices);
    ~PainModel();

    PainModel(const PainModel&) = delete;
    PainModel &operator=(const PainModel&) = delete;

    glm::mat4 getTransform() const { return m_transform; }
    void setTransformMatrix(const glm::vec3 val) { m_transform = glm::translate(glm::mat4(1.f), val); }
    void setTransform(const glm::mat4 val) { m_transform = val; }
    void setRotation(const glm::mat4& val) { m_rotation = val; };
    glm::mat4 getRotation() const { return m_rotation; }
    void setScale(const glm::vec3& val) { m_scale = glm::scale(glm::mat4(1.f), val); }
    glm::mat4 getScale() const { return m_scale; }
    void setModel(const glm::mat4& val) { m_model = val; }
    glm::mat4 getModel() const { return m_model; }
    void setPosition(const glm::vec3& val) { m_pos = val; }
    glm::vec3 getPosition() const { return m_pos; }

    void bind(VkCommandBuffer cmdBuff);
    void draw(VkCommandBuffer cmdBuff);

  private:
    void createVertexBuffers(const std::vector<Vertex> &vertices);

    PainDevice& m_PainDevice;
    VkBuffer vertexBuffer;
    VkDeviceMemory vertexBufferMemory;
    uint32_t vertexCount;

    glm::vec3 m_pos = glm::vec3(1.f);
    glm::mat4 m_model = glm::mat4(1.f);
    glm::mat4 m_transform = glm::mat4(1.f);
    glm::mat4 m_scale = glm::mat4(1.f);
    glm::mat4 m_rotation = glm::mat4(1.f);
};
}