/*
Name: KAZI ISLAM

===============================================================
                 GLOBAL PROGRAM DOCUMENTATION
===============================================================

TITLE:
    2D Character Animation & Parallax Movement System (OpenGL/GLUT)

DESCRIPTION:
    This program creates a 2D animated character inside a 3D OpenGL
    environment using textures, sprite sheets, and parallax scrolling.

    The character remains visually centered on the screen, while the
    background moves in the opposite direction to give the illusion
    of world movement.

    The character uses a sprite sheet where each frame is drawn as a
    textured quad. Frames animate while the character is moving.

---------------------------------------------------------------
FEATURES:
---------------------------------------------------------------

1. **Character Animation**
    - Character uses a PNG sprite sheet.
    - Each frame of animation is selected using texture coordinates.
    - Frames update automatically based on a timer.
    - Character flips horizontally depending on direction.

2. **Movement System**
    There are *two different* movement systems:

    A) **Arrow Keys — Manual Movement**
        - Holding → Arrow moves right.
        - Holding ← Arrow moves left.
        - Releasing the arrow key stops movement immediately.
        - No timer, movement is fully controlled by the player.

    KEYBOARD:
    W  → toggle wireframe rendering
    ←  → move left (while held)
    →  → move right (while held)
    Release key → stop movement
    Q or ESC → exit program


    B) **Mouse Click — Automatic 20-Second Run**
        - Clicking left side of window: run left for 20 seconds.
        - Clicking right side: run right for 20 seconds.
        - Movement & animation continue automatically until timer expires.

    MOUSE:
    Left-click left side  → automatic run left for 20 seconds
    Left-click right side → automatic run right for 20 seconds

---------------------------------------------------------------
END OF GLOBAL PROGRAM DOCUMENTATION
===============================================================
*/

// Include string handling functions from the C standard library
#include <string.h>

// If compiling on macOS, use the Apple version of GLUT
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
// Otherwise, use the standard/freeglut GLUT header
#include <GL/glut.h>
#endif

// If GL_BGR is not defined, define it manually (used for BMP color format)
#ifndef GL_BGR
#define GL_BGR 0x80E0
#endif

// Tell the compiler we are going to use stb_image's implementation in this file
#define STB_IMAGE_IMPLEMENTATION
// Include stb_image to load PNG (and other) images
#include "stb_image.h"

// Standard C input/output library (printf, etc.)
#include <stdio.h>
// Standard library (memory, rand, etc.)
#include <stdlib.h>
// C++ iostream library for input/output streams (cout, etc.)
#include <iostream>
// Math functions (sin, cos, etc.)
#include <math.h>

// Define PI (π ≈ 3.14159)
#define PI 3.14159

// Use the standard namespace so we can use std:: things without std::
using namespace std;

// Declare a function that loads a BMP file and returns an OpenGL texture ID
GLuint LoadBMP(const char* filename);

// Global variable to control if wireframe mode is enabled or not
bool WireFrame= false;


// ---------- NEW GLOBALS ----------

// Movement timer: how long to keep running after a mouse click
float moveTimer = 0.0f;           // remaining movement time in seconds
const float MOVE_DURATION = 20.0f; // total time to run after a mouse click

// For real time delta between frames (for smooth movement and animation)
float lastTime = 0.0f;


// Store current window width and height
float Wwidth, Wheight;

// Texture IDs (handles) for OpenGL textures
GLuint bgTexID;        // texture ID for background image
GLuint charTexID;      // texture ID for character sprite sheet

// Background texture coordinates (used to scroll texture for parallax effect)
float bgXmin = 0.0f, bgXmax = 1.0f;  // x texture coordinate range
float bgYmin = 0.0f, bgYmax = 1.0f;  // y texture coordinate range
float bgOffsetX = 0.0f;   // amount of horizontal texture scroll for parallax

// Character position

float charY = -2.5f;      // vertical position of character in world units
float charSpeed = 0.1f;   // character movement speed (world units per second)

// Sprite sheet information (change these to match your sprite image)
int spriteCols = 8;       // number of columns in sprite sheet
int spriteRows = 1;       // number of rows in sprite sheet
int currentFrame = 0;     // which frame index we are currently showing
int totalFrames = spriteCols * spriteRows; // total number of frames

// "World" position of character (used for logic only, camera stays centered)
float worldX = 0.0f;       // character's x-position in the world
float targetX = 0.0f;      // target x-position (unused now, but kept)
int moveDir = 0;           // movement direction: -1 = left, 1 = right, 0 = stop
bool isMoving = false;     // true if character started moving via mouse click

// Character position on the screen (always drawn in the center)
float charScreenX = 0.0f;  // x-position on screen (centered)
bool facingRight = true;   // true if character faces right, false if faces left


// Simple timer for animation (choose how often to change animation frame)
float frameTime = 0.1f;        // seconds per animation frame
float timeSinceLastFrame = 0.0f; // time since last frame change

// Ambient light color for scene lighting (RGBA)
const GLfloat light_ambient[]  = { 0.0f, 0.0f, 0.0f, 1.0f };
// Diffuse light color (main light brightness)
const GLfloat light_diffuse[]  = { 1.0f, 1.0f, 1.0f, 1.0f };
// Specular light color (shininess)
const GLfloat light_specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
// Light position in the scene
const GLfloat light_position[] = { 2.0f, 5.0f, 5.0f, 0.0f };

// Material ambient color (how object responds to ambient light)
const GLfloat mat_ambient[]    = { 0.7f, 0.7f, 0.7f, 1.0f };
// Material diffuse color (main object color)
const GLfloat mat_diffuse[]    = { 0.8f, 0.8f, 0.8f, 1.0f };
// Material specular color (highlight color)
const GLfloat mat_specular[]   = { 1.0f, 1.0f, 1.0f, 1.0f };
// Material shininess (how sharp highlights are)
const GLfloat high_shininess[] = { 100.0f };







/*
 * Function: resize
 * ----------------
 * Called when the window is resized.
 * Updates the viewport and projection so the 3D scene scales correctly
 * to the new window width and height.
 */
static void resize(int width, int height)
{
     double Ratio;                       // variable to store aspect ratio

     Wwidth = (float)width;              // store new window width as float
     Wheight = (float)height;            // store new window height as float

     Ratio= (double)width /(double)height; // compute aspect ratio width/height

    glViewport(0,0,(GLsizei) width,(GLsizei) height); // set the viewport to cover the entire window

    glMatrixMode(GL_PROJECTION);         // switch to projection matrix mode
    glLoadIdentity();                    // reset the projection matrix

	gluPerspective (45.0f,Ratio,0.1f, 100.0f); // set a perspective projection (FOV, aspect, near, far)
	glMatrixMode(GL_MODELVIEW);          // switch back to modelview matrix
	glLoadIdentity();                    // reset modelview matrix

 }


/*
 * Function: display
 * -----------------
 * Called every frame to draw the scene.
 * Draws the parallax background and the animated character sprite.
 */
static void display(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // clear color and depth buffers
    glMatrixMode(GL_MODELVIEW);                         // use modelview matrix
    glLoadIdentity();                                   // reset modelview

    // Camera
    gluLookAt(0, 0, 10.0,   // camera position (eye)
              0.0, 0.0, 0.0, // look-at point
              0.0, 1.0, 100.0); // up vector (y-axis)

    if (WireFrame)                            // if wireframe mode is enabled
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // draw only lines (no filled polygons)
    else
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); // draw filled polygons

    // For 2D textures we don't really want lighting
    glDisable(GL_LIGHTING);                 // disable lighting so textures show normal colors
    glColor3f(1.0f, 1.0f, 1.0f);            // set drawing color to white (so texture colors are not tinted)

    // ---------- DRAW PARALLAX BACKGROUND ----------
    glBindTexture(GL_TEXTURE_2D, bgTexID);  // bind the background texture

    glBegin(GL_QUADS);                      // draw a textured quad
        // We add bgOffsetX to scroll texture in X
        glTexCoord2f(bgXmin + bgOffsetX, bgYmin); glVertex3f(-12.0f, -7.0f, -5.0f); // bottom-left corner
        glTexCoord2f(bgXmax + bgOffsetX, bgYmin); glVertex3f( 12.0f, -7.0f, -5.0f); // bottom-right corner
        glTexCoord2f(bgXmax + bgOffsetX, bgYmax); glVertex3f( 12.0f,  7.0f, -5.0f); // top-right corner
        glTexCoord2f(bgXmin + bgOffsetX, bgYmax); glVertex3f(-12.0f,  7.0f, -5.0f); // top-left corner
    glEnd();


// ---------- DRAW CHARACTER SPRITE ----------
glBindTexture(GL_TEXTURE_2D, charTexID);    // bind the character texture (sprite sheet)

// Compute which part of the sprite sheet to use
float frameWidth  = 1.0f / (float)spriteCols;  // width of one frame in texture coordinates
float frameHeight = 1.0f / (float)spriteRows;  // height of one frame in texture coordinates

int col = currentFrame % spriteCols;        // column index of current frame
int row = currentFrame / spriteCols;        // row index of current frame

float s0 = col * frameWidth;               // left texture coordinate of frame
float s1 = s0 + frameWidth;                // right texture coordinate of frame


float t1 = 1.0f - row * frameHeight;       // bottom texture coordinate (flipped vertically)
float t0 = t1 + frameHeight;               // top texture coordinate (flipped vertically)


// Flip horizontally if facing left
if (!facingRight) {                         // if character is facing left
    float tmp = s0;                         // temporary variable to swap s0 and s1
    s0 = s1;                                // swap left and right texture coordinates
    s1 = tmp;                               // complete the swap
}

// Size of character quad in world units

float halfW = 1.5f;                         // half width of character in world units
float halfH = 1.5f;                         // half height of character in world units



// Draw at screen center (charScreenX) instead of moving with world
glBegin(GL_QUADS);                          // begin drawing character quad
    glTexCoord2f(s0, t0); glVertex3f(charScreenX - halfW, charY - halfH, 0.0f); // bottom-left corner
    glTexCoord2f(s1, t0); glVertex3f(charScreenX + halfW, charY - halfH, 0.0f); // bottom-right corner
    glTexCoord2f(s1, t1); glVertex3f(charScreenX + halfW, charY + halfH, 0.0f); // top-right corner
    glTexCoord2f(s0, t1); glVertex3f(charScreenX - halfW, charY + halfH, 0.0f); // top-left corner
glEnd();



    glEnable(GL_LIGHTING); // turn it back on in case you draw 3D later

    glutSwapBuffers();     // swap the front and back buffers (double buffering)
}


/*
 * Function: LoadBMP
 * -----------------
 * Loads a BMP image file from disk and creates an OpenGL texture.
 * Returns the generated texture ID, or 0 if loading fails.
 */
GLuint LoadBMP(const char* filename)
{
    GLuint texture;                        // OpenGL texture ID
    int width, height;                     // image width and height
    unsigned char header[54]; // BMP header (always 54 bytes)
    unsigned int dataPos;                  // start position of pixel data in file
    unsigned int imageSize;                // size of image data in bytes
    unsigned char* data;                   // pointer to raw pixel data

    FILE* file = fopen(filename, "rb");    // open file in binary read mode
    if (!file) {                           // if file failed to open
        printf("Image could not be opened: %s\n", filename); // print error
        return 0;                          // return 0 to indicate failure
    }

    if (fread(header, 1, 54, file) != 54) { // read BMP header (54 bytes)
        printf("Incorrect BMP file: %s\n", filename); // header not correct size
        fclose(file);                                   // close file
        return 0;                                      // return failure
    }

    // BMP header check
    if (header[0] != 'B' || header[1] != 'M') { // check 'BM' signature
        printf("Not a valid BMP file: %s\n", filename); // not a BMP file
        fclose(file);                                   // close file
        return 0;                                      // return failure
    }

    // Get information from header
    dataPos   = *(int*)&(header[0x0A]);      // offset to image data
    imageSize = *(int*)&(header[0x22]);      // size of image data
    width     = *(int*)&(header[0x12]);      // image width in pixels
    height    = *(int*)&(header[0x16]);      // image height in pixels

    if (imageSize == 0) imageSize = width * height * 3; // if not given, compute (3 bytes per pixel)
    if (dataPos == 0)   dataPos = 54;                   // default data offset just after header

    data = new unsigned char[imageSize];    // allocate memory for image data
    fread(data, 1, imageSize, file);        // read image data from file
    fclose(file);                           // close the BMP file

    // Create OpenGL texture
    glGenTextures(1, &texture);             // generate one texture ID
    glBindTexture(GL_TEXTURE_2D, texture);  // bind the new texture

    // Load texture into OpenGL
    glTexImage2D(
        GL_TEXTURE_2D, 0, GL_RGB,          // texture target, mipmap level, color format
        width, height, 0,                  // width, height, border
        GL_BGR, GL_UNSIGNED_BYTE, data     // source data format and pointer
    );

    // Texture filters (how texture is scaled)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); // minification filter
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); // magnification filter

    delete[] data;                          // free the loaded image data

    return texture;                         // return the texture ID
}



/*
 * Function: key
 * -------------
 * Handles normal keyboard input (non-special keys).
 * 'q' or ESC exits the program, 'w' toggles wireframe mode.
 */
static void key(unsigned char key, int x, int y)
{
    switch (key)                   // check which key was pressed
    {
        case 27 :                  // ASCII code 27 = ESC key
        case 'q':                  // 'q' key
            exit(0);               // exit the program
            break;

	  case 'w':                  // 'w' key
		WireFrame =!WireFrame;   // toggle wireframe mode on/off
	       break;
    }
}


/*
 * Function: Specialkeys
 * ---------------------
 * Handles special keys pressed (arrow keys, function keys, etc.).
 * Here it is used to start moving left or right while the key is held down.
 */
void Specialkeys(int key, int x, int y)
{
    switch (key)                         // check which special key
    {
    case GLUT_KEY_RIGHT:                // right arrow key
        facingRight = true;             // face the character to the right
        moveDir = 1;                    // move in positive x direction
        break;

    case GLUT_KEY_LEFT:                 // left arrow key
        facingRight = false;            // face the character to the left
        moveDir = -1;                   // move in negative x direction
        break;
    }
}


/*
 * Function: SpecialkeysUp
 * -----------------------
 * Handles special keys released (arrow keys released).
 * When the left or right arrow is released, the character stops moving.
 */
void SpecialkeysUp(int key, int x, int y)
{
    switch (key)                         // check which key was released
    {
    case GLUT_KEY_RIGHT:                // right arrow released
        // if we were moving right, stop
        if (moveDir == 1)               // only stop if we were moving right
            moveDir = 0;                // stop movement
        break;

    case GLUT_KEY_LEFT:                 // left arrow released
        // if we were moving left, stop
        if (moveDir == -1)              // only stop if we were moving left
            moveDir = 0;                // stop movement
        break;
    }
}


/*
 * Function: idle
 * --------------
 * Called when the program is idle (no events).
 * Updates character movement, parallax background, and animation frames
 * based on real time (delta time dt).
 */
static void idle(void)
{
    // --- compute time since last frame (dt) in seconds ---
    float currentTime = glutGet(GLUT_ELAPSED_TIME) / 1000.0f; // get time since program start in seconds
    float dt = currentTime - lastTime;                        // time difference since last frame
    lastTime = currentTime;                                   // store current time for next frame

    // Move character whenever moveDir != 0 (mouse OR arrow keys)
    if (moveDir != 0)                                         // only move if we have a direction
    {
        // Step in world space: charSpeed is units per second now
        float step = charSpeed * moveDir * dt;                // movement amount = speed * direction * time
        worldX += step;                                       // update character world x position

        // Parallax background: move opposite to character
        bgOffsetX += step * 0.3f;                             // scroll background texture for parallax

        // Animate sprite based on time
        timeSinceLastFrame += dt;                             // accumulate time since last animation frame
        if (timeSinceLastFrame >= frameTime)                  // if enough time has passed
        {
            currentFrame = (currentFrame + 1) % totalFrames;  // go to next frame, loop around
            timeSinceLastFrame = 0.0f;                        // reset frame timer
        }

        // --- TIMER ONLY FOR MOUSE CLICKS ---
        if (moveTimer > 0.0f)                                 // if we are in timed movement mode (mouse)
        {
            moveTimer -= dt;                                  // decrease remaining movement time

            // when timer ends, stop movement
            if (moveTimer <= 0.0f)                            // if time is up
            {
                moveTimer = 0.0f;                             // clamp to zero
                moveDir   = 0;                                // stop movement
            }
        }
        // If moveTimer == 0 (arrow keys), we don't stop here.
    }

    glutPostRedisplay();                                      // request another redraw of the scene
}



/*
 * Function: mouse
 * ---------------
 * Handles mouse button input.
 * Left click on left side of screen makes the character auto-run left for 20 seconds.
 * Left click on right side makes the character auto-run right for 20 seconds.
 */
void mouse(int btn, int state, int x, int y)
{
    if (btn == GLUT_LEFT_BUTTON && state == GLUT_DOWN)   // if left mouse button is pressed
    {
        // Decide direction only from which side of the window we click
        float centerX = Wwidth / 2.0f;                   // compute horizontal center of window

        if (x < centerX)                                 // if click is on the left half of the window
        {
            // Click on left side -> run LEFT
            facingRight = false;                         // face left
            moveDir = -1;                                // move to the left
        }
        else                                             // if click is on the right half
        {
            // Click on right side -> run RIGHT
            facingRight = true;                          // face right
            moveDir = 1;                                 // move to the right
        }

        // Start (or restart) a 20-second run in that direction
        moveTimer = MOVE_DURATION;  // reset timer       // set timed movement to 20 seconds
        isMoving = true;                                 // mark that movement started (info only)
    }

    glutPostRedisplay();                                 // request the screen to redraw
}



/*
 * Function: LoadPNG
 * -----------------
 * Uses stb_image to load a PNG image from disk and create an OpenGL texture.
 * Returns the texture ID, or 0 if the load fails.
 */
GLuint LoadPNG(const char* filename)
{
    int width, height, channels;                         // variables to store image size and channels
    unsigned char* data = stbi_load(filename, &width, &height, &channels, 4);
                                                         // load image with stb_image, force 4 channels (RGBA)

    if (!data) {                                         // if loading failed
        printf("Failed to load PNG image %s\n", filename); // print error message
        return 0;                                        // return 0 for failure
    }

    GLuint texID;                                        // OpenGL texture ID
    glGenTextures(1, &texID);                            // generate one texture
    glBindTexture(GL_TEXTURE_2D, texID);                 // bind the texture

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,              // create a 2D texture with RGBA format
                 width, height, 0,
                 GL_RGBA, GL_UNSIGNED_BYTE, data);       // provide raw pixel data

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); // set minification filter
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); // set magnification filter

    stbi_image_free(data);                               // free image memory from stb_image
    return texID;                                        // return created texture ID
}



/*
 * Function: init
 * --------------
 * Initializes OpenGL state, lighting, textures, blending, and timing.
 * Called once at the beginning of the program.
 */
static void init(void)
{
    glEnable(GL_CULL_FACE);                              // enable face culling (do not draw back faces)
    glCullFace(GL_BACK);                                 // specify culling of back faces

    glClearColor(0.5f, 0.5f, 1.0f, 0.0f); // your background color // set clear color (sky blue)

    glEnable(GL_NORMALIZE);                              // normalize normals for correct lighting
    glEnable(GL_COLOR_MATERIAL);                         // enable color tracking for materials

    glEnable(GL_DEPTH_TEST);                             // enable depth testing (3D sorting)
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);   // ask for best perspective correction
    glShadeModel(GL_SMOOTH);                             // use smooth shading

    glLightfv(GL_LIGHT0, GL_AMBIENT,  light_ambient);    // set ambient light properties for light 0
    glLightfv(GL_LIGHT0, GL_DIFFUSE,  light_diffuse);    // set diffuse light properties
    glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);   // set specular light properties
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);   // set position of light 0

    glMaterialfv(GL_FRONT, GL_AMBIENT,   mat_ambient);   // set material ambient color
    glMaterialfv(GL_FRONT, GL_DIFFUSE,   mat_diffuse);   // set material diffuse color
    glMaterialfv(GL_FRONT, GL_SPECULAR,  mat_specular);  // set material specular color
    glMaterialfv(GL_FRONT, GL_SHININESS, high_shininess);// set material shininess

    glEnable(GL_LIGHT0);                                 // enable light 0
    glEnable(GL_NORMALIZE);                              // (again) ensure normals are normalized
    glEnable(GL_LIGHTING);                               // enable lighting system


    glEnable(GL_TEXTURE_2D);  // allow textures to work  // enable 2D texturing

    bgTexID   = LoadBMP("images/Background.bmp");  // background can stay BMP // load background texture
    charTexID = LoadPNG("images/character.png");   // <-- NEW LINE HERE      // load character sprite texture


    // Optional but recommended: texture smoothing
glBindTexture(GL_TEXTURE_2D, bgTexID);                   // bind background texture
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); // background minification filter
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); // background magnification filter

glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);     // repeat background horizontally
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);     // repeat background vertically

glBindTexture(GL_TEXTURE_2D, charTexID);                 // bind character texture
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); // character minification filter
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); // character magnification filter

// enable transparency
glEnable(GL_BLEND);                                      // enable alpha blending
glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);       // set blending function (standard transparency)

// initialize time for movement and animation
lastTime = glutGet(GLUT_ELAPSED_TIME) / 1000.0f;         // set initial time (in seconds)

}


/*
 * Function: main
 * --------------
 * Entry point of the program.
 * Initializes GLUT, creates the window, registers callbacks, and starts the main loop.
 */
int main(int argc, char *argv[])
{
    glutInit(&argc, argv);                               // initialize GLUT with command-line arguments

    glutInitWindowSize(800,600);                         // set initial window size (width, height)
    glutInitWindowPosition(0,0);                         // set initial window position (x, y)
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH); // set display mode: RGB color, double buffer, depth test

    glutCreateWindow("Project Assignment 3");            // create a window with a title
    glutFullScreen();                                    // switch to full-screen mode

    init();                                              // call our initialization function
    glutReshapeFunc(resize);                             // register resize callback
    glutDisplayFunc(display);                            // register display callback
    glutMouseFunc(mouse);                                // register mouse callback
    glutKeyboardFunc(key);                               // register keyboard callback for normal keys
    glutSpecialFunc(Specialkeys);                        // register special keys (arrow keys) callback
    glutSpecialUpFunc(SpecialkeysUp);                    // register special keys release callback


    glutIdleFunc(idle);                                  // register idle callback (runs when nothing else happens)

    glutMainLoop();                                      // enter the GLUT main event loop

    return EXIT_SUCCESS;                                 // return success code (never reached in GLUT)
}
