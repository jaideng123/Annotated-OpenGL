
#include <GLFW/glfw3.h>
#include <iostream>
using namespace std;

void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);
}

int main()
{
    cout << "A nondescript C++ program using the GLFW library, coded and built using VSCode" << endl;
    cout << "Initializing Graphics Library FramWork (GLFW)" << endl;

    if (!glfwInit())
    {
        return -1;
    }

    auto gameWindow = glfwCreateWindow(640, 480, "GLFW Test App", NULL, NULL);
    if (!gameWindow)
    {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(gameWindow);
    glfwSetKeyCallback(gameWindow, key_callback);
    float currentRotationX = 0.f;
    float previousTime = 0.f;
    // Main loop
    while (!glfwWindowShouldClose(gameWindow))
    {
        float ratio;
        int width, height;
        glfwGetFramebufferSize(gameWindow, &width, &height);
        ratio = width / (float)height;
        glViewport(0, 0, width, height);
        glClear(GL_COLOR_BUFFER_BIT);
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glOrtho(-ratio, ratio, -1.f, 1.f, 1.f, -1.f);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        float time = (float)glfwGetTime();
        float deltaTime = time - previousTime;
        previousTime = time;

        if (glfwGetKey(gameWindow, GLFW_KEY_LEFT))
        {
            currentRotationX += deltaTime * -50.f;
        }
        else if (glfwGetKey(gameWindow, GLFW_KEY_RIGHT))
        {
            currentRotationX += deltaTime * 50.f;
        }

        glRotatef(currentRotationX, 0.f, 0.f, 1.f);
        glBegin(GL_TRIANGLES);
        glColor3f(1.f, 0.f, 0.f);
        glVertex3f(-0.6f, -0.4f, 0.f);
        glColor3f(0.f, 1.f, 0.f);
        glVertex3f(0.6f, -0.4f, 0.f);
        glColor3f(0.f, 0.f, 1.f);
        glVertex3f(0.f, 0.7f, 0.f);
        glEnd();
        glfwSwapBuffers(gameWindow);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
