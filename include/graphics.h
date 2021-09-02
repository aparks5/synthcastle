#ifndef GRAPHICS_H_
#define GRAPHICS_H_

#include <glad/gl.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include "linmath.h"
#include <stdlib.h>
#include <stdio.h>
#include <windows.h>

void usleep(__int64 usec)
{
    HANDLE timer;
    LARGE_INTEGER ft;

    ft.QuadPart = -(10 * usec); // Convert to 100 nanosecond interval, negative value indicates relative time

    timer = CreateWaitableTimer(NULL, TRUE, NULL);
    SetWaitableTimer(timer, &ft, 0, NULL, NULL, 0);
    WaitForSingleObject(timer, INFINITE);
    CloseHandle(timer);
}

#define BUFFER_SIZE (256)

// GL global variables
GLint g_buffer_size = BUFFER_SIZE;
float g_buffer[BUFFER_SIZE];
float g_window[BUFFER_SIZE];

GLboolean g_ready = true;


void drawWindowedTimeDomain(float* buffer) {
    // Initialize initial x

    glPushMatrix();
    {
        // Blue Color
        glColor3f(0, 1.0, 1.0);

        glBegin(GL_LINE_STRIP);

        // Draw Windowed Time Domain
        for (int i = 0; i < 256; i++) {
            glVertex3f(i, buffer[i], 0);
        }

        glEnd();

    }
    glPopMatrix();
}
static void error_callback(int error, const char* description)
{
    fprintf(stderr, "Error: %s\n", description);
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);
}

static int graphicsThread(void)
{
    GLFWwindow* window;

    // wait for data
    

    glfwSetErrorCallback(error_callback);

    if (!glfwInit())
        exit(EXIT_FAILURE);

    window = glfwCreateWindow(640, 480, "Scope", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    glfwSetKeyCallback(window, key_callback);

    glfwMakeContextCurrent(window);
    gladLoadGL(glfwGetProcAddress);
    glfwSwapInterval(1);

    auto program = glCreateProgram();
    glLinkProgram(program);

	int width, height;


    glEnable(GL_POINT_SMOOTH);
    glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);

	glfwGetFramebufferSize(window, &width, &height);

	auto ratio = width / (float)height;

	glUseProgram(program);

    // save the new window size
    // map the view port to the client area
    // set the matrix mode to project


    while (!glfwWindowShouldClose(window))
    {

        while (!g_ready) {
            usleep(3000);
        }

        g_ready = false;
		glMatrixMode(GL_PROJECTION);
		// load the identity matrix
		glLoadIdentity();
        glViewport(0, 0, width, height);
		// load the identity matrix
        glOrtho(0, 256.0f, -1.0f, 1.0f, -1.0f, 1.0f);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

        // local variables
        float buffer[256];
        memcpy(buffer, g_buffer, g_buffer_size * sizeof(float));
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		drawWindowedTimeDomain(buffer);
        glFlush();
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);

    glfwTerminate();
    exit(EXIT_SUCCESS);
}

#endif // GRAPHICS_H_
