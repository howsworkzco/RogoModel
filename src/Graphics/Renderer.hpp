#pragma once
#include <glad/glad.h> // OpenGL function loader
#include <GLFW/glfw3.h>
#include "../Core/InstanceManager.hpp"

class RogoRenderer {
private:
    GLFWwindow* m_Window = nullptr;
    GLuint m_CubeVAO = 0;
    GLuint m_CubeVBO = 0;
    GLuint m_InstanceVBO = 0; // Buffer for positions and colors
    GLuint m_ShaderProgram = 0;

    void SetupCubeMesh();
    void SetupShaders();

public:
    RogoRenderer() = default;
    ~RogoRenderer();

    bool Initialize(int width, int height, const char* title);
    void RenderFrame(const InstanceRegistry& registry);
    bool ShouldClose() const;
    void PollEvents();
};
