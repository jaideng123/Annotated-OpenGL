// Created based on https://learnopengl.com/Getting-started
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <string>
#include <fstream>
#include <streambuf>
#include <vector>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "shader.h"
#include "camera.h"

using namespace std;

void processInput(GLFWwindow *window);
void mouse_callback(GLFWwindow *window, double xpos, double ypos);
void framebuffer_size_callback(GLFWwindow *window, int width, int height);
int generateVAO(vector<float> vertices, vector<unsigned int> indices);

Camera camera = Camera();

float deltaTime = 0.0f; // Time between current frame and last frame
float lastFrame = 0.0f; // Time of last frame

// OpenGL acts as a state machine
int main()
{
    stbi_set_flip_vertically_on_load(true);
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

    // hide + capture cursor
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    // get cursor input
    glfwSetCursorPosCallback(window, mouse_callback);

    // Load GLAD (GL calls will only work after this)
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    Shader shader = Shader("./shaders/vertex.glsl", "./shaders/frag.glsl");

    unsigned int texture;
    // (numTextures, out ID)
    glGenTextures(1, &texture);
    // Sets active texture for subsequent commands

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);
    // set the texture wrapping/filtering options (on the currently bound texture object)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    int width, height, nrChannels;
    unsigned char *data = stbi_load("./textures/container.jpg", &width, &height, &nrChannels, 0);
    if (!data)
    {
        throw runtime_error("Failed to load texture");
    }
    // Load data for texture (textureTarget, mipmapLevel, storageFormat, w, h, 0, imageFormat, imageDataType, dataPtr)
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D); // Auto-Generate MipMaps (can be set manually instead)

    stbi_image_free(data);

    unsigned int texture2;
    // (numTextures, out ID)
    glGenTextures(1, &texture2);
    // Sets active texture for subsequent commands

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, texture2);
    // set the texture wrapping/filtering options (on the currently bound texture object)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    data = stbi_load("./textures/awesomeface.png", &width, &height, &nrChannels, 0);
    if (!data)
    {
        throw runtime_error("Failed to load texture");
    }
    // Load data for texture (textureTarget, mipmapLevel, storageFormat, w, h, 0, imageFormat, imageDataType, dataPtr)
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D); // Auto-Generate MipMaps (can be set manually instead)

    stbi_image_free(data);

    // Defined in Normalized Device Coordinates (between -1 and 1)
    // Eventually tranformed into screenspace via glViewport
    vector<float> vertices = {
        // positions          // colors           // texture coords
        0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f,   // top right
        0.5f, -0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,  // bottom right
        -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, // bottom left
        -0.5f, 0.5f, 0.5f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f,  // top left

        0.5f, 0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f,   // top right
        0.5f, -0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,  // bottom right
        -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, // bottom left
        -0.5f, 0.5f, -0.5f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f   // top left
    };
    vector<unsigned int> indices = {
        0, 1, 3, //Front
        1, 2, 3,

        4, 5, 7, //Back
        5, 7, 6,

        0, 5, 1, // Right
        4, 5, 0,

        7, 0, 3, // Top
        7, 4, 0,

        7, 2, 6, // Left
        2, 3, 7,

        2, 6, 1, // Bottom
        1, 5, 6

    };

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

    // use our shader program when we want to render an object
    shader.use();
    shader.setInt("texture1", 0);
    shader.setInt("texture2", 1);

    // Enable Z-buffer test
    glEnable(GL_DEPTH_TEST);

    // Render Loop
    while (!glfwWindowShouldClose(window))
    {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        processInput(window);

        // Clears Color buffer
        // The possible bits for glClear() are:
        // GL_COLOR_BUFFER_BIT, GL_DEPTH_BUFFER_BIT and GL_STENCIL_BUFFER_BIT
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Creates a view matrix w/ (pos,target,up) that is looking from pos to target
        glm::mat4 view = camera.GetViewMatrix();

        glm::mat4 projection;
        // perspective(FOV, aspectRatio, nearPlaneDist, farPlaneDist)
        projection = glm::perspective<double>(glm::radians(45.0f), 800.0 / 600.0, 0.1f, 100.0f);

        // Selects Texture unit for subsequent bindTexture call
        glBindVertexArray(VAO1);
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0, 0, -3.0f));
        model = glm::rotate(model, (float)glfwGetTime() * glm::radians(50.0f), glm::vec3(0.5f, 1.0f, 0.0f));

        shader.setMat4("projection", projection);
        shader.setMat4("view", view);
        shader.setMat4("model", model);

        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
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
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    return VAO;
}

void processInput(GLFWwindow *window)
{
    float cameraSpeed = 2.5f * deltaTime;
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
}

float lastX = 400, lastY = 300;
void mouse_callback(GLFWwindow *window, double xpos, double ypos)
{
    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates range from bottom to top
    lastX = xpos;
    lastY = ypos;
    camera.ProcessMouseMovement(xoffset, yoffset, true);
}

void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
    glViewport(0, 0, width, height);
}
