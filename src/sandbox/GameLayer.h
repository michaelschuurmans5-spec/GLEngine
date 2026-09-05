#pragma once

#include "core/Layer.h"
#include "renderer/VertexArray.h"
#include "renderer/Shader.h"
#include "renderer/Texture.h"
#include "renderer/Framebuffer.h"
#include "renderer/Camera.h"
#include <glm/glm.hpp>
#include <vector>
#include <string>
#include <memory>

struct LevelEntityInstance {
    std::string Name;
    glm::vec3 Position = glm::vec3(0.0f);
    glm::vec3 Rotation = glm::vec3(0.0f);
    glm::vec3 Scale = glm::vec3(1.0f);
    bool IsPrimitiveCube = true;
};

class GameLayer : public Layer {
public:
    GameLayer();
    virtual ~GameLayer() = default;

    virtual void OnAttach() override;
    virtual void OnDetach() override;
    virtual void OnUpdate(float deltaTime) override;
    virtual void OnRender() override;

private:
    std::unique_ptr<Camera> m_Camera;
    std::unique_ptr<Framebuffer> m_Framebuffer;

    std::shared_ptr<Shader> m_UnlitShader;
    std::shared_ptr<Shader> m_LitShader;
    std::shared_ptr<Shader> m_GodRayShader;
    std::shared_ptr<Shader> m_SkyShader;
    std::shared_ptr<Texture> m_Texture;

    std::shared_ptr<VertexArray> m_CubeVAO;
    std::shared_ptr<VertexArray> m_HumanVAO;
    uint32_t m_HumanIndexCount = 0;

    std::shared_ptr<VertexArray> m_SkyboxVAO;
    std::shared_ptr<VertexArray> m_ScreenQuadVAO;

    float m_TimeOfDay = 1.0f;
    glm::vec3 m_DynamicSunDir = glm::vec3(0.0f);
    float m_GodRayExposureOverride = 0.3f;

    int m_ViewportWidth = 800;
    int m_ViewportHeight = 600;
    bool m_F2KeyPressedLastFrame = false;

    // Level structures
    std::vector<LevelEntityInstance> m_EditorEntities;
    int m_ActiveSelectionIndex = -1;
};
