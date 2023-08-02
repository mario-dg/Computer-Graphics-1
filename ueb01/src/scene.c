/**
 * @file
 * Darstellungs-Modul.
 * Das Modul kapselt die Rendering-Funktionalitaet (insbesondere der OpenGL-
 * Aufrufe) des Programms.
 *
 * Bestandteil eines Beispielprogramms fuer Animationen mit OpenGL & GLUT.
 *
 * @author copyright (C) Fachhochschule Wedel 1999-2011. All rights reserved.
 * @author Leonhard Brandes, Mario da Graca
 */

/* ---- System Header einbinden ---- */
#ifdef WIN32
#include <windows.h>
#endif

#ifdef __APPLE__
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>
#endif

/* ---- System Header einbinden ---- */
#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>

/* ---- Eigene Header einbinden ---- */
#include "scene.h"
#include "logic.h"
#include "types.h"
#include "math.h"

/** Konstante fuer Berechnungen mit PI */
#define PI 3.141592653589793

/** Anzahl der Punkte aus dem der Ball gebildet wird */
#define ballVertices 12

/**
 * Zeichnet ein Dreieck mit Katenlaengen 1
 * 
 * @param[in] CGColor3f, Farbe des Dreiecks
 */ 
void drawTriangle(CGColor3f color)
{
    glBegin(GL_TRIANGLES);
    {
        glColor3f(color[0], color[1], color[2]);
        glVertex2f(0.5f, -0.3661f);
        glVertex2f(0.0f, 0.5f);
        glVertex2f(-0.5f, -0.3661f);
    }
    glEnd();
}

/**
 * Zeichnet die Extras, skaliert sie und positioniert sie anhand der Position
 * der Box, aus der sie spawnt
 * 
 * @param[in] box, box aus der das Extra spawnt
 */ 
void drawExtra(box box)
{

    glPushMatrix();
    {
        glLoadIdentity();
        //printf("x: %f, y: %f\n", box.extraType.center[0], box.extraType.center[1]);
        glTranslatef(box.center[0], box.center[1], 0.0f);
        glRotatef(box.extraType.angle, 0.0f, 0.0f, 1.0f);
        glScalef(TRI_SIDE_LENGTH, TRI_SIDE_LENGTH, 1.0f);
        drawTriangle(box.extraType.color);
    }
    glPopMatrix();
}


/**
 * Zeichnet ein Quadrat mit der uebergebenen Farbe, 
 * mit Seitenlaenge 1 um den Ursprung rum
 * 
 * @param[in] color, Farbe des Quadrats
 */
void drawRect(CGColor3f color)
{
    glBegin(GL_QUADS);
    {
        glColor3f(color[0], color[1], color[2]);
        glVertex2f(0.5f, -0.5f);
        glVertex2f(0.5f, 0.5f);
        glVertex2f(-0.5f, 0.5f);
        glVertex2f(-0.5f, -0.5f);
    }
    glEnd();
}

/**
 * Zeichnet einen Zirkel mit Radius 1 um den Ursprung rum
 * 
 */
void drawCircle(void)
{
    GLdouble theta = 2 * PI;
    GLdouble step = 2 * PI / ballVertices;
    glBegin(GL_POLYGON);
    {
        glColor3f(1.0f, 0, 0);
        while (theta > 0)
        {
            glVertex2f(cos(theta), sin(theta));
            theta -= step;
        }
    }
    glEnd();
}

/**
 * Zeichnet einen Ball, Skaliert ihn auf die Groeße CIRCLE_RADIUS und versetzt
 * ihn an die Startposition
 * 
 * @param[in] *Ball, dder gezeichnet werden soll
 */
static void
drawBall(ball *ball)
{

    //CGPoint2f circleCenter = ball->center;
    glPushMatrix();
    {
        glLoadIdentity();
        glTranslatef(ball->center[0], ball->center[1], 0.0f);
        glScalef(CIRCLE_RADIUS, CIRCLE_RADIUS, 1.0f);
        drawCircle();
    }
    glPopMatrix();
}

/**
 * Zeichnet alle Baelle, die momentan im Spiel sind
 * 
 */
void drawAllBalls()
{
    GLint amountBalls = getAmountUsedBalls();
    GLint m = 0;
    for (m = 0; m < amountBalls; m++)
    {
        if (getBall(m)->visible)
        {
            drawBall(getBall(m));
        }
    }
}

/**
 * Zeichnet den Schlaeger, skaliert ihn auf die momentante Breite batWidth und die feste Hoehe QUAD_HEIGHT_BAT
 * 
 */
void drawBat(void)
{
    CGPoint2f *batCenter = getBatCenter();
    GLfloat batWidth = getBatWidth();
    glPushMatrix();
    {
        glLoadIdentity();
        glTranslatef((*batCenter)[0], (*batCenter)[1], 0.0f);
        glScalef(batWidth, QUAD_HEIGHT_BAT, 1.0f);
        CGColor3f white = {1.0f, 1.0f, 1.0f};
        drawRect(white);
    }
    glPopMatrix();
}

/**
 * Zeichnet den rechten Rahmen des Spielfeldes
 * 
 * @param[in] CGColor3f, Farbe des Rahmens
 */ 
void drawRightFrame(CGColor3f color)
{
    glPushMatrix();
    {
        glLoadIdentity();
        glTranslatef(0.975f, 0.0f, 0.0f);
        glScalef(FRAME_WIDTH, 2.0f, 1.0f);
        drawRect(color);
    }
    glPopMatrix();
}

/**
 * Zeichnet den oberen Rahmen des Spielfeldes
 * 
 * @param[in] CGColor3f, Farbe des Rahmens
 */ 
void drawUpperFrame(CGColor3f color)
{
    glPushMatrix();
    {
        glLoadIdentity();
        glTranslatef(0.0f, 0.975f, 0.0f);
        glScalef(1.9f, FRAME_WIDTH, 1.0f);
        drawRect(color);
    }
    glPopMatrix();
}

/**
 * Zeichnet den linken Rahmen des Spielfeldes
 * 
 * @param[in] CGColor3f, Farbe des Rahmens
 */ 
void drawLeftFrame(CGColor3f color)
{
    glPushMatrix();
    {
        glLoadIdentity();
        glTranslatef(-0.975f, 0.0f, 0.0f);
        glScalef(FRAME_WIDTH, 2.0f, 1.0f);
        drawRect(color);
    }
    glPopMatrix();
}

/**
 * Zeichnet den Rahmen des Spielfeldes
 * 
 */
static void
drawFrame(void)
{
    CGColor3f white = {1.0f, 1.0f, 1.0f};
    //Rechter Rand
    drawRightFrame(white);

    // Oberer Rand
    drawUpperFrame(white);

    //Linker Rand
    drawLeftFrame(white);
}

/**
 * Zeichnet eine noch nicht zerstoert Box auf dem Spielfeld, skaliert sie auf die Breite QUAD_WIDTH_BLOCK 
 * und die HoeHE QUAD_HEIGHT_BLOCK und versetzt sie an die richtige Position
 * 
 * @param[in] box, die gezeichnet werden soll
 */
static void
drawBox(box box)
{
    if (box.visible)
    {
        glPushMatrix();
        {
            glLoadIdentity();
            glTranslatef(box.center[0], box.center[1], 0.0f);
            glScalef(QUAD_WIDTH_BLOCK, QUAD_HEIGHT_BLOCK, 1.0f);
            drawRect(box.color);
        }
        glPopMatrix();
    }
    else if (box.extraType.visible && box.extraType.type != e_normal)
    {
        drawExtra(box);
    }
}

/**
 * Zeichnet alle noch nicht zerstoerten Bloecke auf dem Spielfeld
 * 
 */
void drawAllBoxes()
{
    GLint x = 0;
    GLint y = 0;

    for (x = 0; x < ROWS; x++)
    {
        for (y = 0; y < COLS; y++)
        {
            drawBox(getBox(x, y));
        }
    }
}

/**
 * Zeichen-Funktion.
 * Stellt die Szene dar. Ein Quadrat und ein Rahmen werden gezeichnet.
 */
void drawScene(void)
{
    drawFrame();

    drawAllBoxes();

    drawAllBalls();

    drawBat();
}

/**
 * Initialisierung der Szene (inbesondere der OpenGL-Statusmaschine).
 * Setzt Hintergrund- und Zeichenfarbe.
 * @return Rueckgabewert: im Fehlerfall 0, sonst 1.
 */
int initScene(void)
{
    /* Setzen der Farbattribute */
    /* Hintergrundfarbe */
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    /* Zeichenfarbe */
    glColor3f(1.0f, 1.0f, 1.0f);

    /* Linienbreite */
    glLineWidth(3.0f);

    /* Erstellt die Boxen */
    initGame();

    /* Alles in Ordnung? */
    return (glGetError() == GL_NO_ERROR);
}

/**
 * (De-)aktiviert den Wireframe-Modus.
 */
void toggleWireframeMode(void)
{
    /* Flag: Wireframe: ja/nein */
    static GLboolean wireframe = GL_FALSE;

    /* Modus wechseln */
    wireframe = !wireframe;

    if (wireframe)
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }
    else
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }
}
