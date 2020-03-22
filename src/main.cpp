// Created based on https://learnopengl.com/Getting-started
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <iostream>
#include <string>
#include <fstream>
#include <streambuf>
#include <vector>

#include "shader.h"
#include "model.h"
#include "camera.h"

using namespace std;

void processInput(GLFWwindow *window);
void mouse_callback(GLFWwindow *window, double xpos, double ypos);
void framebuffer_size_callback(GLFWwindow *window, int width, int height);
int generateModelVAO(vector<float> vertices, vector<unsigned int> indices);
int generateLightVAO(vector<float> vertices, vector<unsigned int> indices);
unsigned int loadTexture(char const *path);

Camera camera = Camera();

float deltaTime = 0.0f; // Time between current frame and last frame
float lastFrame = 0.0f; // Time of last frame

// OpenGL acts as a state machine
int main()
{
    std::cout << "Starting..." << std::endl;
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
    std::cout << "Loading Shaders..." << std::endl;
    Shader lampShader = Shader("./shaders/vertex.glsl", "./shaders/fragLamp.glsl");
    Shader lightingShader = Shader("./shaders/vertex.glsl", "./shaders/fragLighting.glsl");

    std::cout << "Loading Model..." << std::endl;
    Model nanoSuitModel = Model("./models/nanosuit/nanosuit.obj");

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

    // Enable Z-buffer test
    glEnable(GL_DEPTH_TEST);

    glm::vec3 pointLightPositions[] = {
        glm::vec3(0.7f, 0.2f, 2.0f),
        glm::vec3(2.3f, -3.3f, -4.0f),
        glm::vec3(-4.0f, 2.0f, -12.0f),
        glm::vec3(0.0f, 0.0f, -3.0f)};

    std::cout << "Starting Render Loop" << std::endl;
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

        // Use lamp shader to render lamp
        lampShader.use();
        glm::vec3 lightColor;
        lightColor.x = sin(glfwGetTime() * 2.0f);
        lightColor.y = sin(glfwGetTime() * 0.7f);
        lightColor.z = sin(glfwGetTime() * 1.3f);
        lampShader.setVec3("color", lightColor);

        lampShader.setMat4("projection", projection);
        lampShader.setMat4("view", view);
        for (int i = 0; i < 4; i++)
        {
            glm::vec3 lightPos = pointLightPositions[i];
            glm::mat4 lampModel = glm::mat4(1.0f);
            lampModel = glm::translate(lampModel, lightPos);
            lampModel = glm::scale(lampModel, glm::vec3(0.2f));
            lampShader.setMat4("model", lampModel);
            nanoSuitModel.Draw(lampShader);
        }

        // use our lighting shader program to render an object with light
        lightingShader.use();
        lightingShader.setVec3("viewPos", camera.Position);
        lightingShader.setMat4("projection", projection);
        lightingShader.setMat4("view", view);
        // Set Light Properties
        lightingShader.setVec3("dirLight.direction", glm::vec3(-0.2f, -1.0f, -0.3f));

        glm::vec3 diffuseColor = glm::vec3(0.3f);
        glm::vec3 ambientColor = diffuseColor * glm::vec3(0.2f);

        // Setup Directional Light
        lightingShader.setVec3("dirLight.ambient", ambientColor);
        lightingShader.setVec3("dirLight.diffuse", diffuseColor);
        lightingShader.setVec3("dirLight.specular", glm::vec3(1.0f, 1.0f, 1.0f));

        // Setup Point Lights
        for (int i = 0; i < 4; i++)
        {
            string index = "[" + to_string(i) + "]";
            lightingShader.setVec3("pointLights" + index + ".position", pointLightPositions[i]);
            lightingShader.setVec3("pointLights" + index + ".ambient", ambientColor);
            lightingShader.setVec3("pointLights" + index + ".diffuse", lightColor);
            lightingShader.setVec3("pointLights" + index + ".specular", glm::vec3(1.0f, 1.0f, 1.0f));
            lightingShader.setFloat("pointLights" + index + ".constant", 1.0f);
            lightingShader.setFloat("pointLights" + index + ".linear", 0.09f);
            lightingShader.setFloat("pointLights" + index + ".quadratic", 0.032f);
        }

        // Setup Spot Light
        lightingShader.setVec3("spotLight.ambient", glm::vec3(0.8f) * glm::vec3(0.2f));
        lightingShader.setVec3("spotLight.diffuse", glm::vec3(0.8f));
        lightingShader.setVec3("spotLight.specular", glm::vec3(1.0f, 1.0f, 1.0f));
        lightingShader.setFloat("spotLight.constant", 1.0f);
        lightingShader.setFloat("spotLight.linear", 0.09f);
        lightingShader.setFloat("spotLight.quadratic", 0.032f);
        lightingShader.setVec3("spotLight.position", camera.Position);
        lightingShader.setVec3("spotLight.direction", camera.Front);
        lightingShader.setFloat("spotLight.cutOff", glm::cos(glm::radians(12.5f)));
        lightingShader.setFloat("spotLight.outerCutOff", glm::cos(glm::radians(17.5f)));

        glm::mat4 model = glm::mat4(1.0f);
        model = glm::scale(model, glm::vec3(.2f));
        model = glm::translate(model, glm::vec3(0, 0, 0));
        lightingShader.setMat4("model", model);

        // We can set a struct member using <struct>.member
        // set Material Properties
        lightingShader.setInt("material.emission", 2);
        lightingShader.setFloat("material.shininess", 32.0f);

        nanoSuitModel.Draw(lightingShader);

        // Checks for keyboard, mouse, etc.
        glfwPollEvents();
        // Swap pixel color buffers for window
        glfwSwapBuffers(window);
    }

    // Clean up GLFW resources
    glfwTerminate();

    return 0;
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
bool firstMouse = true;
// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow *window, double xpos, double ypos)
{
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
    glViewport(0, 0, width, height);
}
