#pragma once 

#include "core/Layer.h"
#include "renderer/shader.h"
#include "renderer/VertexArray.h"
#include "renderer/Buffer.h"    
#include "renderer/Camera.h" 
#include "renderer/Texture.h"
#include "renderer/Framebuffer.h"

#include <memory>

class GameLayer : public Layer {
public:
    GameLayer();
    virtual ~GameLayer() = default;

    // OVERRIDING THE LIFECYCLE HOOKS FROM YOUR BASE LAYER CLASS
    virtual void OnAttach() override;
    virtual void OnDetach() override;
    virtual void OnUpdate() override;
    virtual void OnRender() override;
private:
    // SMART POINTERS
    std::unique_ptr<Shader> m_UnlitShader;
    std::unique_ptr<Shader> m_LitShader;
    std::unique_ptr<Shader> m_GodRayShader;
    std::unique_ptr<Camera> m_Camera;
    std::unique_ptr<Texture> m_Texture;
    std::unique_ptr<Framebuffer> m_Framebuffer;
    std::shared_ptr<VertexArray> m_ScreenQuadVAO;
    std::shared_ptr<VertexBuffer> m_ScreenQuadVBO;

    // DEFINE BASIC LIGHT VARIABLES 
    glm::vec3 LightPos = glm::vec3(2.0f, 4.0f, 3.0f);
    glm::vec3 LightColor = glm::vec3(1.0f, 1.0f, 0.9f);

    // ENTITY 1: THE CUBE ASSETS
    std::shared_ptr<VertexArray> m_CubeVAO;
    std::shared_ptr<VertexBuffer> m_CubeVBO;
    std::shared_ptr<IndexBuffer> m_CubeEBO;

    // ENTITY 2: THE HUMAN MESH ASSETS
    std::shared_ptr<VertexArray> m_HumanVAO;
    std::shared_ptr<VertexBuffer> m_HumanVBO;
    std::shared_ptr<IndexBuffer> m_HumanEBO;
    uint32_t m_HumanIndexCount = 0;

    // WINDOW VIEWPORT SIZE 
    uint32_t m_ViewportWidth = 800;
    uint32_t m_ViewportHeight = 600;

    // SUN VARIABLES 
    float m_TimeOfDay = 0.0f; // Tracks our day/night cycle clock
    glm::vec3 m_DynamicSunDir = glm::vec3(0.0f, 1.0f, 0.0f); // The current 3D direction pointing toward the sun
    

    // SKY ELEMENTS
    std::unique_ptr<Shader> m_SkyShader; 
    std::shared_ptr<VertexArray> m_SkyboxVAO;
    std::shared_ptr<VertexBuffer> m_SkyboxVBO;
};