#include "GameLayer.h"
#include "core/Application.h"
#include "core/Log.h"
#include "resources/ModelLoader.h"
#include "resources/AssetManager.h"

#include "GLFW/glfw3.h"

#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glad/glad.h>
#include <iostream>
#include <fstream>
#include <sstream>

// Static workspace configurations
static glm::vec3 s_CameraPos = glm::vec3(0.0f, 0.0f, 5.0f);
static float s_CameraMoveSpeed = 0.05f;
static bool s_FirstMouse = true;
static double s_LastX = 400.0, s_LastY = 300.0;

// STANDALONE FILE I/O HELPERS (Defined at the top so OnRender can see them)
static void SaveMap(const std::string& path, float time, const std::vector<LevelEntityInstance>& list) {
    std::ofstream out(path);
    if (!out.is_open()) {
        std::cout << "[Level Editor Error] Could not open file for writing: " << path << std::endl;
        return;
    }
    out << "TIME " << time << "\n";
    for (const auto& e : list) {
        out << "ENTITY\nNAME " << e.Name << "\nISCUBE " << (e.IsPrimitiveCube ? "1" : "0") << "\n";
        out << "POS " << e.Position.x << " " << e.Position.y << " " << e.Position.z << "\n";
        out << "ROT " << e.Rotation.x << " " << e.Rotation.y << " " << e.Rotation.z << "\n";
        out << "SCALE " << e.Scale.x << " " << e.Scale.y << " " << e.Scale.z << "\nEND\n";
    }
    out.close();
}

static void LoadMap(const std::string& path, float& time, std::vector<LevelEntityInstance>& list) {
    std::ifstream in(path);
    if (!in.is_open()) {
        std::cout << "[Level Editor Error] Could not load level file: " << path << std::endl;
        return;
    }
    list.clear();
    std::string line, tag; LevelEntityInstance current; bool inEntity = false;
    while (std::getline(in, line)) {
        if (line.empty() || line == "#") continue;
        std::istringstream ss(line); ss >> tag;
        if (tag == "TIME") ss >> time;
        else if (tag == "ENTITY") { current = LevelEntityInstance(); inEntity = true; }
        else if (tag == "NAME" && inEntity) { std::getline(ss, current.Name); if (!current.Name.empty() && current.Name[0] == ' ') current.Name.erase(0, 1); }
        else if (tag == "ISCUBE" && inEntity) { std::string val; ss >> val; current.IsPrimitiveCube = (val == "1"); }
        else if (tag == "POS" && inEntity) ss >> current.Position.x >> current.Position.y >> current.Position.z;
        else if (tag == "ROT" && inEntity) ss >> current.Rotation.x >> current.Rotation.y >> current.Rotation.z;
        else if (tag == "SCALE" && inEntity) ss >> current.Scale.x >> current.Scale.y >> current.Scale.z;
        else if (tag == "END" && inEntity) { list.push_back(current); inEntity = false; }
    }
    in.close();
}

GameLayer::GameLayer() : Layer("GameSandboxLayer") {}

void GameLayer::OnAttach() {
    ENGINE_INFO("Integrated Level Editor Attached!");

    GLFWwindow* window = Application::Get().GetWindow().getNativeWindow();
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glEnable(GL_DEPTH_TEST);

    // 1. Setup Camera & Framebuffer
    int w = 0, h = 0;
    glfwGetFramebufferSize(window, &w, &h);
    // If the window returns 0, clamp it to a standard default safety boundary aspect ratio
    if (w <= 0 || h <= 0) {
        w = 1280;
        h = 720;
    }
    m_ViewportWidth = w; m_ViewportHeight = h;
    m_Framebuffer = std::make_unique<Framebuffer>(m_ViewportWidth, m_ViewportHeight);

    float aspect = (float)m_ViewportWidth / (float)m_ViewportHeight;
    m_Camera = std::make_unique<Camera>(80.0f, aspect, 0.1f, 1000.0f);
    m_Camera->SetPosition(s_CameraPos);

    // 2. Setup Master Cube Primitives
    float cubeVertices[] = {
        -0.5f,-0.5f, 0.5f, 0.0f,0.0f,  0.5f,-0.5f, 0.5f, 1.0f,0.0f,  0.5f, 0.5f, 0.5f, 1.0f,1.0f, -0.5f, 0.5f, 0.5f, 0.0f,1.0f,
        -0.5f,-0.5f,-0.5f, 1.0f,0.0f,  0.5f,-0.5f,-0.5f, 0.0f,0.0f,  0.5f, 0.5f,-0.5f, 0.0f,1.0f, -0.5f, 0.5f,-0.5f, 1.0f,1.0f
    };
    uint32_t cubeIndices[] = {
        0,1,2, 2,3,0, 1,5,6, 6,2,1, 7,6,5, 5,4,7, 4,0,3, 3,7,4, 3,2,6, 6,7,3, 4,5,1, 1,0,4
    };

    m_CubeVAO = std::make_shared<VertexArray>();
    auto cubeVBO = std::make_shared<VertexBuffer>(cubeVertices, sizeof(cubeVertices));
    cubeVBO->SetLayout({ { ShaderDataType::Float3, "aPos" }, { ShaderDataType::Float2, "aTexCoord" } });
    m_CubeVAO->AddVertexBuffer(cubeVBO);
    m_CubeVAO->SetIndexBuffer(std::make_shared<IndexBuffer>(cubeIndices, 36));

    // 3. Setup Rigged Obstacle Model via Assimp Loader
    std::string humanPath = std::string(ENGINE_ASSET_DIR) + "Models/Basic Mesh Rigged/Human/Base Mesh sculpt 2.obj";
    LoadedMeshData humanData;
    if (ModelLoader::LoadMesh(humanPath, humanData)) {
        m_HumanIndexCount = (uint32_t)humanData.Indices.size();
        m_HumanVAO = std::make_shared<VertexArray>();
        auto humanVBO = std::make_shared<VertexBuffer>((float*)humanData.Vertices.data(), (uint32_t)(humanData.Vertices.size() * sizeof(ModelVertex)));
        humanVBO->SetLayout({ { ShaderDataType::Float3, "aPos" }, { ShaderDataType::Float2, "aTexCoord" }, { ShaderDataType::Float3, "aNormal" } });
        m_HumanVAO->AddVertexBuffer(humanVBO);
        m_HumanVAO->SetIndexBuffer(std::make_shared<IndexBuffer>(humanData.Indices.data(), m_HumanIndexCount));
    }

    // 4. Setup Post-Processing Screenspace Screen Billboard Quad
    float quadVertices[] = {
        -1.0f,  1.0f,  0.0f, 1.0f,  -1.0f, -1.0f,  0.0f, 0.0f,   1.0f, -1.0f,  1.0f, 0.0f,
        -1.0f,  1.0f,  0.0f, 1.0f,   1.0f, -1.0f,  1.0f, 0.0f,   1.0f,  1.0f,  1.0f, 1.0f
    };
    m_ScreenQuadVAO = std::make_shared<VertexArray>();
    auto quadVBO = std::make_shared<VertexBuffer>(quadVertices, sizeof(quadVertices));
    quadVBO->SetLayout({ { ShaderDataType::Float2, "aPos" }, { ShaderDataType::Float2, "aTexCoord" } });
    m_ScreenQuadVAO->AddVertexBuffer(quadVBO);

    // 5. Setup Infinite Skybox Backdrop Box Primitive
    float skyboxVertices[] = {
        -1,1,-1, -1,-1,-1, 1,-1,-1, 1,-1,-1, 1,1,-1, -1,1,-1,  -1,-1,1, -1,-1,-1, -1,1,-1, -1,1,-1, -1,1,1, -1,-1,1,
         1,-1,-1,  1,-1,1,  1,1,1,  1,1,1,  1,1,-1,  1,-1,-1,  -1,-1,1, -1,1,1,  1,1,1,  1,1,1,  1,-1,1, -1,-1,1,
        -1,1,-1,   1,1,-1,  1,1,1,  1,1,1,  -1,1,1,  -1,1,-1,  -1,-1,-1, -1,-1,1, 1,-1,1, 1,-1,1, 1,-1,-1, -1,-1,-1
    };
    m_SkyboxVAO = std::make_shared<VertexArray>();
    auto skyVBO = std::make_shared<VertexBuffer>(skyboxVertices, sizeof(skyboxVertices));
    skyVBO->SetLayout({ { ShaderDataType::Float3, "aPos" } });
    m_SkyboxVAO->AddVertexBuffer(skyVBO);

    // 6. Gather Shaders & Engine Textures
    std::string assetPath = std::string(ENGINE_ASSET_DIR);
    m_UnlitShader = AssetManager::GetShader(
        assetPath + "Shaders/VertexDeformation/static.vert", 
        assetPath + "Shaders/ShadingModels/unlit.frag"
    );
    m_LitShader = AssetManager::GetShader(
        assetPath + "Shaders/VertexDeformation/static.vert",
        assetPath + "Shaders/ShadingModels/forward_lit.frag"
    );
    m_GodRayShader = AssetManager::GetShader(
        assetPath + "Shaders/ShadingModels/god_rays.vert",
        assetPath + "Shaders/ShadingModels/god_rays.frag"
    );
    // Change your m_SkyShader assignment to look here:
    m_SkyShader = AssetManager::GetShader(
        assetPath + "Shaders/VertexDeformation/sky.vert",
        assetPath + "Shaders/ShadingModels/sky.frag"
    );
    m_Texture = std::make_shared<Texture>(assetPath + "Textures/Snow/Snow010A_2K-PNG_Color.png");

    // Initialize ImGui Runtime bindings
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 410");

    // Spawn starting floor runway plate
    LevelEntityInstance ground;
    ground.Name = "Starting Ground Platform";
    ground.Position = glm::vec3(0.0f, -2.0f, 0.0f);
    ground.Scale = glm::vec3(20.0f, 0.5f, 6.0f);
    ground.IsPrimitiveCube = true;
    m_EditorEntities.push_back(ground);
}

void GameLayer::OnUpdate(float deltaTime) {
    GLFWwindow* window = Application::Get().GetWindow().getNativeWindow();

    // Toggle states via dynamic latch flags
    bool isF2PressedNow = (glfwGetKey(window, GLFW_KEY_F2) == GLFW_PRESS);
    if (isF2PressedNow && !m_F2KeyPressedLastFrame) {
        if (Application::Get().GetMode() == AppMode::Game) {
            Application::Get().SetMode(AppMode::Editor);
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        }
        else {
            Application::Get().SetMode(AppMode::Game);
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        }
    }
    m_F2KeyPressedLastFrame = isF2PressedNow;

    // Gate atmosphere progression timestep to live gameplay tracking
    if (Application::Get().GetMode() == AppMode::Game) {
        m_TimeOfDay += 0.5f * deltaTime;
    }

    // Process camera WASD linear translations in both environments
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) s_CameraPos += s_CameraMoveSpeed * m_Camera->GetFrontVector();
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) s_CameraPos -= s_CameraMoveSpeed * m_Camera->GetFrontVector();
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) s_CameraPos -= m_Camera->GetRightVector() * s_CameraMoveSpeed;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) s_CameraPos += m_Camera->GetRightVector() * s_CameraMoveSpeed;

    // Generate orbiting environment direction vectors
    m_DynamicSunDir.x = cos(m_TimeOfDay);
    m_DynamicSunDir.y = sin(m_TimeOfDay);
    m_DynamicSunDir.z = -0.5f;
    m_DynamicSunDir = glm::normalize(m_DynamicSunDir);

    double mouseX, mouseY;
    glfwGetCursorPos(window, &mouseX, &mouseY);
    if (s_FirstMouse) { s_LastX = mouseX; s_LastY = mouseY; s_FirstMouse = false; }
    float xOffset = (float)(mouseX - s_LastX); float yOffset = (float)(s_LastY - mouseY);
    s_LastX = mouseX; s_LastY = mouseY;

    bool isGameMode = (Application::Get().GetMode() == AppMode::Game);
    bool isRightClickHeld = (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS);

    if (isGameMode || isRightClickHeld) {
        if (!isGameMode) glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        m_Camera->ProcessMouseMovement(xOffset, yOffset);
    }
    else {
        if (!isGameMode && glfwGetInputMode(window, GLFW_CURSOR) == GLFW_CURSOR_DISABLED) {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        }
    }
    m_Camera->SetPosition(s_CameraPos);
}

void GameLayer::OnRender() {
    GLFWwindow* window = Application::Get().GetWindow().getNativeWindow();
    int currentWidth, currentHeight;
    glfwGetFramebufferSize(window, &currentWidth, &currentHeight);

    if (currentWidth > 0 && currentHeight > 0 && (currentWidth != m_ViewportWidth || currentHeight != m_ViewportHeight)) {
        m_ViewportWidth = currentWidth; m_ViewportHeight = currentHeight;
        m_Framebuffer->Resize(m_ViewportWidth, m_ViewportHeight);
        float aspect = (float)m_ViewportWidth / (float)m_ViewportHeight;
        m_Camera = std::make_unique<Camera>(80.0f, aspect, 0.1f, 1000.0f);
        m_Camera->SetPosition(s_CameraPos);
    }

    // PASS 1: RENDER THE SCENE TO THE FRAMEBUFFER
    m_Framebuffer->Bind();
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Draw Skybox background backdrop canvas
    glDepthMask(GL_FALSE); glDepthFunc(GL_LEQUAL);
    m_SkyShader->Bind();
    glm::mat4 skyViewMatrix = glm::mat4(glm::mat3(m_Camera->GetViewMatrix()));
    m_SkyShader->SetUniformMat4("u_ViewProjection", m_Camera->GetProjectionMatrix() * skyViewMatrix);
    m_SkyShader->SetUniformFloat3("u_DynamicSunDir", m_DynamicSunDir);
    m_SkyShader->SetUniformFloat("u_Time", m_TimeOfDay);
    m_SkyboxVAO->Bind(); glDrawArrays(GL_TRIANGLES, 0, 36);
    glDepthMask(GL_TRUE); glDepthFunc(GL_LESS);

    // Draw all active placed entities in the level registry loop
    m_Texture->Bind(0);
    for (const auto& entity : m_EditorEntities) {
        glm::mat4 transform = glm::translate(glm::mat4(1.0f), entity.Position);
        transform = glm::rotate(transform, glm::radians(entity.Rotation.x), glm::vec3(1, 0, 0));
        transform = glm::rotate(transform, glm::radians(entity.Rotation.y), glm::vec3(0, 1, 0));
        transform = glm::rotate(transform, glm::radians(entity.Rotation.z), glm::vec3(0, 0, 1));
        transform = glm::scale(transform, entity.Scale);

        if (entity.IsPrimitiveCube) {
            // Protect Cube drawings
            if (m_UnlitShader && m_CubeVAO) {
                m_UnlitShader->Bind();
                m_UnlitShader->SetUniformMat4("u_ViewProjection", m_Camera->GetViewProjectionMatrix());
                m_UnlitShader->SetUniformMat4("u_Transform", transform);
                m_CubeVAO->Bind();
                glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, nullptr);
            }
        }
        else {
            // Heavy protection check on your human obstacle mesh model asset
            if (m_LitShader && m_HumanVAO) {
                m_LitShader->Bind();
                m_LitShader->SetUniformMat4("u_ViewProjection", m_Camera->GetViewProjectionMatrix());
                m_LitShader->SetUniformMat4("u_Transform", transform);
                m_LitShader->SetUniformFloat3("u_LightPos", m_DynamicSunDir * 50.0f);
                m_LitShader->SetUniformFloat3("u_LightColor", glm::vec3(1.0f, 1.0f, 1.0f));
                m_LitShader->SetUniformFloat3("u_ViewPos", m_Camera->GetPosition());

                m_HumanVAO->Bind();
                glDrawElements(GL_TRIANGLES, m_HumanIndexCount, GL_UNSIGNED_INT, nullptr);
            }
        }
    }
    m_Framebuffer->Unbind();

    // PASS 2: RENDER POST-PROCESSING GOD RAYS
    glViewport(0, 0, m_ViewportWidth, m_ViewportHeight);
    glClearColor(0, 0, 0, 1); glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (m_GodRayShader && m_ScreenQuadVAO && m_Framebuffer) {
        m_GodRayShader->Bind();
        glm::vec4 clipPos = m_Camera->GetViewProjectionMatrix() * glm::vec4(m_DynamicSunDir * 50.0f, 1.0f);

        if (clipPos.w > 0.0f) {
            glm::vec2 screenPos = glm::vec2((clipPos.x / clipPos.w + 1.0f) * 0.5f, (clipPos.y / clipPos.w + 1.0f) * 0.5f);
            m_GodRayShader->SetUniformFloat2("u_LightScreenPos", screenPos);
            m_GodRayShader->SetUniformFloat("u_Exposure", (Application::Get().GetMode() == AppMode::Game) ? (glm::clamp(m_DynamicSunDir.y, 0.0f, 1.0f) * 0.3f) : m_GodRayExposureOverride);
        }
        else {
            m_GodRayShader->SetUniformFloat("u_Exposure", 0.0f);
        }
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, m_Framebuffer->GetColorAttachmentRendererID());
        m_ScreenQuadVAO->Bind();
        glDrawArrays(GL_TRIANGLES, 0, 6);
    }

    // PASS 3: RENDER THE IMGUI WORKSPACE DESKTOP LAYOUT
    ImGui_ImplOpenGL3_NewFrame(); ImGui_ImplGlfw_NewFrame(); ImGui::NewFrame();

    if (Application::Get().GetMode() == AppMode::Editor) {
        ImGui::Begin("Level Builder Panel");
        if (ImGui::Button("Spawn Platform Base Block", ImVec2(-1, 30))) {
            LevelEntityInstance inst;
            inst.Name = "Platform Node " + std::to_string(m_EditorEntities.size());
            inst.IsPrimitiveCube = true;
            inst.Position = m_Camera->GetPosition() + (m_Camera->GetFrontVector() * 5.0f);
            m_EditorEntities.push_back(inst);
            m_ActiveSelectionIndex = (int)m_EditorEntities.size() - 1;
        }
        if (ImGui::Button("Spawn Obstacle Rigged Actor", ImVec2(-1, 30))) {
            LevelEntityInstance inst;
            inst.Name = "Character Obstacle " + std::to_string(m_EditorEntities.size());
            inst.IsPrimitiveCube = false;
            inst.Position = m_Camera->GetPosition() + (m_Camera->GetFrontVector() * 5.0f);
            m_EditorEntities.push_back(inst);
            m_ActiveSelectionIndex = (int)m_EditorEntities.size() - 1;
        }

        ImGui::Spacing(); ImGui::Separator(); ImGui::Spacing();
        static char levelFileName[64] = { "MyFirstLevel.level" };
        ImGui::InputText("Save File Name", levelFileName, sizeof(levelFileName));

        std::string fullPath = std::string(ENGINE_ASSET_DIR) + std::string(levelFileName);
        if (ImGui::Button("Save Current Level Layout", ImVec2(-1, 25))) SaveMap(fullPath, m_TimeOfDay, m_EditorEntities);
        if (ImGui::Button("Load Map From File Name", ImVec2(-1, 25))) LoadMap(fullPath, m_TimeOfDay, m_EditorEntities);
        ImGui::End();

        ImGui::Begin("Active Scene Graph Hierarchy");
        ImGui::BeginChild("CollectionTree", ImVec2(180, 0), true);
        for (int i = 0; i < m_EditorEntities.size(); i++) {
            if (ImGui::Selectable(m_EditorEntities[i].Name.c_str(), m_ActiveSelectionIndex == i)) m_ActiveSelectionIndex = i;
        }
        ImGui::EndChild(); ImGui::SameLine();

        ImGui::BeginChild("PropertiesInspector");
        if (m_ActiveSelectionIndex >= 0 && m_ActiveSelectionIndex < m_EditorEntities.size()) {
            auto& target = m_EditorEntities[m_ActiveSelectionIndex];
            ImGui::Text("Transform Specifications"); ImGui::Separator();
            ImGui::DragFloat3("World Position Offset", &target.Position.x, 0.05f);
            ImGui::DragFloat3("Euler Rotations Axis", &target.Rotation.x, 0.5f);
            ImGui::DragFloat3("Dimension Scaling Matrix", &target.Scale.x, 0.02f);
            if (ImGui::Button("Delete Object from Map", ImVec2(-1, 22))) { m_EditorEntities.erase(m_EditorEntities.begin() + m_ActiveSelectionIndex); m_ActiveSelectionIndex = -1; }
        }
        else { ImGui::Text("Select an entity to view its properties."); }
        ImGui::EndChild(); ImGui::End();
    }
    ImGui::Render(); ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}
   
void GameLayer::OnDetach() {
    ENGINE_WARN("GameLayer Detached.");

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

}
