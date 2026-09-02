#include "GameLayer.h"
#include "core/Log.h"
#include <glad/glad.h>
#include <glfw/glfw3.h>
#include <cmath>

GameLayer::GameLayer() : Layer("GameSandboxLayer") {}

void GameLayer::OnAttach() {
    ENGINE_INFO("GameLayer Attached! Reading assets from external GLSL Files...");

    // HARDCODED 2D TRIANGLE POSITIONS X Y Z
    float vertices[] = {
        -0.5f, -0.5f, 0.0f, // Bottom Left
         0.5f, -0.5f, 0.0f, // Bottom Right
         0.0f,  0.5f, 0.0f  // Top Center
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
}

void GameLayer::OnUpdate() {
    // GAME LOGIC GOES HERE 
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

    // BIND YOUR VERTEXARRAY ABSTRACTION OBJECT
    m_VertexArray->Bind();

    // ISSUE THE DRAW CALL TO THE HARDWARE
    glDrawArrays(GL_TRIANGLES, 0, 3);

    // UNBIND CLEANLY
    m_VertexArray->Unbind();
    m_Shader->Unbind();
}

void GameLayer::OnDetach() {
    ENGINE_WARN("GameLayer Detached. Cleaning up GPU memory.");
}
