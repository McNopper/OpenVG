/**
 * OpenVG 1.1 - Example 04
 *
 * @author	Norbert Nopper norbert@nopper.tv
 *
 * Homepage: http://nopper.tv
 *
 * Copyright Norbert Nopper
 */

#include <stdio.h>

#include "VG/glus.h"

// Amount of glyphs/letters in the bitmap font.
#define GLYPH_CAPACITY	256

// Maximum numbers of elements in the output string
#define MAX_INDICES		1024

static VGint g_width;
static VGint g_height;

//

static VGFont g_font = VG_INVALID_HANDLE;

static VGImage g_images[GLYPH_CAPACITY];

static VGuint g_outputIndices[MAX_INDICES];

static VGint g_outputLength;

static VGfloat g_outputOrigin[2];

static VGfloat g_outputEscapementX;

/**
 * Function for initialization.
 */
GLUSboolean init(GLUSvoid)
{
	VGint i, x, y;

	GLUStgaimage tgaimage;

	VGint numberColorChannels, cellWidth, cellHeight, fontWidth, fontHeight, numberCellColumns;

	VGfloat glyphOrigin[2];
	VGfloat escapement[2];

	const GLUSchar* output = "Hello OpenVG!";

	//

	g_outputLength = strlen(output);

	if (g_outputLength > MAX_INDICES)
	{
		printf("Error: String too long!\n");

		return GLUS_FALSE;
	}

	for (i = 0; i < g_outputLength; i++)
	{
		g_outputIndices[i] = (GLUSuint)output[i];
	}

	// Dark grey, opaque background.

	VGfloat clearColor[4] = {0.2f, 0.2f, 0.2f, 1.0f};

    // The background will just cleared with black color.
	vgSetfv(VG_CLEAR_COLOR, 4, clearColor);

	//

	// Load the bitmap font.

	if (!glusLoadTgaImage("CourierNew.tga", &tgaimage))
	{
		printf("Error: Could not load bitmap font!\n");

		return GLUS_FALSE;
	}

	// RGBA, so for channels.
	numberColorChannels = 4;
	// Specific parameters of this bitmap font.
	cellWidth = 32;
	cellHeight = 18;
	fontWidth = 16;
	fontHeight = 16;
	numberCellColumns = tgaimage.width / cellWidth;

	glyphOrigin[0] = 0.0f;
	glyphOrigin[1] = 0.0f;

	// Bitmap font, so all values are the same.
	g_outputEscapementX = (VGfloat)fontWidth - 4.0f;

	escapement[0] = g_outputEscapementX;
	escapement[1] = 0.0f;

	// Create the font.

	g_font = vgCreateFont(GLYPH_CAPACITY);

	// Create all glyphs.
	for (i = 0; i < GLYPH_CAPACITY; i++)
	{
		g_images[i] = vgCreateImage(VG_lRGBA_8888, fontWidth, fontHeight, VG_IMAGE_QUALITY_BETTER);

		// Calculate x coordinate in the image.
		x = cellWidth*(i%numberCellColumns);
		// Calculate y coordinate in the image.
		y = (tgaimage.height - 1) - cellHeight * (i/numberCellColumns + 1) + (cellHeight - fontHeight);

		// Copy one glyph out of the TGA image into the VG image.
		vgImageSubData(g_images[i], &tgaimage.data[y*numberColorChannels*tgaimage.width + x*numberColorChannels], tgaimage.width * numberColorChannels, VG_lRGBA_8888, 0, 0, fontWidth, fontHeight);

		// Assign the glyph to the VG image.
		vgSetGlyphToImage(g_font, i, g_images[i], glyphOrigin, escapement);
	}

	// Destroy, as the bitmap image is now stored in the VG images.

	glusDestroyTgaImage(&tgaimage);

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

	// Center on the screen.
	g_outputOrigin[0] = (VGfloat)g_width / 2.0f - g_outputEscapementX * (VGfloat)g_outputLength / 2.0f;
	g_outputOrigin[1] = (VGfloat)g_height / 2.0f;
}

/**
 * Function to render and display content. Swapping of the buffers is automatically done.
 *
 * @return true for continuing, false to exit the application
 */
GLUSboolean update(GLUSfloat time)
{
    // Now, the background is painted with dark grey.
    vgClear(0, 0, g_width, g_height);

    // Set the position of the text ...
	vgSetfv(VG_GLYPH_ORIGIN, 2, g_outputOrigin);

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

	for (i = 0; i < GLYPH_CAPACITY; i++)
	{
		if (g_font != VG_INVALID_HANDLE)
		{
			vgClearGlyph(g_font, i);
		}

		if(g_images[i] != VG_INVALID_HANDLE)
		{
			vgDestroyImage(g_images[i]);

			g_images[i] = VG_INVALID_HANDLE;
		}
	}

	if (g_font != VG_INVALID_HANDLE)
	{
		vgDestroyFont(g_font);

		g_font = VG_INVALID_HANDLE;
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
	        EGL_RENDERABLE_TYPE, EGL_OPENVG_BIT,
	        EGL_NONE
	};

    EGLint eglContextAttributes[] = {
    		EGL_CONTEXT_CLIENT_VERSION, 1,
    		EGL_NONE
    };

    glusInitFunc(init);

    glusReshapeFunc(reshape);

    glusUpdateFunc(update);

    glusTerminateFunc(terminate);

    if (!glusCreateWindow("GLUS Example Window", 640, 480, GLUS_FALSE, GLUS_FALSE, eglConfigAttributes, eglContextAttributes))
    {
        printf("Could not create window!\n");
        return -1;
    }

    glusRun();

    return 0;
}
