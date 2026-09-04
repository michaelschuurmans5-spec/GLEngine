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

    m_UnlitShader = std::make_unique<Shader>(staticVert, unlitFrag);
    m_LitShader = std::make_unique<Shader>(staticVert, litFrag);

    std::string texPath = std::string(ENGINE_ASSET_DIR) + "Textures/Snow/Snow010A_2K-PNG_Color.png";
    m_Texture = std::make_unique<Texture>(texPath);

    m_UnlitShader->Bind();
    int unlitTexLoc= glGetUniformLocation(m_UnlitShader->GetRendererID(), "u_Texture");
    if (unlitTexLoc != -1) glUniform1i(unlitTexLoc, 0);

    m_Camera = std::make_unique<Camera>(80.0f, 1.6f, 0.1f, 100.0f);
    m_Camera->SetPosition(s_CameraPos);

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
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // DRAW STEP 1: RENDER THE UNLIT CUBE ENTITY
    m_Texture->Bind(0);
    m_UnlitShader->Bind();
    m_UnlitShader->SetUniformMat4("u_ViewProjection", m_Camera->GetViewProjectionMatrix());

    glm::mat4 cubeTransform = glm::translate(glm::mat4(1.0f), glm::vec3(-1.5f, 0.0f, 0.0f));
    m_UnlitShader->SetUniformMat4("u_Transform", cubeTransform);

    m_CubeVAO->Bind();
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, nullptr);

    // DRAW STEP 2: RENDER THE LIT HUMAN ENTITY
    if (m_HumanVAO) {
        m_LitShader->Bind();
        m_LitShader->SetUniformMat4("u_ViewProjection", m_Camera->GetViewProjectionMatrix());

        glm::mat4 humanTransform = glm::translate(glm::mat4(1.0f), glm::vec3(1.0f, -0.5f, 0.0f));
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

    m_CubeVAO->Unbind(); 
    m_LitShader->Unbind();
}

void GameLayer::OnDetach() {
    ENGINE_WARN("GameLayer Detached.");
}
