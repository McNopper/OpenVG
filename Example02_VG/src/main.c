/**
 * OpenVG 1.1 - Example 02
 *
 * @author	Norbert Nopper norbert@nopper.tv
 *
 * Homepage: http://nopper.tv
 *
 * Copyright Norbert Nopper
 */

#include <stdio.h>

#include "VG11/glus.h"

static VGint g_width;
static VGint g_height;

static VGPaint g_paint = VG_INVALID_HANDLE;

static VGPath g_trianglePath = VG_INVALID_HANDLE;

/**
 * Function for initialization.
 */
GLUSboolean init(GLUSvoid)
{
	VGfloat clearColor[4] = {0.0f, 0.0f, 0.0f, 1.0f};


	VGubyte trianglePathSegments[4] = {
			VG_MOVE_TO_ABS,
			VG_LINE_TO_ABS,
			VG_LINE_TO_ABS,
			VG_CLOSE_PATH
	};

	VGfloat trianglePoints[3*2] = {
			0.25f, 0.5f,
			0.75f, 0.5f,
			0.5f, 0.75f
	};


    // The background will just cleared with black color.
	vgSetfv(VG_CLEAR_COLOR, 4, clearColor);


	// Create a paint object.
	g_paint = vgCreatePaint();
	// Plain color.
	vgSetParameteri(g_paint, VG_PAINT_TYPE, VG_PAINT_TYPE_COLOR);
	// Opaque red.
	vgSetColor(g_paint, 0xFF0000FF);
	// Filled.
	vgSetPaint(g_paint, VG_FILL_PATH);


	// Create path object for a triangle. See OpenVG 1.1 Specification p80ff
	g_trianglePath = vgCreatePath(VG_PATH_FORMAT_STANDARD, VG_PATH_DATATYPE_F, 1.0f, 0.0f, 4, 3, VG_PATH_CAPABILITY_APPEND_TO);
	// Append the path data.
	vgAppendPathData(g_trianglePath, 4, trianglePathSegments, trianglePoints);
	// Finalize the path, as no more data will be added.
	vgRemovePathCapabilities(g_trianglePath, VG_PATH_CAPABILITY_APPEND_TO);

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
    // Now, the background is painted black.
    vgClear(0, 0, g_width, g_height);


	// Scale to the dimension of the window. Triangle will be stretched when window is resized.
	vgLoadIdentity();
	vgScale((VGfloat)g_width, (VGfloat)g_height);

	// Draw the red triangle.
	vgDrawPath(g_trianglePath, VG_FILL_PATH);

    return GLUS_TRUE;
}

/**
 * Function to clean up things.
 */
GLUSvoid terminate(GLUSvoid)
{
	if (g_trianglePath != VG_INVALID_HANDLE)
	{
		vgDestroyPath(g_trianglePath);

		g_trianglePath = VG_INVALID_HANDLE;
	}

	if (g_paint != VG_INVALID_HANDLE)
	{
		vgDestroyPaint(g_paint);

		g_paint = VG_INVALID_HANDLE;
	}
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
