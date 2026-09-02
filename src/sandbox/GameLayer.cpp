#include "GameLayer.h"
#include "core/Log.h"
#include "core/Application.h"

#include <glad/glad.h>
#include <glfw/glfw3.h>
#include <cmath>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

// SQUARE COORDINATES AND SPEED
static glm::vec2 s_SquarePosition = glm::vec2(0.0f, 0.0f);
static float s_MoveSpeed = 0.01f;

GameLayer::GameLayer() : Layer("GameSandboxLayer") {}

void GameLayer::OnAttach() {
    ENGINE_INFO("GameLayer Attached! Reading assets from external GLSL Files...");

    //  DYNAMIC SQUARE LAYOUT X Y Z
    float vertices[] = {
        -0.5f, -0.5f, 0.0f, // 0: Bottom-Left
         0.5f, -0.5f, 0.0f, // 1: Bottom-Right
         0.5f,  0.5f, 0.0f, // 2: Top-Right
        -0.5f,  0.5f, 0.0f  // 3: Top-Left
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
}

void GameLayer::OnUpdate() {
    // GRAB RAW GLFW WINDOW HANDLE FROM APPLICATION LAYER 
    GLFWwindow* window = Application::Get().GetWindow().getNativeWindow();

    // CHECK KEY PATTERNS CLEANLY EVERY FRAME 
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
        s_SquarePosition.x -= s_MoveSpeed; // MOVE LEFT

    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
        s_SquarePosition.x += s_MoveSpeed; // MOVE RIGHT

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
        s_SquarePosition.y += s_MoveSpeed; // MOVE UP

    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
        s_SquarePosition.y -= s_MoveSpeed; // MOVE DOWN

}

void GameLayer::OnRender() {
    // ACTIVATE OUR SHADER PROGRAM PIPELINE
    m_Shader->Bind();

    // DYNAMIC COLOR TIME MATH
    float time = (float)glfwGetTime();
    // WAVES SMOOTHLY BACK AND FORTH 
    float greenValue = (sin(time) / 2.0f) + 0.5f;
    float redValue = (cos(time) / 2.0f) + 0.5f;
    // PASS THESE CHANGING VALUES INTO UNIFORM 
    m_Shader->SetUniform4f("u_Color", redValue, greenValue, 0.2f, 1.0f);

    // MATRIX ASSEMBLY PAHSE
    glm::mat4 transform = glm::mat4(1.0f);
    // TRANSLATE TRANSORMATION MATRIX BASED ON OUR INPUT VECTOR POSITION
    transform = glm::translate(transform, glm::vec3(s_SquarePosition, 0.0f));

    // SHADER UNIFORM
    m_Shader->SetUniformMat4("u_Transform", transform);

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
