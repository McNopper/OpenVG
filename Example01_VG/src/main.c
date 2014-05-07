/**
 * OpenVG 1.1 - Example 01
 *
 * @author	Norbert Nopper norbert@nopper.tv
 *
 * Homepage: http://nopper.tv
 *
 * Copyright Norbert Nopper
 */

#include <stdio.h>

#include "VG/glus.h"

static VGint g_width;
static VGint g_height;

/**
 * Function for initialization.
 */
GLUSboolean init(GLUSvoid)
{
	VGfloat clearColor[4] = {0.0f, 0.0f, 1.0f, 1.0f};

    // The background will just cleared with blue color.
	vgSetfv(VG_CLEAR_COLOR, 4, clearColor);

    return GLUS_TRUE;
}

/**
 * Function is called before first update and every time when the window is resized.
 *
 * @param w	width of the window
 * @param h	height of the window
 */
GLUSvoid reshape(GLUSint width, GLUSint height)
{
	g_width = width;
	g_height = height;
}

/**
 * Function to render and display content. Swapping of the buffers is automatically done.
 *
 * @return true for continuing, false to exit the application
 */
GLUSboolean update(GLUSfloat time)
{
    // Now, the background is painted blue.
    vgClear(0, 0, g_width, g_height);

    return GLUS_TRUE;
}

/**
 * Function to clean up things.
 */
GLUSvoid terminate(GLUSvoid)
{
}

/**
 * Main entry point.
 */
int main(int argc, char* argv[])
{
	EGLint eglAttributes[] = {
	        EGL_RED_SIZE, 8,
	        EGL_GREEN_SIZE, 8,
	        EGL_BLUE_SIZE, 8,
	        EGL_RENDERABLE_TYPE, EGL_OPENVG_BIT,
	        EGL_NONE
	};

    glusInitFunc(init);

    glusReshapeFunc(reshape);

    glusUpdateFunc(update);

    glusTerminateFunc(terminate);

    if (!glusCreateWindow("GLUS Example Window", 640, 480, eglAttributes, GLUS_FALSE))
    {
        printf("Could not create window!\n");
        return -1;
    }

    glusRun();

    return 0;
}
