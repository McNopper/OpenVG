/**
 * OpenVG 1.1 - Example 05
 *
 * @author	Norbert Nopper norbert@nopper.tv
 *
 * Homepage: http://nopper.tv
 *
 * Copyright Norbert Nopper
 */

#include <stdio.h>

#include "VG/glus.h"

#include "FreeMono.h"

// Points are stored as short.
#define POINT_RANGE	65536.0f

// Amount of glyphs/letters in the true type font.
#define GLYPH_CAPACITY	256

// Maximum numbers of elements in the output string
#define MAX_INDICES		1024

// Fixed size font.
#define FONT_SCALE 50.0f

static VGint g_width;
static VGint g_height;

//

static VGFont g_font = VG_INVALID_HANDLE;

static VGPaint g_paint = VG_INVALID_HANDLE;

static VGuint g_outputIndices[MAX_INDICES];

static VGint g_outputLength;

static VGfloat g_outputOrigin[2];

static VGfloat g_outputEscapementX;

/**
 * Function for initialization.
 */
GLUSboolean init(GLUSvoid)
{
	VGint i;

	VGfloat glyphOrigin[2];
	VGfloat escapement[2];

	const GLUSchar* output = "Hello OpenVG!";

	//
	// Create the output string.
	//

	g_outputLength = strlen(output);

	if (g_outputLength > MAX_INDICES)
	{
		printf("Error: String too long!\n");

		return GLUS_FALSE;
	}

    g_outputEscapementX = 0.0f;

	for (i = 0; i < g_outputLength; i++)
	{
		if (output[i] >= 0 && output[i] < glus_glyphCount)
		{
			// Map character to generated indices.
			g_outputIndices[i] = (VGuint)glus_characterMap[(VGuint)output[i]];

			// Calculate total length of the string to center the string on the screen.
			g_outputEscapementX += glus_glyphAdvances[(VGuint)output[i]] / POINT_RANGE;
		}
	}

	// Dark grey, opaque background.

	VGfloat clearColor[4] = {0.2f, 0.2f, 0.2f, 1.0f};

    // The background will just cleared with dark, grey color.
	vgSetfv(VG_CLEAR_COLOR, 4, clearColor);

	// White paint for the font.

    g_paint = vgCreatePaint();
    if (g_paint == VG_INVALID_HANDLE)
    {
    	return GLUS_FALSE;
    }
    vgSetParameteri(g_paint, VG_PAINT_TYPE, VG_PAINT_TYPE_COLOR);
    vgSetPaint(g_paint, VG_FILL_PATH);
    // White, opaque color.
    vgSetColor(g_paint, 0xFFFFFFFF);

	//
	// Create the font.
    //

	glyphOrigin[0] = 0.0f;
	glyphOrigin[1] = 0.0f;

	escapement[1] = 0.0f;

	g_font = vgCreateFont(GLYPH_CAPACITY);
    if (g_paint == VG_INVALID_HANDLE)
    {
    	return GLUS_FALSE;
    }

	// Create all glyphs.
	for (i = 0; i < glusMathMinf(glus_glyphCount, GLYPH_CAPACITY); i++)
	{
        VGPath path = vgCreatePath(VG_PATH_FORMAT_STANDARD, VG_PATH_DATATYPE_S_32, 1.0f / POINT_RANGE, 0.0f, 0, 0, VG_PATH_CAPABILITY_ALL);
        if (path == VG_INVALID_HANDLE)
        {
        	return GLUS_FALSE;
        }

        escapement[0] = glus_glyphAdvances[i] / POINT_RANGE;

        vgAppendPathData(path, glus_glyphInstructionCounts[i], &glus_glyphInstructions[glus_glyphInstructionIndices[i]], &glus_glyphPoints[glus_glyphPointIndices[i] * 2]);

        vgSetGlyphToPath(g_font, i, path, VG_TRUE, glyphOrigin, escapement);

        vgDestroyPath(path);
	}

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

	// Calculate origin to center on the screen.
	g_outputOrigin[0] = (VGfloat)g_width / 2.0f - g_outputEscapementX * 0.5 * FONT_SCALE;
	g_outputOrigin[1] = (VGfloat)g_height / 2.0f;
}

/**
 * Function to render and display content. Swapping of the buffers is automatically done.
 *
 * @return true for continuing, false to exit the application
 */
GLUSboolean update(GLUSfloat time)
{
	static VGfloat glyphOrigin[2] = { 0.0f, 0.0f };

    // Now, the background is painted with dark grey.
    vgClear(0, 0, g_width, g_height);

    // Set up matrix and parameters for the font ...
    vgSeti(VG_MATRIX_MODE, VG_MATRIX_GLYPH_USER_TO_SURFACE);

    vgLoadIdentity();

    vgTranslate(g_outputOrigin[0], g_outputOrigin[1]);

    vgScale(FONT_SCALE, FONT_SCALE);

    vgSetfv(VG_GLYPH_ORIGIN, 2, glyphOrigin);

	// ... and render it.
	vgDrawGlyphs(g_font, g_outputLength, g_outputIndices, 0, 0, VG_FILL_PATH, VG_TRUE);

    return GLUS_TRUE;
}

/**
 * Function to clean up things.
 */
GLUSvoid terminate(GLUSvoid)
{
	VGint i;

	for (i = 0; i < glusMathMinf(glus_glyphCount, GLYPH_CAPACITY); i++)
	{
		if (g_font != VG_INVALID_HANDLE)
		{
			vgClearGlyph(g_font, i);
		}
	}

	if (g_font != VG_INVALID_HANDLE)
	{
		vgDestroyFont(g_font);

		g_font = VG_INVALID_HANDLE;
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
	EGLint eglConfigAttributes[] = {
	        EGL_RED_SIZE, 8,
	        EGL_GREEN_SIZE, 8,
	        EGL_BLUE_SIZE, 8,
			EGL_ALPHA_SIZE, 8,
	        EGL_RENDERABLE_TYPE, EGL_OPENVG_BIT,
	        EGL_NONE
	};

    EGLint eglContextAttributes[] = {
    		EGL_NONE
    };

    glusWindowSetInitFunc(init);

    glusWindowSetReshapeFunc(reshape);

    glusWindowSetUpdateFunc(update);

    glusWindowSetTerminateFunc(terminate);

    if (!glusWindowCreate("GLUS Example Window", 640, 480, GLUS_FALSE, GLUS_FALSE, eglConfigAttributes, eglContextAttributes))
    {
        printf("Could not create window!\n");
        return -1;
    }

    glusWindowRun();

    return 0;
}
