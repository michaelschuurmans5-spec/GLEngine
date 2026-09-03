#include "GameLayer.h"
#include "core/Log.h"
#include "core/Application.h"
#include "renderer/Camera.h"

#include <glad/glad.h>
#include <glfw/glfw3.h>
#include <cmath>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

// NAVIGATION TRACKING SCALARS 
static glm::vec3 s_CameraPos = glm::vec3(0.0f, 0.0f, 3.0f); // Put camera at Z=3
static float s_CameraMoveSpeed = 0.04f;
// MOUSE POSITIONAL DELTAS TRACKING STATE 
static double s_LastX = 400.0, s_LastY = 300.0;
static bool s_FirstMouse = true;

GameLayer::GameLayer() : Layer("GameSandboxLayer") {}

void GameLayer::OnAttach() {
    ENGINE_INFO("GameLayer Attached! Reading assets from external GLSL Files...");

    //  DYNAMIC DATA STRIDE
    float vertices[] = {
        // Positions          // UVs (Texture Bounds Mapping Mapping)
       -0.5f, -0.5f, 0.0f,   0.0f, 0.0f, // 0: Bottom-Left corner of image
        0.5f, -0.5f, 0.0f,   1.0f, 0.0f, // 1: Bottom-Right corner of image
        0.5f,  0.5f, 0.0f,   1.0f, 1.0f, // 2: Top-Right corner of image
       -0.5f,  0.5f, 0.0f,   0.0f, 1.0f  // 3: Top-Left corner of image
    };
    // INDEX MAP
    uint32_t indices[] = {
        0, 1, 2, // First Triangle: Bottom-Left -> Bottom-Right -> Top-Right
        2, 3, 0  // Second Triangle: Top-Right -> Top-Left -> Bottom-Left
    };

    // ENGINE ASSET DIR MACRO 
    std::string vertPath = std::string(ENGINE_ASSET_DIR) + "Shaders/basic.vert";
    std::string fragPath = std::string(ENGINE_ASSET_DIR) + "Shaders/basic.frag";

    // INSTANTIATE OUR NEW CUSTOM SHADER
    m_Shader = std::make_unique<Shader>(vertPath, fragPath);
    // INITIALIZE YOUR VERTEX ARRAY CONTAINER OBJECT 
    m_VertexArray = std::make_shared<VertexArray>();
    // CREATE VERTEX BUFFER THIS AUTO UPLOADS DATA TO THE GPU INTERNALLY
    m_VertexBuffer = std::make_shared<VertexBuffer>(vertices, (uint32_t)sizeof(vertices));
    // CONNECT THE BUFFER TO THIS ARRAY THIS SETS UP LAYOUT ATTRIBUTES INTERNALLY 
    m_VertexArray->AddVertexBuffer(m_VertexBuffer);
    // INDEX BUFFER 
    m_IndexBuffer = std::make_shared<IndexBuffer>(indices, 6);
    // BIND INDEX BUFFER LAYOUT CONFIG INTO MASTER SHADER VERTEX ARRAY OBJECT
    m_VertexArray->SetIndexBuffer(m_IndexBuffer);

    // TEXTURE: Snow Path
    std::string texPath = std::string(ENGINE_ASSET_DIR) + "Textures/Snow/Snow010A_2K-PNG_Color.png";
    m_Texture = std::make_unique<Texture>(texPath);

    // TELL FRAGMENT SHADER UNIFORM THAT OUR TEXTURE IS MAPPED TO GPU HARDWARE SLOT 0
    m_Shader->Bind();
    m_Shader->SetUniform4f("u_Color", 1.0f, 1.0f, 1.0f, 1.0f);

    // INTEGER VALUE OF 0 TELLS THE SAMPLER2D "u_Texture" TO READ HARDWARE PIPELINE SLOT GL_TEXTURE0
    int textureSlotIndex = 0;
    int location = glGetUniformLocation(m_Shader->GetRendererID(), "u_Texture");
    if (location != -1) {
        glUniform1i(location, textureSlotIndex);
    }
    else {
        ENGINE_WARN("Warning: Uniform 'u_Texture' could not be mapped to registry!");
    }

    // CAMERA WITH AN 80 DEGREE FIELD OF VIEW 
    m_Camera = std::make_unique<Camera>(80.0f, 1.6f, 0.1f, 100.0f);
    m_Camera->SetPosition(s_CameraPos);

    // LOCK MOUSE POINTER INSIDE VIEWPORT KEEPS CURSOR INVISBLE AND CENTERED INSIDE SCREEN CANVAS WINDOW
    GLFWwindow* window = Application::Get().GetWindow().getNativeWindow();
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

void GameLayer::OnUpdate() {
    // GRAB RAW GLFW WINDOW HANDLE FROM APPLICATION LAYER 
    GLFWwindow* window = Application::Get().GetWindow().getNativeWindow();

    // ESCAPE UNLOCK SYSTEM
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    }
    // MOUSE ACCELERATION CALCULATION ROUTINE
    double mouseX, mouseY;
    glfwGetCursorPos(window, &mouseX, &mouseY);

    if (s_FirstMouse) {
        s_LastX = mouseX;
        s_LastY = mouseY;
        s_FirstMouse = false;
    }

    // Find distances the cursor traveled since the previous frame execution step
    float xOffset = (float)(mouseX - s_LastX);
    float yOffset = (float)(s_LastY - mouseY); // Inverted since y-coordinates go from bottom to top

    s_LastX = mouseX;
    s_LastY = mouseY;

    // ONLY PROCESS MOUSE LOOK IF THE CURSOR IS ACTUALLY LOCKED INSIDE SCREEN FRAME
    if (glfwGetInputMode(window, GLFW_CURSOR) == GLFW_CURSOR_DISABLED) {
        m_Camera->ProcessMouseMovement(xOffset, yOffset);
    }

    //  CAMERA FLIGHT SYSTEM (WASD MOVEMENT)
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        s_CameraPos += s_CameraMoveSpeed * m_Camera->GetFrontVector(); // Forward

    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        s_CameraPos -= s_CameraMoveSpeed * m_Camera->GetFrontVector(); // Backward

    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        s_CameraPos -= s_CameraMoveSpeed * m_Camera->GetRightVector(); // Strafe Left

    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        s_CameraPos += s_CameraMoveSpeed * m_Camera->GetRightVector(); // Strafe Right

    m_Camera->SetPosition(s_CameraPos);

}
void GameLayer::OnRender() {
    // ACTIVATE OUR SHADER PROGRAM PIPELINE
    m_Shader->Bind();
    // BIND THE TEXTURE MAP TO HARDWARE SLOT 0 BEFORE DRAW CALL ISSUES 
    m_Texture->Bind(0);

    // SQUARE STAYS LOCKED IN CENTER WORLD ORIGIN
    glm::mat4 squareTransform = glm::mat4(1.0f);
    // SHADER UNIFORM
    m_Shader->SetUniformMat4("u_ViewProjection",m_Camera->GetViewProjectionMatrix());
    m_Shader->SetUniformMat4("u_Transform", squareTransform);

    // BIND YOUR VERTEXARRAY ABSTRACTION OBJECT
    m_VertexArray->Bind();
    // ISSUE THE DRAW CALL TO THE HARDWARE
    glDrawElements(
        GL_TRIANGLES,
        m_VertexArray->GetIndexBuffer()->GetCount(),
        GL_UNSIGNED_INT,
        nullptr
    );

    // UNBIND CLEANLY
    m_VertexArray->Unbind();
    m_Shader->Unbind();
}

void GameLayer::OnDetach() {
    ENGINE_WARN("GameLayer Detached. Cleaning up GPU memory.");
}
