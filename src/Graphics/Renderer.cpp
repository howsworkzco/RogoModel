#include "Renderer.hpp"
#include <iostream>

// Minimal Vertex and Fragment Shaders for Instanced Bricks
const char* vertexShaderSource = R"(
    #version 330 core
    layout (location = 0) in vec3 aPos;       // Base cube vertex shape
    layout (location = 1) in vec3 aInstancePos;// Position from our flat array
    layout (location = 2) in vec3 aColor;      // Color from our flat array

    out vec3 FragColor;
    
    uniform mat4 view;
    uniform mat4 projection;

    void main() {
        FragColor = aColor;
        // Simple positioning calculation per brick instance
        gl_Position = projection * view * vec4(aPos + aInstancePos, 1.0);
    }
)";

const char* fragmentShaderSource = R"(
    #version 330 core
    in vec3 FragColor;
    out vec4 FinalColor;
    void main() {
        FinalColor = vec4(FragColor, 1.0);
    }
)";

bool RogoRenderer::Initialize(int width, int height, const char* title) {
    if (!glfwInit()) return false;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    m_Window = glfwCreateWindow(width, height, title, nullptr, nullptr);
    if (!m_Window) {
        glfwTerminate();
        return false;
    }

    glfwMakeContextCurrent(m_Window);
    
    // Load OpenGL extensions using GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        return false;
    }

    glEnable(GL_DEPTH_TEST);

    SetupCubeMesh();
    SetupShaders();

    return true;
}

void RogoRenderer::SetupCubeMesh() {
    // 3D coordinates for a simple standard unit block (cube)
    float cubeVertices[] = {
        -0.5f,-0.5f,-0.5f,  0.5f,-0.5f,-0.5f,  0.5f, 0.5f,-0.5f,
         0.5f, 0.5f,-0.5f, -0.5f, 0.5f,-0.5f, -0.5f,-0.5f,-0.5f,
        -0.5f,-0.5f, 0.5f,  0.5f,-0.5f, 0.5f,  0.5f, 0.5f, 0.5f,
         0.5f, 0.5f, 0.5f, -0.5f, 0.5f, 0.5f, -0.5f,-0.5f, 0.5f,
         // ... remaining faces of the cube
    };

    glGenVertexArrays(1, &m_CubeVAO);
    glGenBuffers(1, &m_CubeVBO);
    glGenBuffers(1, &m_InstanceVBO);

    glBindVertexArray(m_CubeVAO);

    // Bind base geometry vertex data
    glBindBuffer(GL_ARRAY_BUFFER, m_CubeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

    // Setup Instance buffer for our custom flat positions
    glBindBuffer(GL_ARRAY_BUFFER, m_InstanceVBO);
    
    // Attribute 1: Instance Positions
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glVertexAttribDivisor(1, 1); // Crucial: Tell GPU this updates per *instance*, not per vertex

    // Attribute 2: Instance Colors
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glVertexAttribDivisor(2, 1); // Updates per instance

    glBindVertexArray(0);
}

void RogoRenderer::RenderFrame(const InstanceRegistry& registry) {
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f); // Polytoria/Roblox style sky blue-grey background
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    const auto& data = registry.GetRawData();
    size_t count = data.instance_ids.size();
    if (count == 0) {
        glfwSwapBuffers(m_Window);
        return;
    }

    // Pack positions and colors into a contiguous array to stream to GPU
    std::vector<float> instanceData;
    instanceData.reserve(count * 6);
    for (size_t i = 0; i (count));
    glBindVertexArray(0);

    glfwSwapBuffers(m_Window);
}

void RogoRenderer::SetupShaders() {
    // Boilerplate code to compile vertexShaderSource & fragmentShaderSource 
    // and link them into m_ShaderProgram goes here...
}

bool RogoRenderer::ShouldClose() const { return glfwWindowShouldClose(m_Window); }
void RogoRenderer::PollEvents() { glfwPollEvents(); }
RogoRenderer::~RogoRenderer() { glfwTerminate(); }
