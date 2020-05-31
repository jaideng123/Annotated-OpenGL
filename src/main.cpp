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
#include <map>

#include "shader.h"
#include "model.h"
#include "camera.h"
#include "simple_models.h"

using namespace std;

void processInput(GLFWwindow *window);
void mouse_callback(GLFWwindow *window, double xpos, double ypos);
void framebuffer_size_callback(GLFWwindow *window, int width, int height);
unsigned int loadCubemap(vector<std::string> faces);
void enableFrameBuffer(int frameBuffer);
int generate_screen_texture();
vector<glm::vec3> sortByCameraDistance(vector<glm::vec3> positions, glm::vec3 cameraPosition);

Camera camera = Camera();

float deltaTime = 0.0f; // Time between current frame and last frame
float lastFrame = 0.0f; // Time of last frame

int currentScreenWidth = 800;
int currentScreenHeight = 600;

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

    GLFWwindow *window = glfwCreateWindow(currentScreenWidth, currentScreenHeight, "LearnOpenGL", NULL, NULL);
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

    // We can use a frame buffer to render to a texture and do cool post processing effects
    // A FrameBuffer Requires
    // 1. At least one attached buffer (color, depth or stencil buffer).
    // 2. At least one color attachment.
    // 3. All attachments should be complete (reserved memory).
    // 4. Each buffer should have the same number of samples.
    unsigned int frameBuffer;
    glGenFramebuffers(1, &frameBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);

    int renderTexture = generate_screen_texture();
    // Attach the texture to the currently bound Frame Buffer
    // (Target, Attachment, TextureType, TextureId, MMLevel)
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, renderTexture, 0);

    // Create a render buffer for depth + stencil
    // RenderBuffers Offer a more efficient alternative to textures but don't allow sampling
    unsigned int rbo;
    glGenRenderbuffers(1, &rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, currentScreenWidth, currentScreenHeight);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);

    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
    // Re-Bind the default Frame Buffer
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    std::cout << "Loading Shaders..." << std::endl;
    Shader lampShader = Shader("./shaders/vertex.glsl", "./shaders/fragLamp.glsl");
    Shader lightingShader = Shader("./shaders/vertex.glsl", "./shaders/fragLighting.glsl");
    Shader transparencyShader = Shader("./shaders/vertex.glsl", "./shaders/fragTrans.glsl");
    Shader screenShader = Shader("./shaders/vertScreen.glsl", "./shaders/fragScreen.glsl");
    Shader skyboxShader = Shader("./shaders/vertSkybox.glsl", "./shaders/fragSkybox.glsl");
    Shader reflectiveCubeShader = Shader("./shaders/vertReflect.glsl", "./shaders/fragReflect.glsl");
    Shader refractiveCubeShader = Shader("./shaders/vertReflect.glsl", "./shaders/fragRefract.glsl");

    std::cout
        << "Loading Model..." << std::endl;
    Model nanoSuitModel = Model("./models/nanosuit/nanosuit.obj");
    Texture texture;
    texture.id = TextureFromFile("transparent-window.png", "./textures", false, GL_CLAMP_TO_EDGE);
    texture.type = "texture_diffuse";
    Mesh planeMesh = generate_plane(texture);

    int quadVAO = generateQuadVAO();

    vector<std::string> faces{
        "./textures/skybox/right.jpg",
        "./textures/skybox/left.jpg",
        "./textures/skybox/top.jpg",
        "./textures/skybox/bottom.jpg",
        "./textures/skybox/front.jpg",
        "./textures/skybox/back.jpg"};
    unsigned int cubemapTexture = loadCubemap(faces);
    unsigned int invertedCubemapTexture = loadCubemap(faces);

    int skyboxVao = generate_skybox_vao();

    int cubeVAO = generate_cube_vao();

    // Set size of the rendering window(viewport)
    // (X,Y,Len,Width) from top left corner
    glViewport(0, 0, currentScreenWidth, currentScreenHeight);

    // Callback to re-adjust viewport on window resize
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // Sets color to clear screen with
    //(R,G,B,A)
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);

    // Enable wireframe mode
    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    // Enable Z-buffer test
    // Depth -> Z value is based on a 1/x curve
    glEnable(GL_DEPTH_TEST);
    // Defines which depth test function to use (Default = GL_LESS)
    glDepthFunc(GL_LEQUAL);

    // Enable Stencil Test
    glEnable(GL_STENCIL_TEST);
    // (stencilFail, stencilPassDepthFail, stencilAndDepthPass)
    // GL_KEEP = keep original frag
    // GL_REPLACE = replace original frag w/ new frag
    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

    // Enable Alpha Blending
    glEnable(GL_BLEND);
    // Sets Source and Dest Factors (color = c1(src) + c2(dest))
    // (source,dest)
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    // Blends RGB and A separately
    // glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ZERO);
    // Enable Culling
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    // Defines which winding order to look for(winding order = order of verts in triangle)
    glFrontFace(GL_CCW);

    glm::vec3 pointLightPositions[] = {
        glm::vec3(0.7f, 0.2f, 2.0f),
        glm::vec3(2.3f, -3.3f, -4.0f),
        glm::vec3(-4.0f, 2.0f, -12.0f),
        glm::vec3(0.0f, 0.0f, -3.0f)};

    vector<glm::vec3> windowPositions = {
        glm::vec3(0.0f, 0.0f, 1.0f),
        glm::vec3(0.0f, 0.0f, 3.0f),
        glm::vec3(0.0f, 0.0f, 5.0f),
        glm::vec3(0.0f, 0.0f, 7.0f)};

    std::cout << "Starting Render Loop" << std::endl;
    // Render Loop
    while (!glfwWindowShouldClose(window))
    {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        processInput(window);

        enableFrameBuffer(frameBuffer);

        // Creates a view matrix w/ (pos,target,up) that is looking from pos to target
        glm::mat4 view = camera.GetViewMatrix();

        glm::mat4 projection;
        // perspective(FOV, aspectRatio, nearPlaneDist, farPlaneDist)
        // Near Plane should be as far as possible to avoid z-fighting
        projection = glm::perspective<double>(glm::radians(45.0f), currentScreenWidth / currentScreenHeight, 0.1f, 100.0f);

        // Use lamp shader to render lamp
        lampShader.use();
        glm::vec3 lightColor;
        lightColor.x = sin(glfwGetTime() * 2.0f);
        lightColor.y = sin(glfwGetTime() * 0.7f);
        lightColor.z = sin(glfwGetTime() * 1.3f);
        lampShader.setVec3("color", lightColor);

        lampShader.setMat4("projection", projection);
        lampShader.setMat4("view", view);
        glStencilMask(0x00); // disable writing to the stencil buffer
        for (int i = 0; i < 4; i++)
        {
            glm::vec3 lightPos = pointLightPositions[i];
            glm::mat4 lampModel = glm::mat4(1.0f);
            lampModel = glm::translate(lampModel, lightPos);
            lampModel = glm::scale(lampModel, glm::vec3(0.2f));
            lampShader.setMat4("model", lampModel);
            nanoSuitModel.Draw(lampShader);
        }
        // (function, comparison value, stencil mask)
        glStencilFunc(GL_ALWAYS, 1, 0xFF); // all fragments should pass the stencil test
        glStencilMask(0xFF);               // enable writing to the stencil buffer
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
        lightingShader.setVec3("spotLight.position", camera.Position);
        lightingShader.setVec3("spotLight.ambient", glm::vec3(0.8f) * glm::vec3(0.2f));
        lightingShader.setVec3("spotLight.diffuse", glm::vec3(0.8f));
        lightingShader.setVec3("spotLight.specular", glm::vec3(1.0f, 1.0f, 1.0f));
        lightingShader.setFloat("spotLight.constant", 1.0f);
        lightingShader.setFloat("spotLight.linear", 0.09f);
        lightingShader.setFloat("spotLight.quadratic", 0.032f);
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

        // Draw a Reflective Cube
        reflectiveCubeShader.use();
        reflectiveCubeShader.setMat4("projection", projection);
        reflectiveCubeShader.setMat4("view", view);
        glm::mat4 cubeModel = glm::mat4(1.0f);
        cubeModel = glm::translate(cubeModel, glm::vec3(5, 0, 0));
        reflectiveCubeShader.setMat4("model", cubeModel);
        reflectiveCubeShader.setVec3("cameraPos", camera.Position);
        glDisable(GL_CULL_FACE); // TODO: fix this
        glBindVertexArray(cubeVAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);
        glEnable(GL_CULL_FACE);
        // Instead of using the skybox you can use a dynamically generated cubemap
        // rendered in real-time (or baked) using framebuffers + six camera shots

        // Draw a Refractive Cube
        refractiveCubeShader.use();
        refractiveCubeShader.setMat4("projection", projection);
        refractiveCubeShader.setMat4("view", view);
        glm::mat4 cubeModel2 = glm::mat4(1.0f);
        cubeModel2 = glm::translate(cubeModel2, glm::vec3(-5, 0, 0));
        refractiveCubeShader.setMat4("model", cubeModel2);
        refractiveCubeShader.setVec3("cameraPos", camera.Position);
        glDisable(GL_CULL_FACE); // TODO: fix this
        glBindVertexArray(cubeVAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);
        glEnable(GL_CULL_FACE);

        transparencyShader.use();
        // We don't want culling for our quad windows
        glDisable(GL_CULL_FACE);
        transparencyShader.setVec3("viewPos", camera.Position);
        transparencyShader.setMat4("projection", projection);
        transparencyShader.setMat4("view", view);
        windowPositions = sortByCameraDistance(windowPositions, camera.Position);
        for (size_t i = 0; i < windowPositions.size(); i++)
        {
            model = glm::mat4(1.0f);
            model = glm::scale(model, glm::vec3(1.0f));
            model = glm::translate(model, windowPositions[i]);
            transparencyShader.setMat4("model", model);
            planeMesh.Draw(transparencyShader);
        }
        glEnable(GL_CULL_FACE);

        lampShader.use();
        // TODO: fix the rendering of this (skybox w/ new depth text broke it)
        glStencilFunc(GL_NOTEQUAL, 1, 0xFF); // ignore all stencil values != 1
        glStencilMask(0x00);                 // disable writing to the stencil buffer
        glDisable(GL_DEPTH_TEST);            // ignore depth
        glm::vec3 lightPos = glm::vec3(0.0f);
        glm::mat4 lampModel = glm::mat4(1.0f);
        lampModel = glm::translate(lampModel, lightPos);
        lampModel = glm::scale(lampModel, glm::vec3(0.3f));
        lampShader.setMat4("model", lampModel);
        nanoSuitModel.Draw(lampShader);
        // Reset Stencil Buffer
        glStencilMask(0xFF);
        glStencilFunc(GL_ALWAYS, 1, 0xFF);
        glEnable(GL_DEPTH_TEST);

        // Draw Skybox
        glDepthMask(GL_FALSE);
        skyboxShader.use();
        skyboxShader.setMat4("projection", projection);
        // Skybox is always drawn around camera position
        skyboxShader.setMat4("view", glm::mat4(glm::mat3(camera.GetViewMatrix())));
        glBindVertexArray(skyboxVao);
        glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glDepthMask(GL_TRUE);

        enableFrameBuffer(0);

        screenShader.use();
        glBindVertexArray(quadVAO);
        glDisable(GL_DEPTH_TEST);
        glBindTexture(GL_TEXTURE_2D, renderTexture);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        // Checks for keyboard, mouse, etc.
        glfwPollEvents();
        // Swap pixel color buffers for window
        glfwSwapBuffers(window);
    }

    // Clean up GLFW resources
    glfwTerminate();

    return 0;
}

// Warning: handling transparency like this can break under certain circumstances
// https://www.khronos.org/opengl/wiki/Transparency_Sorting
// Order Independant transparency can solve this (for newer hardware and/or w/ a perf cost)
vector<glm::vec3> sortByCameraDistance(vector<glm::vec3> positions, glm::vec3 cameraPosition)
{
    std::map<float, glm::vec3> sorted;
    for (unsigned int i = 0; i < positions.size(); i++)
    {
        float distance = glm::length(cameraPosition - positions[i]);
        sorted[distance] = positions[i];
    }
    int i = 0;
    for (std::map<float, glm::vec3>::reverse_iterator it = sorted.rbegin(); it != sorted.rend(); ++it)
    {
        positions[i] = it->second,
        ++i;
    }
    return positions;
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

void enableFrameBuffer(int frameBuffer)
{
    glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
}

unsigned int loadCubemap(vector<std::string> faces)
{
    // Don't need to flip textures for the cube map
    stbi_set_flip_vertically_on_load(false);
    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    int width, height, nrChannels;
    for (unsigned int i = 0; i < faces.size(); i++)
    {
        unsigned char *data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
        if (data)
        {
            // Adding i iterates through enum
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                         0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            stbi_image_free(data);
        }
        else
        {
            std::cout << "Cubemap tex failed to load at path: " << faces[i] << std::endl;
            stbi_image_free(data);
        }
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    stbi_set_flip_vertically_on_load(true);

    return textureID;
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

int generate_screen_texture()
{
    unsigned int texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, currentScreenWidth, currentScreenHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    return texture;
}

void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
    glViewport(0, 0, width, height);
    currentScreenHeight = height;
    currentScreenWidth = width;
}
