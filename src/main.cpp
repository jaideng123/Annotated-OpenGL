// Created based on https://learnopengl.com/Getting-started
#include <GLFW/glfw3.h>
#include <iostream>
using namespace std;
void processInput(GLFWwindow *window);
void framebuffer_size_callback(GLFWwindow *window, int width, int height);
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
    // Set size of the rendering window(viewport)
    // (X,Y,Len,Width) from top left corner
    glViewport(0, 0, 800, 600);

    // Callback to re-adjust viewport on window resize
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // Sets color to clear screen with
    //(R,G,B,A)
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);

    // Render Loop
    while (!glfwWindowShouldClose(window))
    {
        processInput(window);

        // Clears Color buffer
        // The possible bits for glClear() are:
        // GL_COLOR_BUFFER_BIT, GL_DEPTH_BUFFER_BIT and GL_STENCIL_BUFFER_BIT
        glClear(GL_COLOR_BUFFER_BIT);

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
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
    glViewport(0, 0, width, height);
}
