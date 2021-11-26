#ifndef GRAPHICS_H_
#define GRAPHICS_H_

#include <glad/gl.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include "linmath.h"
#include <stdlib.h>
#include <stdio.h>
#include "constants.h"


#include <complex>
#include <fftw3.h>

#define BUFFER_SIZE (FRAMES_PER_BUFFER)

#include "windows.h"

enum DisplayMode {
    DISPLAY_MODE_OSCILLOSCOPE,
    DISPLAY_MODE_METER,
    DISPLAY_MODE_FFT
};

DisplayMode g_mode = DISPLAY_MODE_OSCILLOSCOPE;

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



// GL global variables
GLint g_buffer_size = BUFFER_SIZE;
float g_buffer[BUFFER_SIZE];
float g_window[BUFFER_SIZE];

GLboolean g_ready = true;


void drawWindowedTimeDomain(float* buffer) {
	glMatrixMode(GL_PROJECTION);
	// load the identity matrix
	glLoadIdentity();
	glOrtho(0, 256.0f, -1.0f, 1.0f, -1.0f, 1.0f);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

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


// changes time sequecies (real value ) to power spectral values.
// modified from:
// https://gist.github.com/sigidagi/576988
void  fftw(float* x)
{

    // buffer x's values will be modified in place 
    fftw_complex* in, * out;
    fftw_plan p;

    int N = 256; // N samples 
    int nchans = 1; // nchans channels

    in = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * N);
    out = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * N);
    p = fftw_plan_dft_1d(N, in, out, FFTW_FORWARD, FFTW_ESTIMATE);

    int i;
    std::complex<double> temp;

    for (int j = 0; j < nchans; ++j)
    {
        for (i = 0; i < N; ++i)
        {
            in[i][0] = x[i];
            in[i][1] = 0.0;
        }

        fftw_execute(p); /* repeat as needed */

        for (i = 0; i < N; ++i)
        {
            temp = std::complex<double>(out[i][0], out[i][1]);
            float db = 20*log10(std::abs(std::pow(temp, 2)));
            x[i] = db;
        }
    }

    fftw_destroy_plan(p);
    fftw_free(in); fftw_free(out);
}

void drawFFT(float* buffer)
{
	glMatrixMode(GL_PROJECTION);
	// load the identity matrix
	glLoadIdentity();
	glOrtho(0, 256.0f, -100.f, 0.0f, -2.0f, 2.0f);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();


    {
        // Blue Color
        glColor3f(0, 1.0, 1.0);


        // perform fft in place
        fftw(buffer);
        for (int i = 0; i < 64; i++) {
			glPushMatrix();
            glBegin(GL_LINE_STRIP);
            glColor3f(0.5, 0.7, 1.f);
            glVertex3f(i*4, buffer[i], 0);
        }
		glEnd();
		glPopMatrix();
    }

}

void drawVUMeter(float* buffer) {
	glMatrixMode(GL_PROJECTION);
	// load the identity matrix
	glLoadIdentity();
	glOrtho(0, 256.0f, -100.f, 0.0f, -2.0f, 2.0f);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

    float rms = 0.f;

    glPushMatrix();
	{
		glBegin(GL_LINE_STRIP);

  		// calculate rms dB 
		float accum = 0.f;
		for (int i = 0; i < 256; i++) {
			accum += buffer[i] * buffer[i];
		}

		rms = 20 * log10(sqrt(accum / 256));

		glEnd();
	}

	// color scale
    float colorScale = rms / -100.f;
    glColor3f(colorScale, 0.7, 1.f);
	glRectf(85.f, -100.f, 170.f, rms);
    glPopMatrix();
}

static void error_callback(int error, const char* description)
{
    fprintf(stderr, "Error: %s\n", description);
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }
    if (key == GLFW_KEY_1 && action == GLFW_PRESS) {
        g_mode = DISPLAY_MODE_OSCILLOSCOPE;
    }
    if (key == GLFW_KEY_2 && action == GLFW_PRESS) {
        g_mode = DISPLAY_MODE_METER;

    }
    if (key == GLFW_KEY_3 && action == GLFW_PRESS) {
        g_mode = DISPLAY_MODE_FFT;

    }


}

static void initWindow(int width, int height)
{
    glMatrixMode(GL_PROJECTION);
    // load the identity matrix
    glLoadIdentity();
    glViewport(0, 0, width, height);
    // load the identity matrix
    glOrtho(0, 256.0f, -1.0f, 1.0f, -1.0f, 1.0f);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

static int graphicsThread(void)
{
    GLFWwindow* window;
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


    glEnable(GL_POINT_SMOOTH);
    glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);

    int width = 0;
    int height = 0;
	glfwGetFramebufferSize(window, &width, &height);
	auto ratio = width / (float)height;

	glUseProgram(program);

    while (!glfwWindowShouldClose(window))
    {
        while (!g_ready) {
            usleep(3000);
        }

        initWindow(width, height);

        g_ready = false;

        float buffer[256];
        memcpy(buffer, g_buffer, g_buffer_size * sizeof(float));

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // choose display mode
        if (g_mode == DISPLAY_MODE_OSCILLOSCOPE) {
            drawWindowedTimeDomain(buffer);
        }
        if (g_mode == DISPLAY_MODE_METER) {
            drawVUMeter(buffer);
        }
        if (g_mode == DISPLAY_MODE_FFT) {
            drawFFT(buffer);
        }

        glFlush();
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);

    glfwTerminate();
    exit(EXIT_SUCCESS);
}


#endif // GRAPHICS_H_
