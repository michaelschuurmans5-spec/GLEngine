#include "GameLayer.h"
#include "core/Log.h"
#include "core/Application.h" 
#include "renderer/Camera.h" 
#include "resources/ModelLoader.h" 
#include "resources/AssetManager.h"

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

    m_UnlitShader = AssetManager::GetShader(staticVert, unlitFrag);
    m_LitShader = AssetManager::GetShader(staticVert, litFrag);
    m_GodRayShader = AssetManager::GetShader(godRayVert, godRayFrag);

    std::string texPath = std::string(ENGINE_ASSET_DIR) + "Textures/Snow/Snow010A_2K-PNG_Color.png";
    m_Texture = AssetManager::GetTexture(texPath);

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

    // SKY
    float skyboxVertices[] = {
        // Positions          
        -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

        -1.0f,  1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f,  1.0f
    };
    // SKY PATHS
    std::string skyVert = std::string(ENGINE_ASSET_DIR) + "Shaders/VertexDeformation/sky.vert";
    std::string skyFrag = std::string(ENGINE_ASSET_DIR) + "Shaders/ShadingModels/sky.frag";
    m_SkyShader = AssetManager::GetShader(skyVert, skyFrag);
    // ARRAY & BUFFERS
    m_SkyboxVAO = std::make_shared<VertexArray>();
    m_SkyboxVBO = std::make_shared<VertexBuffer>(skyboxVertices, (uint32_t)sizeof(skyboxVertices));
    // LAYOUT 
    m_SkyboxVBO->SetLayout({
    { ShaderDataType::Float3, "aPos" }
        });
    m_SkyboxVAO->AddVertexBuffer(m_SkyboxVBO);

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

    // SUN
    // INCREMENT OUR CYCLE CLOCK ADJUST TIME OF DAY 
    m_TimeOfDay += 0.1f * s_CameraMoveSpeed;
    // Calculate the Sun Direction vector spinning on a circular arc (X and Y axis)
    m_DynamicSunDir.x = cos(m_TimeOfDay);
    m_DynamicSunDir.y = sin(m_TimeOfDay); // Moves up and down (Sunrise -> Noon -> Sunset -> Night)
    m_DynamicSunDir.z = -0.5f;            // Slight tilt back so it shines on the front of our assets
    m_DynamicSunDir = glm::normalize(m_DynamicSunDir);

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
        m_Camera = std::make_unique<Camera>(80.0f, aspect, 0.1f, 100.0f); 
        m_Camera->SetPosition(s_CameraPos);
    }

    //  PASS 1: RENDER STANDARD GAME SCENE INTO THE FRAMEBUFFER
    m_Framebuffer->Bind();
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f); // Dark background helps rays pop!
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // SUB-STEP: RENDER THE PROCEDURAL SKYBOX BACKGROUND
    glDepthFunc(GL_LEQUAL);
    m_SkyShader->Bind();
    // Stripping out position vectors from your View matrix so the skybox centers on the camera view path
    glm::mat4 skyViewMatrix = glm::mat4(glm::mat3(m_Camera->GetViewMatrix()));
    glm::mat4 skyViewProj = m_Camera->GetProjectionMatrix() * skyViewMatrix;
    m_SkyShader->SetUniformMat4("u_ViewProjection", skyViewProj);
    m_SkyShader->SetUniformFloat3("u_DynamicSunDir", m_DynamicSunDir);
    m_SkyShader->SetUniformFloat("u_Time", m_TimeOfDay);

    m_SkyboxVAO->Bind();
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glDepthFunc(GL_LESS);

    // Cube & MESH 
    m_Texture->Bind(0);
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

        // LIGHTING (SUN) 
        glm::vec3 dynamicLightPos = m_DynamicSunDir * 50.0f;
        // CHANGE SUN COLOR BASED ON HEIGHT
        glm::vec3 dynamicLightColor = glm::vec3(1.0f, 1.0f, 1.0f);
        if (m_DynamicSunDir.y < 0.2f && m_DynamicSunDir.y > 0.0f) {
            // Golden Hour / Sunset: Blend to a warm orange/red sun
            dynamicLightColor = glm::vec3(1.0f, 0.5f, 0.2f);
        }
        else if (m_DynamicSunDir.y <= 0.0f) {
            // Night time: Turn the sun off completely! (Dim blue ambient moonlight)
            dynamicLightColor = glm::vec3(0.05f, 0.05f, 0.1f);
        }
        // Upload the dynamic, moving properties to the GPU
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

    // dynamic far-away sun coordinate position for screen projection
    glm::vec3 sunWorldPos = m_DynamicSunDir * 50.0f;
    glm::vec4 clipSpacePos = m_Camera->GetViewProjectionMatrix() * glm::vec4(sunWorldPos, 1.0f);

    // CHECK: Only process god rays if the sun is in front of the camera view frustum
    if (clipSpacePos.w > 0.0f) {
        glm::vec3 ndcPos = glm::vec3(clipSpacePos) / clipSpacePos.w;

        glm::vec2 lightScreenPos;
        lightScreenPos.x = (ndcPos.x + 1.0f) * 0.5f;
        lightScreenPos.y = (ndcPos.y + 1.0f) * 0.5f;

        // Bind the God Ray shader and assign variables
        m_GodRayShader->Bind();
        m_GodRayShader->SetUniformFloat2("u_LightScreenPos", lightScreenPos);

        // Adjust the intensity uniform dynamically so rays fade out when the sun sets below the horizon
        float sunsetExposureFactor = glm::clamp(m_DynamicSunDir.y, 0.0f, 1.0f);
        m_GodRayShader->SetUniformFloat("u_Exposure", sunsetExposureFactor * 0.3f);
    }
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
