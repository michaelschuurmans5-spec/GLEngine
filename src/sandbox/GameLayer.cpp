#include "GameLayer.h"
#include "core/Log.h"
#include "core/Application.h" 
#include "renderer/Camera.h" 
#include "resources/ModelLoader.h" 
#include <glad/glad.h>
#include <glfw/glfw3.h>
#include <cmath>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

static glm::vec3 s_CameraPos = glm::vec3(0.0f, 1.0f, 4.0f); 
static float s_CameraMoveSpeed = 0.04f;

static double s_LastX = 400.0, s_LastY = 300.0;
static bool s_FirstMouse = true;

GameLayer::GameLayer() : Layer("GameSandboxLayer") {}

void GameLayer::OnAttach() {
    ENGINE_INFO("GameLayer Attached! Loading Multi-Entity 3D Scene...");

    // SETUP OBJECT A: THE 3D CUBE GEOMETRY
    float cubeVertices[] = {
        // Front Face          // UVs
        -0.5f, -0.5f,  0.5f,   0.0f, 0.0f,
         0.5f, -0.5f,  0.5f,   1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,   1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,   0.0f, 1.0f,
        // Back Face
        -0.5f, -0.5f, -0.5f,   1.0f, 0.0f,
         0.5f, -0.5f, -0.5f,   0.0f, 0.0f,
         0.5f,  0.5f, -0.5f,   0.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,   1.0f, 1.0f
    };
    uint32_t cubeIndices[] = {
        0, 1, 2,  2, 3, 0, // Front
        1, 5, 6,  6, 2, 1, // Right
        7, 6, 5,  5, 4, 7, // Back
        4, 0, 3,  3, 7, 4, // Left
        3, 2, 6,  6, 7, 3, // Top
        4, 5, 1,  1, 0, 4  // Bottom
    };

    m_CubeVAO = std::make_shared<VertexArray>();
    m_CubeVBO = std::make_shared<VertexBuffer>(cubeVertices, (uint32_t)sizeof(cubeVertices));

    //Explicitly declare layout
    m_CubeVBO->SetLayout({
       { ShaderDataType::Float3, "aPos" },
       { ShaderDataType::Float2, "aTexCoord" }
    });

    m_CubeVAO->AddVertexBuffer(m_CubeVBO);
    m_CubeEBO = std::make_shared<IndexBuffer>(cubeIndices, 36);
    m_CubeVAO->SetIndexBuffer(m_CubeEBO);

    // SETUP OBJECT B: ASSIMP HUMAN MODEL IMPORT
    LoadedMeshData humanData;
    std::string humanPath = std::string(ENGINE_ASSET_DIR) + "Models/Basic Mesh Rigged/Human/Base Mesh sculpt 2.obj";

    if (ModelLoader::LoadMesh(humanPath, humanData)) {
        m_HumanIndexCount = (uint32_t)humanData.Indices.size();

        m_HumanVAO = std::make_shared<VertexArray>();
        m_HumanVBO = std::make_shared<VertexBuffer>((float*)humanData.Vertices.data()
            , (uint32_t)(humanData.Vertices.size() * sizeof(ModelVertex)));

        // Explicitly declare layout
        m_HumanVBO->SetLayout({
        { ShaderDataType::Float3, "aPos" },
        { ShaderDataType::Float2, "aTexCoord" },
        { ShaderDataType::Float3, "aNormal" }
        });

        m_HumanVAO->AddVertexBuffer(m_HumanVBO);
        m_HumanEBO = std::make_shared<IndexBuffer>(humanData.Indices.data(), m_HumanIndexCount);
        m_HumanVAO->SetIndexBuffer(m_HumanEBO);
    }
    else {
        ENGINE_ERROR("Failed to load human mesh asset!");
    }

    // CORE ASSETS INITIALIZATION
    std::string staticVert = std::string(ENGINE_ASSET_DIR) + "Shaders/VertexDeformation/static.vert";
    std::string unlitFrag = std::string(ENGINE_ASSET_DIR) + "Shaders/ShadingModels/unlit.frag";
    std::string litFrag = std::string(ENGINE_ASSET_DIR) + "Shaders/ShadingModels/forward_lit.frag";
    std::string godRayVert = std::string(ENGINE_ASSET_DIR) + "Shaders/ShadingModels/god_rays.vert";
    std::string godRayFrag = std::string(ENGINE_ASSET_DIR) + "Shaders/ShadingModels/god_rays.frag";

    m_UnlitShader = std::make_unique<Shader>(staticVert, unlitFrag);
    m_LitShader = std::make_unique<Shader>(staticVert, litFrag);
    m_GodRayShader = std::make_unique<Shader>(godRayVert, godRayFrag);

    std::string texPath = std::string(ENGINE_ASSET_DIR) + "Textures/Snow/Snow010A_2K-PNG_Color.png";
    m_Texture = std::make_unique<Texture>(texPath);

    m_UnlitShader->Bind();
    int unlitTexLoc= glGetUniformLocation(m_UnlitShader->GetRendererID(), "u_Texture");
    if (unlitTexLoc != -1) glUniform1i(unlitTexLoc, 0);

    m_Camera = std::make_unique<Camera>(80.0f, 1.6f, 0.1f, 100.0f);
    m_Camera->SetPosition(s_CameraPos);

    // INIT OFF SCREEN RENDERING CANVAS
    m_Framebuffer = std::make_unique<Framebuffer>(800, 600);

    // NORMLIZED COORDINATE SPACES COVERING FULL WINDOW AREA 
    float quadVertices[] = {
        // Positions   // TexCoords
        -1.0f,  1.0f,  0.0f, 1.0f, // Top-Left
        -1.0f, -1.0f,  0.0f, 0.0f, // Bottom-Left
         1.0f, -1.0f,  1.0f, 0.0f, // Bottom-Right

        -1.0f,  1.0f,  0.0f, 1.0f, // Top-Left
         1.0f, -1.0f,  1.0f, 0.0f, // Bottom-Right
         1.0f,  1.0f,  1.0f, 1.0f  // Top-Right
    };

    m_ScreenQuadVAO = std::make_shared<VertexArray>();
    m_ScreenQuadVBO = std::make_shared<VertexBuffer>(quadVertices, (uint32_t)sizeof(quadVertices));

    m_ScreenQuadVBO->SetLayout({
        { ShaderDataType::Float2, "aPos" },
        { ShaderDataType::Float2, "aTexCoords" }
        });
    m_ScreenQuadVAO->AddVertexBuffer(m_ScreenQuadVBO);

    m_GodRayShader->Bind();
    int screenTexLoc = glGetUniformLocation(m_GodRayShader->GetRendererID(), "u_ScreenTexture");
    if (screenTexLoc != -1) glUniform1i(screenTexLoc, 0);

    GLFWwindow* window = Application::Get().GetWindow().getNativeWindow();
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glEnable(GL_DEPTH_TEST);
}

void GameLayer::OnUpdate() {
    GLFWwindow* window = Application::Get().GetWindow().getNativeWindow();

    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    }

    double mouseX, mouseY;
    glfwGetCursorPos(window, &mouseX, &mouseY);

    if (s_FirstMouse) {
        s_LastX = mouseX;
        s_LastY = mouseY;
        s_FirstMouse = false;
    }

    float xOffset = (float)(mouseX - s_LastX);
    float yOffset = (float)(s_LastY - mouseY);

    s_LastX = mouseX;
    s_LastY = mouseY;

    if (glfwGetInputMode(window, GLFW_CURSOR) == GLFW_CURSOR_DISABLED) {
        m_Camera->ProcessMouseMovement(xOffset, yOffset);
    }

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) s_CameraPos += s_CameraMoveSpeed * m_Camera->GetFrontVector();
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) s_CameraPos -= s_CameraMoveSpeed * m_Camera->GetFrontVector();
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) s_CameraPos -= m_Camera->GetRightVector() * s_CameraMoveSpeed;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) s_CameraPos += m_Camera->GetRightVector() * s_CameraMoveSpeed;

    m_Camera->SetPosition(s_CameraPos);
}

void GameLayer::OnRender() {
    // RETAIN WINDOW SIZE FULL SCREEN
    GLFWwindow* window = Application::Get().GetWindow().getNativeWindow();
    int currentWidth, currentHeight;
    glfwGetFramebufferSize(window, &currentWidth, &currentHeight);

    // RUNTIME CHECKS: Did the user resize or maximize the window?
    if (currentWidth > 0 && currentHeight > 0 &&
        (currentWidth != m_ViewportWidth || currentHeight != m_ViewportHeight))
    {
        m_ViewportWidth = currentWidth;
        m_ViewportHeight = currentHeight;

        // Reallocate off-screen texture attachment boundaries dynamically
        m_Framebuffer->Resize(m_ViewportWidth, m_ViewportHeight);

        // Update your Camera's internal 3D perspective mapping calculations
        float aspect = (float)m_ViewportWidth / (float)m_ViewportHeight;
        m_Camera = std::make_unique<Camera>(80.0f, aspect, 0.1f, 100.0f); //
        m_Camera->SetPosition(s_CameraPos); //
    }

    //  PASS 1: RENDER STANDARD GAME SCENE INTO THE FRAMEBUFFER
    m_Framebuffer->Bind();
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f); // Dark background helps rays pop!
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    m_Texture->Bind(0);
    // Render Unlit Cube

    m_UnlitShader->Bind();
    m_UnlitShader->SetUniformMat4("u_ViewProjection",
        m_Camera->GetViewProjectionMatrix());
    glm::mat4 cubeTransform = glm::translate(glm::mat4(1.0f),
        glm::vec3(-1.5f, 0.0f, 0.0f));
    m_UnlitShader->SetUniformMat4("u_Transform", cubeTransform);
    m_CubeVAO->Bind();
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, nullptr);

    //  Render Lit Human
    if (m_HumanVAO) {
        m_LitShader->Bind();
        m_LitShader->SetUniformMat4("u_ViewProjection",
            m_Camera->GetViewProjectionMatrix());
        glm::mat4 humanTransform = glm::translate(glm::mat4(1.0f),
            glm::vec3(1.0f, -0.5f, 0.0f));
        m_LitShader->SetUniformMat4("u_Transform", humanTransform);

        // PASS LIGHTING DATA TO GPU
        glm::vec3 lightPos = glm::vec3(3.0f, 5.0f, 4.0f);
        glm::vec3 lightColor = glm::vec3(1.0f, 1.0f, 1.0f);
        // Pass Lighting and Camera positions for Fragment calculations
        m_LitShader->SetUniformFloat3("u_LightPos", LightPos);
        m_LitShader->SetUniformFloat3("u_LightColor", LightColor);
        m_LitShader->SetUniformFloat3("u_ViewPos", m_Camera->GetPosition());

        m_HumanVAO->Bind();
        glDrawElements(GL_TRIANGLES, m_HumanIndexCount, GL_UNSIGNED_INT, nullptr);

    }

    m_Framebuffer->Unbind();// Stop rendering to the Framebuffer texture target

    // PASS 2: RENDER POST-PROCESSING GOD RAYS TO THE SCREEN

      // Clear your physical screen monitor canvas 
    glViewport(0, 0, m_ViewportWidth, m_ViewportHeight);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Calculate the light's 2D Screen Position
        glm::vec3 lightWorldPos = glm::vec3(3.0f, 5.0f, 4.0f);
        glm::vec4 clipSpacePos = m_Camera->GetViewProjectionMatrix()
            * glm::vec4(lightWorldPos, 1.0f);
        glm::vec3 ndcPos = glm::vec3(clipSpacePos) / clipSpacePos.w;
        glm::vec2 lightScreenPos;
        lightScreenPos.x = (ndcPos.x + 1.0f) * 0.5f;
        lightScreenPos.y = (ndcPos.y + 1.0f) * 0.5f;

        // Bind the God Ray shader and assign variables
        m_GodRayShader->Bind();
        m_GodRayShader->SetUniformFloat2("u_LightScreenPos", lightScreenPos);

        // Bind the color canvas texture we generated in Pass 1 into slot 0
        glBindTexture(GL_TEXTURE_2D, m_Framebuffer->GetColorAttachmentRendererID());

        // Draw the full screen billboard quad to apply the raymarching effect
        m_ScreenQuadVAO->Bind();
        glDrawArrays(GL_TRIANGLES, 0, 6);

        // Cleanup Pipeline State
        m_ScreenQuadVAO->Unbind();
        m_GodRayShader->Unbind();
        
        m_CubeVAO->Unbind(); 
        m_LitShader->Unbind();
}

void GameLayer::OnDetach() {
    ENGINE_WARN("GameLayer Detached.");
}
