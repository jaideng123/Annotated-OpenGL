// Created based on https://learnopengl.com/Getting-started
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <string>
#include <fstream>
#include <streambuf>
#include <vector>
using namespace std;
void processInput(GLFWwindow *window);
void framebuffer_size_callback(GLFWwindow *window, int width, int height);
string readFileContents(string filename);
void checkSuccessfulShaderCompilation(int shaderId);
int generateAndCompileShader(string sourceFileLocation, int shaderType);
void checkSuccessfulShaderLink(int shaderId);
int generateVAO(vector<float> vertices, vector<unsigned int> indices);

// OpenGL acts as a state machine
int main()
{
    glfwInit();
    // Set to OpenGL 3.3
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow *window = glfwCreateWindow(800, 600, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    // Load GLAD (GL calls will only work after this)
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    int vertexShader = generateAndCompileShader("./shaders/vertex.glsl", GL_VERTEX_SHADER);

    checkSuccessfulShaderCompilation(vertexShader);

    int fragmentShader = generateAndCompileShader("./shaders/frag.glsl", GL_FRAGMENT_SHADER);

    checkSuccessfulShaderCompilation(fragmentShader);

    unsigned int shaderProgram;
    shaderProgram = glCreateProgram();

    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    checkSuccessfulShaderLink(shaderProgram);

    // Need to clean up shader object no that we no longer use them
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // Defined in Normalized Device Coordinates (between -1 and 1)
    // Eventually tranformed into screenspace via glViewport
    vector<float> vertices = {
        0.0f, 0.5f, 0.0f, // top
        0.5f, 0.0f, 0.0f, // right
        -0.5f, 0.0f, 0.0f // left
    };

    vector<unsigned int> indices = {0, 1, 2};

    int VAO1 = generateVAO(vertices, indices);
    // Set size of the rendering window(viewport)
    // (X,Y,Len,Width) from top left corner
    glViewport(0, 0, 800, 600);

    // Callback to re-adjust viewport on window resize
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // Sets color to clear screen with
    //(R,G,B,A)
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);

    // Enable wireframe mode
    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    // Render Loop
    while (!glfwWindowShouldClose(window))
    {
        processInput(window);

        // Clears Color buffer
        // The possible bits for glClear() are:
        // GL_COLOR_BUFFER_BIT, GL_DEPTH_BUFFER_BIT and GL_STENCIL_BUFFER_BIT
        glClear(GL_COLOR_BUFFER_BIT);

        // use our shader program when we want to render an object
        glUseProgram(shaderProgram);
        glBindVertexArray(VAO1);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0); // Unbind vertex array

        // Checks for keyboard, mouse, etc.
        glfwPollEvents();
        // Swap pixel color buffers for window
        glfwSwapBuffers(window);
    }

    // Clean up GLFW resources
    glfwTerminate();

    return 0;
}

int generateVAO(vector<float> vertices, vector<unsigned int> indices)
{
    // Similar to VBO stores:
    // - Calls to glEnableVertexAttribArray or glDisableVertexAttribArray.
    // - Vertex attribute configurations via glVertexAttribPointer
    // - vertex buffer objects associated with vertex attributes by calls to glVertexAttribPointer
    unsigned int VAO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    // 0. copy our vertices array in a buffer for OpenGL to use
    // Generate 1 Buffer binding point and assign ID to VBO
    // VBO stores large # of vertices in GPU memory
    unsigned int VBO;
    glGenBuffers(1, &VBO);
    // Bind ID to a new buffer object with GL_BUFFER_ARRAY buffer type
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    // Copy vertex data into buffer
    // GL_STATIC_DRAW: the data will most likely not change at all or very rarely.
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), &vertices[0], GL_STATIC_DRAW);

    unsigned int EBO;
    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

    // 1. then set the vertex attributes pointers
    // Tells opengl how to interpret vertex data
    // (location(in shader), size of vertex, datatype, normalized, stride(length of vertex data), offset)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
    // Enable array in shader at location 0
    glEnableVertexAttribArray(0);

    return VAO;
}

string readFileContents(string filename)
{
    std::ifstream filestream(filename);
    std::string output((std::istreambuf_iterator<char>(filestream)),
                       std::istreambuf_iterator<char>());
    return output;
}

int generateAndCompileShader(string sourceFileLocation, int shaderType)
{
    unsigned int shaderId;
    // Create Shader Object
    shaderId = glCreateShader(shaderType);

    // Read source code into a C string
    const char *vertexShaderSource = readFileContents(sourceFileLocation).c_str();
    // Read source code into shader object
    glShaderSource(shaderId, 1, &vertexShaderSource, NULL);

    // Compile shader
    glCompileShader(shaderId);

    return shaderId;
}

void checkSuccessfulShaderCompilation(int shaderId)
{
    // Check if shader compilation was successful
    int success;
    char infoLog[512];
    glGetShaderiv(shaderId, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(shaderId, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n"
                  << infoLog << std::endl;
        throw runtime_error("Shader Compilation Unsuccessful");
    }
}

void checkSuccessfulShaderLink(int shaderProgramId)
{
    // Check if shader compilation was successful
    int success;
    char infoLog[512];
    glGetProgramiv(shaderProgramId, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(shaderProgramId, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINK_FAILED\n"
                  << infoLog << std::endl;
        throw runtime_error("Shader Linking Unsuccessful");
    }
}

void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
    glViewport(0, 0, width, height);
}
