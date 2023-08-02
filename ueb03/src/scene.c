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
#include <GLUT/glut.h>
#else
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#endif

/* ---- System Header einbinden ---- */
#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>

/* ---- Eigene Header einbinden ---- */
#include "scene.h"
#include "types.h"
#include "logic.h"
#include "math.h"
#include "stringOutput.h"

/**Kameraposition im KugelKoordinatensystem */
static GLfloat cameraRadiussph = 0.0f;
static GLfloat cameraThetaSph = 0.0f;
static GLfloat cameraPhiSph = 0.0f;

/** Displaylist IDs fuer alle Objekte, die mehrfach gezeichnet werden */
static GLuint g_wallCubeListID = 0;
static GLuint g_wallCubeListNormalID = 0;

static GLuint g_boxCubeListID = 0;
static GLuint g_boxCubeListNormalID = 0;

static GLuint g_freeCellListID = 0;
static GLuint g_freeCellListNormalID = 0;

static GLuint g_portalListID = 0;

static GLuint g_tetrahedronListID = 0;
static GLuint g_tetrahedronListNormalID = 0;

static GLuint g_tetrahedronBlueTransListID = 0;
static GLuint g_tetrahedronBlueTransListNormalID = 0;

/** Zum Konstrueriern aller Bloecke */
#define AMOUNT_PIXEL_PER_BLOCK 12
#define AMOUNT_TRIS_PER_TRIANGLE 8

/** Zum Konstruieren, eines gleichseitigen Dreiecks */
#define TRIANGLE_SCALING_VALUE 0.67f
#define TRIANGLE_HEIGHT ((sqrt(3)) / (2))
#define TRIANGLE_HEIGHT_LOWER_THIRD ((sqrt(3)) / (6))

/** Konstanten fuer die Erstellung eines Portals */
#define RADIUS_1 0.9f
#define RADIUS_2 0.7f
#define RADIUS_3 0.5f
#define RADIUS_4 0.3f
#define RADIUS_5 0.15f

/** Konstanten fuer die Erstellung einer Mauer */
#define AMOUNT_ROWS_WALL 4
#define EVEN_ROWS_WALL 3
#define ODD_ROWS_WALL 4

/** Konstanten fuer die Erstellung einer Kiste*/
#define AMOUNT_BOXES_ROW 8

/** Konstanten fuer die Erstellung eines Portals */
#define RADIUS_1 0.9f
#define RADIUS_2 0.7f
#define RADIUS_3 0.5f
#define RADIUS_4 0.3f
#define RADIUS_5 0.15f
/** Anzahl der Segmente, aus denen die konzentrischen Ringe aufgebaut sind. */
#define SLICES (30)

/** Anzahl der konzentrischen Ringe um den Mittelpunkt. */
#define LOOPS (5)

/** Konstanten fuer die Textausgabe */
#define TEXT_Y 0.99f
#define TEXT_TIME_X 0.3f
#define TEXT_LEVEL_X 0.65f
#define TEXT_END_X 0.4f
#define TEXT_END_Y 0.5f

/** Verhindert Z-Fighting */
#define Z_OFFSET 0.001f

/** Winkel fuer die Dachschraegen arccos(sqrt(3) / 3)*/
#define ANGLE_ROOF 54.73561032f

/** Tetraeder Winkel 1 - arccos(1 / 3) */
#define ANGLE_TETRAHEDON 109.4712206f

//#define M_PI_2 1.570796327
//#define M_PI_4 0.785398163

/** Startposition der Kamera */
#define CAMERA_DEFAULT_RADIUS 17.0f
#define CAMERA_DEFAULT_THETA M_PI_4
#define CAMERA_DEFAULT_PHI M_PI_2

/** Faktor zur Helligkeitsreflektion */
GLfloat brightnessScaling = 0.35f;

/** Alle Farben, die verwendet werden */
static const CGColor3f COLOR_DARK_GREY = {0.25f, 0.25f, 0.25f};
static const CGColor3f COLOR_LIGHT_GREY = {0.78f, 0.78f, 0.78f};
static const CGColor4f COLOR_DARK_BLUE = {0.1f, 0.0f, 0.6f, 0.0f};
static const CGColor3f COLOR_RED = {1.0f, 0.0f, 0.0f};
static const CGColor3f COLOR_BLACK = {0.0f, 0.0f, 0.0f};
static const CGColor3f COLOR_BROWN = {0.59f, 0.29f, 0.0f};
static const CGColor3f COLOR_ORANGE = {1.0f, 0.5f, 0.0f};
static const CGColor3f COLOR_YELLOW = {1.0f, 1.0f, 0.0f};
static const CGColor3f COLOR_GREEN = {0.0f, 1.0f, 0.0f};
static const CGColor3f COLOR_BLUE = {0.0f, 0.0f, 1.0f};
static const CGColor3f COLOR_PINK = {1.0f, 0.2f, 0.8f};
static const CGColor3f COLOR_WHITE = {1.0f, 1.0f, 1.0f};
static const CGColor3f COLOR_DARK_GREEN = {0.0f, 0.3f, 0.0f};
static const CGColor3f COLOR_SKIN = {1.0f, 0.8f, 0.6f};
static const CGColor4f COLOR_BLUE_TRANS = {0.0f, 0.0f, 1.0f, 0.5f};

/** Umschalten einiger Funktionen */
GLboolean showNormal = GL_FALSE;
GLboolean showLight = GL_TRUE;
GLboolean showFixedLight = GL_TRUE;
GLboolean showSpotlight = GL_TRUE;

/**
 * Zeichnet ein Zylinder mit der uebergebenen Farbe
 * 
 * @param[in] color, Farbe des Quadrats
 */
static void drawCylinderTransparent(const CGColor4f color)
{
    glEnable(GL_BLEND);
    glDisable(GL_CULL_FACE);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    CGColor4f newCol = {color[0] * brightnessScaling, color[1] * brightnessScaling, color[2] * brightnessScaling, color[3]};
    /* Material des Wuerfels */
    float matShininess[] = {5.0f};

    /* Setzen der Material-Parameter */
    glMaterialfv(GL_FRONT, GL_AMBIENT, newCol);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, newCol);
    glMaterialfv(GL_FRONT, GL_SPECULAR, newCol);
    glMaterialfv(GL_FRONT, GL_SHININESS, matShininess);

    const GLfloat radius = 0.5f;
    const GLfloat height = 1.0f;

    glPushMatrix();
    {
        glRotatef(-90, 1.0f, 0.0f, 0.0f);
        /* Quadric erzuegen */
        GLUquadricObj *qobj = gluNewQuadric();

        if (qobj != 0)
        {
            glColor3f(color[0], color[1], color[2]);
            /* Gefuellt darstellen */
            gluQuadricDrawStyle(qobj, GLU_FILL);
            /* Scheibe zeichnen */
            gluCylinder(qobj, radius, radius, height, SLICES, SLICES);

            gluQuadricNormals(qobj, GLU_SMOOTH);
            /* Loeschen des Quadrics nicht vergessen */
            gluDeleteQuadric(qobj);
        }
        else
        {
            printf("FAIL\n");
        }
    }
    glPopMatrix();
    glDisable(GL_BLEND);
    glEnable(GL_CULL_FACE);
}

/**
 * Zeichnet eine Kugel mit der uebergebenen Farbe
 * 
 * @param[in] color, Farbe des Quadrats
 */
static void drawSphere(const CGColor3f color)
{

    CGColor3f newCol = {color[0] * brightnessScaling,
                        color[1] * brightnessScaling,
                        color[2] * brightnessScaling};
    /* Material des Wuerfels */
    float matShininess[] = {128.0f};

    /* Setzen der Material-Parameter */
    glMaterialfv(GL_FRONT, GL_AMBIENT, newCol);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, newCol);
    glMaterialfv(GL_FRONT, GL_SPECULAR, newCol);
    glMaterialfv(GL_FRONT, GL_SHININESS, matShininess);

    const GLfloat radius = 0.5f;

    glPushMatrix();
    {
        glRotatef(-90, 1.0f, 0.0f, 0.0f);
        /* Quadric erzuegen */
        GLUquadricObj *qobj = gluNewQuadric();

        if (qobj != 0)
        {
            glColor3f(color[0], color[1], color[2]);
            /* Gefuellt darstellen */
            gluQuadricDrawStyle(qobj, GLU_FILL);
            /* Scheibe zeichnen */
            gluSphere(qobj, radius, SLICES, SLICES);
            gluQuadricNormals(qobj, GLU_SMOOTH);
            /* Loeschen des Quadrics nicht vergessen */
            gluDeleteQuadric(qobj);
        }
        else
        {
            printf("FAIL\n");
        }
    }
    glPopMatrix();
}

/**
 * Zeichnet einen Kreis mit der uebergebenen Farbe
 * 
 * @param[in] color, Farbe des Quadrats
 */
static void drawCircle(const CGColor3f color, GLfloat radius)
{
    CGColor3f newCol = {color[0] * brightnessScaling, color[1] * brightnessScaling, color[2] * brightnessScaling};
    /* Material des Wuerfels */
    float matShininess[] = {100.0f};

    /* Setzen der Material-Parameter */
    glMaterialfv(GL_FRONT, GL_AMBIENT, newCol);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, newCol);
    glMaterialfv(GL_FRONT, GL_SPECULAR, newCol);
    glMaterialfv(GL_FRONT, GL_SHININESS, matShininess);
    glPushMatrix();
    {
        glRotatef(-90, 1.0f, 0.0f, 0.0f);
        /* Quadric erzuegen */
        GLUquadricObj *qobj = gluNewQuadric();

        if (qobj != 0)
        {
            glColor3f(color[0], color[1], color[2]);
            /* Gefuellt darstellen */
            gluQuadricDrawStyle(qobj, GLU_FILL);
            /* Scheibe zeichnen */
            gluDisk(qobj, 0, radius, SLICES, LOOPS);
            gluQuadricNormals(qobj, GLU_SMOOTH);
            /* Loeschen des Quadrics nicht vergessen */
            gluDeleteQuadric(qobj);
        }
        else
        {
            printf("FAIL\n");
        }
    }
    glPopMatrix();
}

/**
 * Zeichnet ein Quadrat mit der uebergebenen Farbe, 
 * mit Seitenlaenge 1 um den Ursprung rum
 * 
 * @param[in] color, Farbe des Quadrats
 */
static void drawRect(const CGColor3f color)
{
    CGColor3f newCol = {color[0] * brightnessScaling, color[1] * brightnessScaling, color[2] * brightnessScaling};
    /* Material des Wuerfels */
    float matShininess[] = {100.0f};

    /* Setzen der Material-Parameter */
    glMaterialfv(GL_FRONT, GL_AMBIENT, newCol);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, newCol);
    glMaterialfv(GL_FRONT, GL_SPECULAR, newCol);
    glMaterialfv(GL_FRONT, GL_SHININESS, matShininess);

    glBegin(GL_QUADS);
    {
        glNormal3f(0.0f, 1.0f, 0.0f);
        glColor3f(color[0], color[1], color[2]);
        glVertex3f(-0.5f, 0.0f, -0.5f);
        glVertex3f(-0.5f, 0.0f, 0.5f);
        glVertex3f(0.5f, 0.0f, 0.5f);
        glVertex3f(0.5f, 0.0f, -0.5f);
    }
    glEnd();
}

/**
 * Zeichnet eine Linie mit der uebergebenen Farbe,
 * mit der Laenge 1 entlang der z-Achse
 * 
 * @param[in] color, Farbe des Quadrats
 */
static void drawLine(const CGColor3f color)
{
    CGColor3f newCol = {color[0] * brightnessScaling,
                        color[1] * brightnessScaling,
                        color[2] * brightnessScaling};
    /* Material des Wuerfels */
    float matShininess[] = {5.0f};

    /* Setzen der Material-Parameter */
    glMaterialfv(GL_FRONT, GL_AMBIENT, newCol);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, newCol);
    glMaterialfv(GL_FRONT, GL_SPECULAR, newCol);
    glMaterialfv(GL_FRONT, GL_SHININESS, matShininess);

    glBegin(GL_LINES);
    {
        glColor3f(color[0], color[1], color[2]);
        glVertex3f(0.0f, 0.0f, -0.5f);
        glVertex3f(0.0f, 0.0f, 0.5f);
    }
    glEnd();
}

/**
 * Zeichnet Linien in Form eines Quadrat mit der uebergebenen Farbe, 
 * mit Seitenlaenge 1 um den Ursprung rum
 * 
 * @param[in] color, Farbe des Quadrats
 */
static void drawLines(const CGColor3f color)
{
    CGColor3f newCol = {color[0] * brightnessScaling,
                        color[1] * brightnessScaling,
                        color[2] * brightnessScaling};
    /* Material des Wuerfels */
    float matShininess[] = {5.0f};

    /* Setzen der Material-Parameter */
    glMaterialfv(GL_FRONT, GL_AMBIENT, newCol);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, newCol);
    glMaterialfv(GL_FRONT, GL_SPECULAR, newCol);
    glMaterialfv(GL_FRONT, GL_SHININESS, matShininess);

    glBegin(GL_LINE_LOOP);
    {
        glColor3f(color[0], color[1], color[2]);
        glVertex3f(-0.5f, 0.0f, -0.5f);
        glVertex3f(-0.5f, 0.0f, 0.5f);
        glVertex3f(0.5f, 0.0f, 0.5f);
        glVertex3f(0.5f, 0.0f, -0.5f);
    }
    glEnd();
}

static void drawNormal(const CGColor3f color)
{
    glPushMatrix();
    {
        glRotatef(90, 1.0f, 0.0f, 0.0f);
        glScalef(0.5f, 1.0f, 1.0f);
        drawLine(color);
    }
    glPopMatrix();
}

/*
 * Zeichnet ein Dreieck mit Katenlaengen 1
 * 
 * @param[in] CGColor3f, Farbe des Dreiecks
 */
static void drawTriangle(const CGColor4f color, GLboolean transparent)
{
    if (transparent)
    {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    }
    CGColor4f newCol = {color[0] * brightnessScaling, color[1] * brightnessScaling, color[2] * brightnessScaling, color[3]};
    /* Material des Wuerfels */
    float matShininess[] = {5.0f};

    /* Setzen der Material-Parameter */
    glMaterialfv(GL_FRONT, GL_AMBIENT, newCol);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, newCol);
    glMaterialfv(GL_FRONT, GL_SPECULAR, newCol);
    glMaterialfv(GL_FRONT, GL_SHININESS, matShininess);

    glBegin(GL_TRIANGLES);
    {
        glNormal3f(0.0f, 1.0f, 0.0f);
        glColor4f(color[0], color[1], color[2], color[3]);
        glVertex3f(-0.5f, 0.0f, TRIANGLE_HEIGHT_LOWER_THIRD);
        glVertex3f(0.5f, 0.0f, TRIANGLE_HEIGHT_LOWER_THIRD);
        glVertex3f(0.0f, 0.0f, -2 * TRIANGLE_HEIGHT_LOWER_THIRD);
    }
    glEnd();

    if (transparent)
    {
        glDisable(GL_BLEND);
    }
}

/**
 * Zeichnet eine Flaeche mit der uebergebenen Farbe, 
 * mit Seitenlaenge 1 um den Ursprung rum.
 * Zusammengesetzt aus AMOUNT_PIXEL_PER_BLOCK kleineren Quadraten
 * 
 * @param[in] color, Farbe des Quadrats
 */
static void drawPlane(const CGColor3f color)
{
    GLint i = 0;
    GLint m = 0;

    glPushMatrix();
    {
        // Verschieben des Mittelpunktes der Grundflaeche in den Ursprung
        glTranslatef(-0.5f + 0.5f * 1.0f / AMOUNT_PIXEL_PER_BLOCK, 0.0f, -0.5f + 0.5f * 1.0f / AMOUNT_PIXEL_PER_BLOCK);
        for (i = 0; i < AMOUNT_PIXEL_PER_BLOCK; i++)
        {
            glPushMatrix();
            {
                glTranslatef(0.0f, 0.0f, (1.0f / AMOUNT_PIXEL_PER_BLOCK) * i);
                for (m = 0; m < AMOUNT_PIXEL_PER_BLOCK; m++)
                {
                    glPushMatrix();
                    {
                        glTranslatef((1.0f / AMOUNT_PIXEL_PER_BLOCK) * m, 0.0f, 0.0f);
                        glScalef(1.0f / AMOUNT_PIXEL_PER_BLOCK, 1.0f, 1.0f / AMOUNT_PIXEL_PER_BLOCK);
                        drawRect(color);
                    }
                    glPopMatrix();
                }
            }
            glPopMatrix();
        }
    }
    glPopMatrix();
}

/**
 * Zeichnet ein Dreieck mit der uebergebenen Farbe, 
 * mit Seitenlaenge 1 um den Ursprung rum
 * Zusammengesetzt aus AMOUNT_TRIS_PER_TRIANGLE kleineren Dreiecken
 * 
 * @param[in] color, Farbe des Quadrats
 */
static void drawTriangles(const CGColor4f color, GLboolean transparent)
{
    GLint i = 0;
    GLint m = 0;

    GLint amountUpTris = AMOUNT_TRIS_PER_TRIANGLE;
    glPushMatrix();
    {
        // Verschieben des Mittelpunktes der Grundflaeche in den Ursprung
        glTranslatef(-0.5f + (1.0f / AMOUNT_TRIS_PER_TRIANGLE) / 2, 0.0f, -TRIANGLE_HEIGHT_LOWER_THIRD + ((1.0f / AMOUNT_TRIS_PER_TRIANGLE) * 2 * TRIANGLE_HEIGHT_LOWER_THIRD));
        glScalef(1.0f / AMOUNT_TRIS_PER_TRIANGLE, 1.0f, 1.0f / AMOUNT_TRIS_PER_TRIANGLE);
        for (m = 0; m < AMOUNT_TRIS_PER_TRIANGLE; m++)
        {
            glPushMatrix();
            {
                glTranslatef(0.5f * m, 0.0f, (TRIANGLE_HEIGHT * m));
                for (i = 0; i < 2 * amountUpTris - 1; i++)
                {
                    glPushMatrix();
                    {
                        glTranslatef(i * 0.5f, 0.0f, 0.0f);
                        if (i % 2 == 0)
                        {
                            glTranslatef(0.0f, 0.0f, -TRIANGLE_HEIGHT_LOWER_THIRD);
                            glRotatef(60, 0.0f, 1.0f, 0.0f);
                        }
                        drawTriangle(color, transparent);
                    }
                    glPopMatrix();
                }
                amountUpTris--;
            }
            glPopMatrix();
        }
    }
    glPopMatrix();
}

/**
 * Zeichnet den SpielBlock "Mauer"
 * 
 */
static void drawWall(void)
{
    GLint i = 0;
    GLint m = 0;

    GLint amountEvenVertLines = EVEN_ROWS_WALL - 1;
    GLint amountOddVertLines = ODD_ROWS_WALL - 1;
    GLint amountHorLines = AMOUNT_ROWS_WALL - 1;

    glPushMatrix();
    {
        drawPlane(COLOR_DARK_GREY);
        glPushMatrix();
        {
            // Verschieben des Mittelpunktes der Grundflaeche in den Ursprung
            glTranslatef(-0.5f, Z_OFFSET, -0.5f);
            for (m = 0; m < AMOUNT_ROWS_WALL; m++)
            {
                glPushMatrix();
                {
                    glTranslatef(0.0f, 0.0f, (1.0f / AMOUNT_ROWS_WALL) * m);
                    if (m % 2 == 0)
                    {
                        glPushMatrix();
                        {
                            glTranslatef(1.0f / EVEN_ROWS_WALL, 0.0f, (1.0f / AMOUNT_ROWS_WALL) / 2.0f);
                            glScalef(1.0f, 1.0f, 1.0f / AMOUNT_ROWS_WALL);
                            for (i = 0; i < amountEvenVertLines; i++)
                            {
                                glPushMatrix();
                                {
                                    glTranslatef((1.0f / EVEN_ROWS_WALL) * i, 0.0f, 0.0f);
                                    drawLine(COLOR_LIGHT_GREY);
                                }
                                glPopMatrix();
                            }
                        }
                        glPopMatrix();
                    }
                    else
                    {
                        glPushMatrix();
                        {
                            glTranslatef(1.0f / ODD_ROWS_WALL, 0.0f, (1.0f / AMOUNT_ROWS_WALL) / 2.0f);
                            glScalef(1.0f, 1.0f, 1.0f / AMOUNT_ROWS_WALL);
                            for (i = 0; i < amountOddVertLines; i++)
                            {
                                glPushMatrix();
                                {
                                    glTranslatef((1.0f / ODD_ROWS_WALL) * i, 0.0f, 0.0f);
                                    drawLine(COLOR_LIGHT_GREY);
                                }
                                glPopMatrix();
                            }
                        }
                        glPopMatrix();
                    }
                }
                glPopMatrix();
            }

            glPushMatrix();
            {
                glTranslatef(0.5f, 0.0f, 1.0f / AMOUNT_ROWS_WALL);
                for (i = 0; i < amountHorLines; i++)
                {
                    glPushMatrix();
                    {
                        glTranslatef(0.0f, 0.0f, (1.0f / AMOUNT_ROWS_WALL) * i);
                        glRotatef(90, 0.0f, 1.0f, 0.0f);
                        drawLine(COLOR_LIGHT_GREY);
                    }
                    glPopMatrix();
                }
            }
            glPopMatrix();
        }
        glPopMatrix();
        drawLines(COLOR_BLACK);
    }
    glPopMatrix();
}

/**
 * Zeichnet einen Wuerfel mit Kantenlaenge 1.
 */
static void
drawWallCube(GLboolean normal)
{
    /* Unterseite */
    glPushMatrix();
    {
        glRotatef(180, 1.0f, 0.0f, 0.0f);
        drawWall();
        drawLines(COLOR_BLACK);
        if (normal)
        {
            drawNormal(COLOR_DARK_GREY);
        }
    }
    glPopMatrix();

    /* rechte Seitenflaeche */
    glPushMatrix();
    {
        glTranslatef(0.5f, 0.5f, 0.0f);
        glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
        glRotatef(-90.0f, 0.0f, 0.0f, 1.0f);
        drawWall();
        drawLines(COLOR_BLACK);
        if (normal)
        {
            drawNormal(COLOR_DARK_GREY);
        }
    }
    glPopMatrix();

    /* linke Seite */
    glPushMatrix();
    {
        glTranslatef(-0.5f, 0.5f, 0.0f);
        glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
        glRotatef(-90.0f, 0.0f, 0.0f, 1.0f);
        glRotatef(180, 1.0f, 0.0f, 0.0f);
        drawWall();
        drawLines(COLOR_BLACK);
        if (normal)
        {
            drawNormal(COLOR_DARK_GREY);
        }
    }
    glPopMatrix();

    /* hintere Seite */
    glPushMatrix();
    {
        glTranslatef(0.0f, 0.5f, -0.5f);
        glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
        glRotatef(180, 1.0f, 0.0f, 0.0f);
        drawWall();
        drawLines(COLOR_BLACK);
        if (normal)
        {
            drawNormal(COLOR_DARK_GREY);
        }
    }
    glPopMatrix();

    /* vordere Seite */
    glPushMatrix();
    {
        glTranslatef(0.0f, 0.5f, 0.5f);
        glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
        drawWall();
        drawLines(COLOR_BLACK);
        if (normal)
        {
            drawNormal(COLOR_DARK_GREY);
        }
    }
    glPopMatrix();

    /* Bodenflaeche */
    glPushMatrix();
    {
        glTranslatef(0.0f, 1.0f, 0.0f);
        drawWall();
        drawLines(COLOR_BLACK);
        if (normal)
        {
            drawNormal(COLOR_DARK_GREY);
        }
    }
    glPopMatrix();
}

/**
 * Zeichnet eine Hauswand
 * 
 */
static void drawHouseWall(void)
{
    /* Material des Wuerfels */
    float matShininess[] = {20.0f};

    /* Setzen der Material-Parameter */
    glMaterialfv(GL_FRONT, GL_AMBIENT, COLOR_WHITE);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, COLOR_WHITE);
    glMaterialfv(GL_FRONT, GL_SPECULAR, COLOR_WHITE);
    glMaterialfv(GL_FRONT, GL_SHININESS, matShininess);
    drawPlane(COLOR_WHITE);
}

/**
 * Zeichnet das Dach des Hauses
 * Farbe Gruen -> Haus offen
 * Pink -> geschlossen
 * 
 * @param[in] open, Haus offen/geschlossen
 */
static void drawRoof(GLboolean open, GLboolean normal)
{

    glPushMatrix();
    {

        //Vorderseite
        glPushMatrix();
        {
            glTranslatef(0.0f, 0.0f, -0.5f);
            glRotatef(-ANGLE_ROOF, 1.0f, 0.0f, 0.0f);
            glTranslatef(0.0f, 0.0f, TRIANGLE_HEIGHT_LOWER_THIRD);
            open ? drawTriangles(COLOR_DARK_GREEN, GL_FALSE) : drawTriangles(COLOR_PINK, GL_FALSE);
            if (normal)
            {
                open ? drawNormal(COLOR_DARK_GREEN) : drawNormal(COLOR_PINK);
            }
        }
        glPopMatrix();

        //Hinterseite
        glPushMatrix();
        {
            glRotatef(180, 0.0f, 1.0f, 0.0f);
            glTranslatef(0.0f, 0.0f, -0.5f);
            glRotatef(-ANGLE_ROOF, 1.0f, 0.0f, 0.0f);
            glTranslatef(0.0f, 0.0f, TRIANGLE_HEIGHT_LOWER_THIRD);
            open ? drawTriangles(COLOR_DARK_GREEN, GL_FALSE) : drawTriangles(COLOR_PINK, GL_FALSE);
            if (normal)
            {
                open ? drawNormal(COLOR_DARK_GREEN) : drawNormal(COLOR_PINK);
            }
        }
        glPopMatrix();

        //Seiten
        glPushMatrix();
        {
            glRotatef(90, 0.0f, 1.0f, 0.0f);
            glTranslatef(0.0f, 0.0f, -0.5f);
            glRotatef(-ANGLE_ROOF, 1.0f, 0.0f, 0.0f);
            glTranslatef(0.0f, 0.0f, TRIANGLE_HEIGHT_LOWER_THIRD);
            open ? drawTriangles(COLOR_DARK_GREEN, GL_FALSE) : drawTriangles(COLOR_PINK, GL_FALSE);
            if (normal)
            {
                open ? drawNormal(COLOR_DARK_GREEN) : drawNormal(COLOR_PINK);
            }
        }
        glPopMatrix();

        glPushMatrix();
        {
            glRotatef(270, 0.0f, 1.0f, 0.0f);
            glTranslatef(0.0f, 0.0f, -0.5f);
            glRotatef(-ANGLE_ROOF, 1.0f, 0.0f, 0.0f);
            glTranslatef(0.0f, 0.0f, TRIANGLE_HEIGHT_LOWER_THIRD);
            open ? drawTriangles(COLOR_DARK_GREEN, GL_FALSE) : drawTriangles(COLOR_PINK, GL_FALSE);
            if (normal)
            {
                open ? drawNormal(COLOR_DARK_GREEN) : drawNormal(COLOR_PINK);
            }
        }
        glPopMatrix();
    }
    glPopMatrix();
}

/**
 * Zeichnet den SpielBlock "Kiste"
 * 
 */
static void drawBox(void)
{

    // Hoehe der Balken Oben und unten & Breite der Balken in der Mitte
    GLfloat shortSide = 1.0f / AMOUNT_BOXES_ROW;
    // Hoehe der Balken in der Mitte
    GLfloat height = 1.0f - 2.0f * shortSide;
    GLint i = 0;

    glPushMatrix();
    {
        drawPlane(COLOR_BROWN);
        glPushMatrix();
        {
            glTranslatef(-0.5f, Z_OFFSET, -0.5f);
            glPushMatrix();
            {
                glTranslatef(shortSide, 0.0f, 0.5f);
                glScalef(1.0f, 1.0f, height);
                for (i = 0; i < AMOUNT_BOXES_ROW - 1; i++)
                {
                    glPushMatrix();
                    {
                        glTranslatef(i * shortSide, 0.0f, 0.0f);
                        drawLine(COLOR_BLACK);
                    }
                    glPopMatrix();
                }
            }
            glPopMatrix();

            glPushMatrix();
            {
                glTranslatef(0.5f, 0.0f, shortSide);
                glRotatef(90, 0.0f, 1.0f, 0.0f);
                drawLine(COLOR_BLACK);
            }
            glPopMatrix();

            glPushMatrix();
            {
                glTranslatef(0.5f, 0.0f, 1.0f - shortSide);
                glRotatef(90, 0.0f, 1.0f, 0.0f);
                drawLine(COLOR_BLACK);
            }
            glPopMatrix();
        }
        glPopMatrix();
    }
    glPopMatrix();
}

/**
 * Zeichnet einen Wuerfel mit Kantenlaenge 1.
 */
static void
drawBoxCube(GLboolean normal)
{

    /* Unterseite */
    glPushMatrix();
    {
        glRotatef(180, 1.0f, 0.0f, 0.0f);
        drawBox();
        drawLines(COLOR_BLACK);
        if (normal)
        {
            drawNormal(COLOR_BROWN);
        }
    }
    glPopMatrix();

    /* rechte Seitenflaeche */
    glPushMatrix();
    {
        glTranslatef(0.5f, 0.5f, 0.0f);
        glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
        glRotatef(-90.0f, 0.0f, 0.0f, 1.0f);
        drawBox();
        drawLines(COLOR_BLACK);
        if (normal)
        {
            drawNormal(COLOR_BROWN);
        }
    }
    glPopMatrix();

    /* linke Seite */
    glPushMatrix();
    {
        glTranslatef(-0.5f, 0.5f, 0.0f);
        glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
        glRotatef(-90.0f, 0.0f, 0.0f, 1.0f);
        glRotatef(180, 1.0f, 0.0f, 0.0f);
        drawBox();
        drawLines(COLOR_BLACK);
        if (normal)
        {
            drawNormal(COLOR_BROWN);
        }
    }
    glPopMatrix();

    /* hintere Seite */
    glPushMatrix();
    {
        glTranslatef(0.0f, 0.5f, -0.5f);
        glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
        glRotatef(180, 1.0f, 0.0f, 0.0f);
        drawBox();
        drawLines(COLOR_BLACK);
        if (normal)
        {
            drawNormal(COLOR_BROWN);
        }
    }
    glPopMatrix();

    /* vordere Seite */
    glPushMatrix();
    {
        glTranslatef(0.0f, 0.5f, 0.5f);
        glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
        drawBox();
        drawLines(COLOR_BLACK);
        if (normal)
        {
            drawNormal(COLOR_BROWN);
        }
    }
    glPopMatrix();

    /* Bodenflaeche */
    glPushMatrix();
    {
        glTranslatef(0.0f, 1.0f, 0.0f);
        drawBox();
        drawLines(COLOR_BLACK);
        if (normal)
        {
            drawNormal(COLOR_BROWN);
        }
    }
    glPopMatrix();
}

/**
 * Zeichnet einen Wuerfel mit Kantenlaenge 1.
 */

/**
 * Zeichnet den Spielblock "Freies Feld"
 * 
 */
static void drawFreeCell(GLboolean normal)
{
    drawPlane(COLOR_LIGHT_GREY);
    glPushMatrix();
    {
        glTranslatef(0.0f, Z_OFFSET, 0.0f);
        drawLines(COLOR_BLACK);
    }
    glPopMatrix();
    if (normal)
    {
        drawNormal(COLOR_LIGHT_GREY);
    }
}

static void drawTetrahedron(GLboolean normal, GLboolean transparent)
{
    if (transparent)
    {
        glEnable(GL_BLEND);
        glDisable(GL_CULL_FACE);
    }
    else
    {
        glCallList(g_freeCellListID);
    }

    if (normal)
    {
        drawNormal(COLOR_LIGHT_GREY);
    }

    glPushMatrix();
    {
        glScalef(0.8f, 0.8f, 0.8f);
        glPushMatrix();
        {
            glRotatef(180, 1.0f, 0.0f, 0.0f);
            glRotatef(60, 0.0f, 1.0f, 0.0f);
            transparent ? drawTriangles(COLOR_BLUE_TRANS, transparent) : drawTriangles(COLOR_DARK_BLUE, transparent);
            if (normal)
            {
                drawNormal(COLOR_DARK_BLUE);
            }
        }
        glPopMatrix();

        glPushMatrix();
        {
            glTranslatef(0.0f, 0.0f, -TRIANGLE_HEIGHT_LOWER_THIRD);
            glRotatef(ANGLE_TETRAHEDON, 1.0f, 0.0f, 0.0f);
            glTranslatef(0.0f, 0.0f, -TRIANGLE_HEIGHT_LOWER_THIRD);
            glRotatef(180, 1.0f, 0.0f, 0.0f);
            glRotatef(120, 0.0f, 1.0f, 0.0f);
            transparent ? drawTriangles(COLOR_BLUE_TRANS, transparent) : drawTriangles(COLOR_DARK_BLUE, transparent);
            if (normal)
            {
                drawNormal(COLOR_DARK_BLUE);
            }
        }
        glPopMatrix();

        glPushMatrix();
        {
            glRotatef(120.0f, 0.0f, 1.0f, 0.0f);
            glTranslatef(0.0f, 0.0f, -TRIANGLE_HEIGHT_LOWER_THIRD);
            glRotatef(ANGLE_TETRAHEDON, 1.0f, 0.0f, 0.0f);
            glTranslatef(0.0f, 0.0f, -TRIANGLE_HEIGHT_LOWER_THIRD);
            glRotatef(180, 1.0f, 0.0f, 0.0f);
            transparent ? drawTriangles(COLOR_BLUE_TRANS, transparent) : drawTriangles(COLOR_DARK_BLUE, transparent);
            if (normal)
            {
                drawNormal(COLOR_DARK_BLUE);
            }
        }
        glPopMatrix();

        glPushMatrix();
        {
            glRotatef(-120.0f, 0.0f, 1.0f, 0.0f);
            glTranslatef(0.0f, 0.0f, -TRIANGLE_HEIGHT_LOWER_THIRD);
            glRotatef(ANGLE_TETRAHEDON, 1.0f, 0.0f, 0.0f);
            glTranslatef(0.0f, 0.0f, -TRIANGLE_HEIGHT_LOWER_THIRD);
            glRotatef(180, 1.0f, 0.0f, 0.0f);
            transparent ? drawTriangles(COLOR_BLUE_TRANS, transparent) : drawTriangles(COLOR_DARK_BLUE, transparent);
            if (normal)
            {
                drawNormal(COLOR_DARK_BLUE);
            }
        }
        glPopMatrix();
    }
    glPopMatrix();

    if (transparent)
    {
        glDisable(GL_BLEND);
        glEnable(GL_CULL_FACE);
    }
}

/**
 * Zeichnet das Haus, zusammengesetzt aus Dach und Hauswaenden
 * 
 */
static void drawHouse(GLboolean normal)
{
    glCallList(g_freeCellListID);

    /* rechte Seitenflaeche */
    glPushMatrix();
    {
        glTranslatef(0.5f, 0.5f, 0.0f);
        glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
        glRotatef(-90.0f, 0.0f, 0.0f, 1.0f);
        drawHouseWall();
        if (normal)
        {
            drawNormal(COLOR_WHITE);
        }
    }
    glPopMatrix();

    /* linke Seite */
    glPushMatrix();
    {
        glTranslatef(-0.5f, 0.5f, 0.0f);
        glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
        glRotatef(-90.0f, 0.0f, 0.0f, 1.0f);
        glRotatef(180, 1.0f, 0.0f, 0.0f);
        drawHouseWall();
        if (normal)
        {
            drawNormal(COLOR_WHITE);
        }
    }
    glPopMatrix();

    /* hintere Seite */
    glPushMatrix();
    {
        glTranslatef(0.0f, 0.5f, -0.5f);
        glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
        glRotatef(180, 1.0f, 0.0f, 0.0f);
        drawHouseWall();
        if (normal)
        {
            drawNormal(COLOR_WHITE);
        }
    }
    glPopMatrix();

    /* vordere Seite */
    glPushMatrix();
    {
        glTranslatef(0.0f, 0.5f, 0.5f);
        glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
        drawHouseWall();
        if (normal)
        {
            drawNormal(COLOR_WHITE);
        }
    }
    glPopMatrix();

    /* Bodenflaeche */
    glPushMatrix();
    {
        glTranslatef(0.0f, 1.0f, 0.0f);
        drawHouseWall();
        if (normal)
        {
            drawNormal(COLOR_WHITE);
        }
    }
    glPopMatrix();

    glPushMatrix();
    {
        glTranslatef(0.0f, 1.0f, 0.0f);
        g_allLevels[getLevelIndex()].amountMoveableTriangle == 0 ? drawRoof(GL_TRUE, normal) : drawRoof(GL_FALSE, normal);
    }
    glPopMatrix();
}

/**
 * Zeichnet eine geschlossene Tuer
 * 
 */
static void drawClosedDoor(void)
{
    glCallList(g_freeCellListID);
    GLint i = 0;
    for (i = 0; i < 2; i++)
    {
        glPushMatrix();
        {
            glRotatef(90, 0.0f, 1.0f, 0.0f);
            glTranslatef(0.5f * i, 0.0f, 0.0f);
            glPushMatrix();
            {
                glTranslatef(-1.0f / 4, 0.0f, 0.0f);
                glScalef(0.5f, 1.0f, 1.0f / 3);
                glRotatef(180, 0.0f, 0.0f, 1.0f);
                drawRect(COLOR_BROWN);
                if (showNormal)
                {
                    drawNormal(COLOR_BROWN);
                }
                glPushMatrix();
                {
                    glTranslatef(0.0f, -Z_OFFSET, 0.0f);
                    drawLines(COLOR_BLACK);
                }
                glPopMatrix();
            }
            glPopMatrix();

            glPushMatrix();
            {
                glTranslatef(-1.0f / 4, 1.0f, 0.0f);
                glScalef(0.5f, 1.0f, 1.0f / 3);
                drawRect(COLOR_BROWN);
                if (showNormal)
                {
                    drawNormal(COLOR_BROWN);
                }
                glPushMatrix();
                {
                    glTranslatef(0.0f, Z_OFFSET, 0.0f);
                    drawLines(COLOR_BLACK);
                }
                glPopMatrix();
            }
            glPopMatrix();

            glPushMatrix();
            {
                glTranslatef(-1.0f / 4, 0.5f, 1.0f / 6);
                glScalef(0.5f, 1.0f, 1.0f / 3);
                glRotatef(90, 1.0f, 0.0f, 0.0f);
                drawRect(COLOR_BROWN);
                if (showNormal)
                {
                    drawNormal(COLOR_BROWN);
                }
                glPushMatrix();
                {
                    glTranslatef(0.0f, 0.0f, Z_OFFSET);
                    drawLines(COLOR_BLACK);
                }
                glPopMatrix();
            }
            glPopMatrix();

            glPushMatrix();
            {
                glTranslatef(-1.0f / 4, 0.5f, -1.0f / 6);
                glScalef(0.5f, 1.0f, 1.0f / 3);
                glRotatef(270, 1.0f, 0.0f, 0.0f);
                drawRect(COLOR_BROWN);
                if (showNormal)
                {
                    drawNormal(COLOR_BROWN);
                }
                glPushMatrix();
                {
                    glTranslatef(0.0f, 0.0f, -Z_OFFSET);
                    drawLines(COLOR_BLACK);
                }
                glPopMatrix();
            }
            glPopMatrix();

            glPushMatrix();
            {
                glTranslatef(0.0f, 0.5f, 0.0f);
                glScalef(0.5f, 1.0f, 1.0f / 3);
                glRotatef(270, 0.0f, 0.0f, 1.0f);
                drawRect(COLOR_BROWN);
                if (showNormal)
                {
                    drawNormal(COLOR_BROWN);
                }
            }
            glPopMatrix();

            glPushMatrix();
            {
                glTranslatef(-0.5f, 0.5f, 0.0f);
                glScalef(0.5f, 1.0f, 1.0f / 3);
                glRotatef(90, 0.0f, 0.0f, 1.0f);
                drawRect(COLOR_BROWN);
                if (showNormal)
                {
                    drawNormal(COLOR_BROWN);
                }
            }
            glPopMatrix();
        }
        glPopMatrix();
    }
}

/**
 * Zeichnet das Zielfeld fuer die Tetraeder
 * 
 */
static void drawTarget(GLboolean normal)
{
    drawPlane(COLOR_DARK_BLUE);
    if (normal)
    {
        drawNormal(COLOR_DARK_BLUE);
    }
}

/**
 * Zeichnet ein Portal
 * 
 */
static void drawPortal(void)
{
    glPushMatrix();
    {
        glScalef(0.4f, 0.4f, 0.4f);
        glTranslatef(0.0f, 0.01f, 0.0f);
        drawCircle(COLOR_RED, RADIUS_1);

        glTranslatef(0.0f, 0.01f, 0.0f);
        drawCircle(COLOR_ORANGE, RADIUS_2);

        glTranslatef(0.0f, 0.01f, 0.0f);
        drawCircle(COLOR_YELLOW, RADIUS_3);

        glTranslatef(0.0f, 0.01f, 0.0f);
        drawCircle(COLOR_GREEN, RADIUS_4);

        glTranslatef(0.0f, 0.01f, 0.0f);
        drawCircle(COLOR_BLUE, RADIUS_5);
    }
    glPopMatrix();
}

/**
 * Zeichnet einen transparenten Tuerschalter
 * Zusammengesetzt aus 2 Tetraedern und 1 Zylinder
 * 
 */
static void drawDoorSwitchTransparent(void)
{
    glPushMatrix();
    {
        glScalef(1.75f, 1.75f, 1.75f);
        glPushMatrix();
        {
            glTranslatef(0.0f, (1.0f / 3) * TRIANGLE_HEIGHT_LOWER_THIRD, 0.0f);
            glScalef(1.0f / 3, 1.0f / 3, 1.0f / 3);
            glRotatef(45, 0.0f, 1.0f, 0.0f);
            glRotatef(90, 0.0f, 0.0f, 1.0f);
            glRotatef(-30, 0.0f, 1.0f, 0.0f);
            glTranslatef(0.0f, -0.5f, 0.0f);
            glPushMatrix();
            {
                glScalef(0.3f, 1.0f, 0.3f);
                drawCylinderTransparent(COLOR_BLUE_TRANS);
            }
            glPopMatrix();

            glPushMatrix();
            {
                glTranslatef(0.0f, 1.0f, 0.0f);
                showNormal ? glCallList(g_tetrahedronBlueTransListNormalID) : glCallList(g_tetrahedronBlueTransListID);
            }
            glPopMatrix();

            glPushMatrix();
            {
                glRotatef(180, 0.0f, 0.0f, 1.0f);
                glCallList(g_tetrahedronBlueTransListID);
                showNormal ? glCallList(g_tetrahedronBlueTransListNormalID) : glCallList(g_tetrahedronBlueTransListID);
            }
            glPopMatrix();
        }
        glPopMatrix();
    }
    glPopMatrix();
}

/**
 * Zeichnet die Spielfigur(Pushy)
 * 
 */
static void drawPlayer(void)
{
    const GLfloat limbs[3] = {1.0f / 5.0f, 1.0f / 2.0f, 1.0f / 5.0f};
    const GLfloat headRad = 0.55f;
    glPushMatrix();
    {
        glRotatef(90 * lastMoveDir, 0.0f, 1.0f, 0.0f);
        //Arme
        glPushMatrix();
        {
            glTranslatef(0.2f, 0.85f, 0.0f);
            glRotatef(30, 0.0f, 0.0f, 1.0f);
            glTranslatef(0.0f, -0.15f, 0.0f);
            glScalef(limbs[0], limbs[1], limbs[2]);
            drawSphere(COLOR_RED);
        }
        glPopMatrix();

        glPushMatrix();
        {
            glTranslatef(-0.2f, 0.85f, 0.0f);
            glRotatef(-30, 0.0f, 0.0f, 1.0f);
            glTranslatef(0.0f, -0.15f, 0.0f);
            glScalef(limbs[0], limbs[1], limbs[2]);
            drawSphere(COLOR_RED);
        }
        glPopMatrix();

        //Beine
        glPushMatrix();
        {
            glTranslatef(0.05f, 0.3f, 0.0f);
            glRotatef(30, 0.0f, 0.0f, 1.0f);
            glTranslatef(0.0f, -0.15f, 0.0f);
            glScalef(limbs[0], limbs[1], limbs[2]);
            drawSphere(COLOR_DARK_BLUE);
        }
        glPopMatrix();

        glPushMatrix();
        {
            glTranslatef(-0.05f, 0.3f, 0.0f);
            glRotatef(-30, 0.0f, 0.0f, 1.0f);
            glTranslatef(0.0f, -0.15f, 0.0f);
            glScalef(limbs[0], limbs[1], limbs[2]);
            drawSphere(COLOR_DARK_BLUE);
        }
        glPopMatrix();

        //Rumpf
        glPushMatrix();
        {
            glTranslatef(0.0f, 0.75f, 0.0f);
            glScalef(1.0f / 2.5f, 1.0f, 1.0f / 2.5f);
            drawSphere(COLOR_RED);
        }
        glPopMatrix();

        //Kopf
        glPushMatrix();
        {
            glTranslatef(0.0f, 1.2f, 0.0f);
            glScalef(headRad, headRad, headRad);
            drawSphere(COLOR_SKIN);
        }
        glPopMatrix();
    }
    glPopMatrix();
}

/**
* Gibt den Hilfetext aus.
*/
static void drawHelp()
{
    int i = 0;
    int size = 16;

    GLfloat color[3] = {1.0f, 0.2f, 0.8f};

    char *help[] = {"Hilfe",
                    "F1 - Wireframe an/aus",
                    "F2 - Normalen an/aus",
                    "F3 - Punktlichtquelle an/aus",
                    "F4 - Spotlight an/aus",
                    "F5 - Beleuchtung an/aus",
                    "F6 - Vollbildmodus an/aus",
                    "1/2/3 - Levelauswahl",
                    "Pfeiltasten - Pushy bewegen",
                    "r/R - Neustart des Levels",
                    "a/A - Animation (Pushy) an/aus",
                    "c/C - Helmkamera an/aus",
                    "u,U/o,O - rein-/rauszoomen 3D-Ansicht",
                    "i,I/j,J/k,K/l,L - Bewegen der Kamera 3D-Ansicht",
                    "h/H - Hilfe an/aus",
                    "ESC/q/Q - Ende"};

    drawString(0.4f, 0.1f, color, help[0]);

    for (i = 1; i < size; ++i)
    {
        drawString(0.35f, 0.15f + i * 0.05f, color, help[i]);
    }
}

/**
 * Gibt Gewonnen oder Verloren aus
 * 
 */
static void drawEndOfGame()
{

    CGColor3f pink = {1.0f, 0.3f, 0.7f};
    // Spiel verloren
    if ((g_gameState == timesUp) && !getShowHelp())
    {
        drawString(TEXT_END_X, TEXT_END_Y, pink, "VERLOREN!");
    }
    // Spiel gewonnen
    else if ((g_gameState == won) && !getShowHelp())
    {
        drawString(TEXT_END_X, TEXT_END_Y, pink, "GEWONNEN!");
    }
}

/**
 * Zeichnet die Spielfigur an der richtigen Stelle
 * 
 */
void drawPushy(void)
{
    glPushMatrix();
    {
        glTranslatef(getPushyPosFloats()[1], 0.0f, getPushyPosFloats()[0]);
        drawPlayer();
    }
    glPopMatrix();
}

static void drawLevel(void)
{
    CGPoint2f doorSwitchPos;
    glPushMatrix();
    {
        /* glTranslatef(0.0f, 100.0f, 0.0f); */
        // Gewaehltes Level
        GLuint currIndexLevel = getLevelIndex();
        level playingField = getLevel(currIndexLevel);
        // Radius der Portale, da diese animiert sein sollen
        GLfloat radius = getRadiusPortal();
        GLint y = 0;
        GLint x = 0;
        
        if (currIndexLevel == 2)
        {
            glTranslatef(g_cellSize / 2.0f, 0.0f, 0.0f);
        }

        for (y = 0; y < playingField.dimensions[1]; y++)
        {
            for (x = 0; x < playingField.dimensions[0]; x++)
            {
                glPushMatrix();
                {
                    // Position der momentan zu zeichnenden Kachel
                    glTranslatef(playingField.currLevel[y][x].center[0], 0.0f, playingField.currLevel[y][x].center[1]);
                    glScalef(g_cellSize, 1.0f, g_cellSize);

                    switch (playingField.currLevel[y][x].fieldType)
                    {
                    case P_HOUSE:
                    case P_OPEN_HOUSE:
                        drawHouse(showNormal);
                        break;
                    case P_FREE:
                    case P_OPEN_DOOR:
                        showNormal ? glCallList(g_freeCellListNormalID) : glCallList(g_freeCellListID);
                        break;
                    case P_WALL:
                        showNormal ? glCallList(g_wallCubeListNormalID) : glCallList(g_wallCubeListID);
                        break;
                    case P_BOX_DOOR_SWITCH:
                        showNormal ? glCallList(g_boxCubeListNormalID) : glCallList(g_boxCubeListID);
                        doorSwitchPos[0] = playingField.currLevel[y][x].center[0];
                        doorSwitchPos[1] = playingField.currLevel[y][x].center[1];
                        break;
                    case P_BOX:
                        showNormal ? glCallList(g_boxCubeListNormalID) : glCallList(g_boxCubeListID);
                        break;
                    case P_TARGET:
                        drawTarget(showNormal);
                        break;
                    case P_MOVEABLE_TRIANGLE:
                    case P_TRIANGLE_OPEN_DOOR:
                        showNormal ? glCallList(g_tetrahedronListNormalID) : glCallList(g_tetrahedronListID);
                        break;
                    case P_TARGET_TRIANGLE:
                        break;
                    case P_DOOR:
                        drawClosedDoor();
                        break;
                    case P_DOOR_SWITCH:
                        showNormal ? glCallList(g_freeCellListNormalID) : glCallList(g_freeCellListID);
                        doorSwitchPos[0] = playingField.currLevel[y][x].center[0];
                        doorSwitchPos[1] = playingField.currLevel[y][x].center[1];
                        break;
                    case P_PORTAL:
                        // Im Hintergrund eine leere zelle zeichnen,
                        // damit nur das Portal animiert ist
                        showNormal ? glCallList(g_freeCellListNormalID) : glCallList(g_freeCellListID);
                        glPushMatrix();
                        {
                            glScalef(radius, 1.0f, radius);
                            glCallList(g_portalListID);
                        }
                        glPopMatrix();
                        break;
                    }
                }
                glPopMatrix();
            }
        }

        drawPushy();
        glPushMatrix();
        {
            glTranslatef(doorSwitchPos[0], 0.0f, doorSwitchPos[1]);
            glScalef(g_cellSize, 1.0f, g_cellSize);
            drawDoorSwitchTransparent();
        }
        glPopMatrix();
    }
    glPopMatrix();
}

/**
 * Zeichen-Funktion fuer eine 3-dimensionale Darstellung.
 * Stellt die Szene dar. Ein rotierender Wuerfel.
 */
void drawScene3D(void)
{
    float ambient[] = {1.0f, 1.0f, 1.0f, 1.0f};
    float diffuse[] = {1.0f, 1.0f, 1.0f, 1.0f};
    float specular[] = {1.0f, 1.0f, 1.0f, 1.0f};

    /* erste (feste) Lichtquelle */
    /* gerichtetes / paralleles Licht von links-unten-vorne */
    CGPoint4f lightPos0 = {0.0f, 15.0f, 0.0f, 0.0f};

    /* zweites (mobiles) Licht*/
    CGPoint4f lightPos1 = {getPushyPosFloats()[1], g_cellSize + g_cellSize / 2, getPushyPosFloats()[0], 1.0f};
    CGVector3f lightDirection1 = {0, -0.6f, 0};
    switch (lastMoveDir)
    {
    case dirUp:
        lightDirection1[2] = -1.0f;
        break;
    case dirDown:
        lightDirection1[2] = 1.0f;
        break;
    case dirLeft:
        lightDirection1[0] = -1.0f;
        break;
    case dirRight:
        lightDirection1[0] = 1.0f;
        break;
    }

    if (showLight)
    {
        brightnessScaling = 0.35f;
        /* Lichtberechnung aktivieren */
        glEnable(GL_LIGHTING);

        if (showFixedLight)
        {
            glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
            glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
            glLightfv(GL_LIGHT0, GL_SPECULAR, specular);
            /* Position der ersten Lichtquelle setzen */
            glLightfv(GL_LIGHT0, GL_POSITION, lightPos0);
            /* Erste Lichtquelle aktivieren */
            glEnable(GL_LIGHT0);
        }
        else
        {
            glDisable(GL_LIGHT0);
        }

        if (showSpotlight)
        {
            glLightfv(GL_LIGHT1, GL_AMBIENT, ambient);
            glLightfv(GL_LIGHT1, GL_DIFFUSE, diffuse);
            glLightfv(GL_LIGHT1, GL_SPECULAR, specular);
            glPushMatrix();
            {
                //expo : Intensitaetsverteilung des Lichts
                //cutoff :  oeffnungswinkel der Lichtquelle
                const float expo = 30.0f;
                const float *ptr2 = &expo;
                glLightfv(GL_LIGHT1, GL_POSITION, lightPos1);
                glLightfv(GL_LIGHT1, GL_SPOT_DIRECTION, lightDirection1);
                glLightfv(GL_LIGHT1, GL_SPOT_EXPONENT, ptr2);
                const float cutoff = 10.0f;
                const float *ptr = &cutoff;
                glLightfv(GL_LIGHT1, GL_SPOT_CUTOFF, ptr);
            }
            glPopMatrix();

            /* Zweite Lichtquelle aktivieren */
            glEnable(GL_LIGHT1);
        }
        else
        {
            glDisable(GL_LIGHT1);
        }
    }
    else
    {
        glDisable(GL_LIGHT0);
        glDisable(GL_LIGHT1);
        glDisable(GL_LIGHTING);
        brightnessScaling = 1.0f;
    }

    if (g_allowMovement)
    {
        drawLevel();
    }
    drawEndOfGame();
}

/**
 * Zeichen-Funktion fuer eine 2-dimensionale Darstellung
 * Stellt die Szene dar. Ein rotierendes Quadrat.
 */
void drawScene2D(void)
{
    CGColor3f pink = {1.0f, 0.3f, 0.7f};
    // Prueft, ob das Spiel vorbei ist, da auch beim nicht bewegen, die Zeit ablaufen kann
    checkEndOfGame();

    if (getShowHelp())
    {
        drawHelp();
    }
    else if (g_allowMovement)
    {
        glPushMatrix();
        {
            glDisable(GL_LIGHTING);
            glRotatef(-270, 1.0f, 0.0f, 0.0f);
            drawLevel();
        }
        glPopMatrix();
        drawString(TEXT_TIME_X, TEXT_Y, pink, "Zeit uebrig: %i", g_allLevels[getLevelIndex()].time - (int)g_timer);
        drawString(TEXT_LEVEL_X, TEXT_Y, pink, "LEVEL: %i", getLevelIndex() + 1);
    }
}

/**
 * Erzeugen der Display-Listen fuer Wuerfel und Koordinatenachsen.
 */
static void
initDisplayLists(void)
{
    g_wallCubeListID = glGenLists((GLsizei)11);
    if (g_wallCubeListID != 0)
    {
        // Wand mit und ohne Normale
        glNewList(g_wallCubeListID, GL_COMPILE);
        drawWallCube(GL_FALSE);
        glEndList();

        g_wallCubeListNormalID = g_wallCubeListID + 1;
        glNewList(g_wallCubeListNormalID, GL_COMPILE);
        drawWallCube(GL_TRUE);
        glEndList();

        // Box mit und ohne Normale
        g_boxCubeListID = g_wallCubeListID + 2;
        glNewList(g_boxCubeListID, GL_COMPILE);
        drawBoxCube(GL_FALSE);
        glEndList();

        g_boxCubeListNormalID = g_wallCubeListID + 3;
        glNewList(g_boxCubeListNormalID, GL_COMPILE);
        drawBoxCube(GL_TRUE);
        glEndList();

        //Freie Zelle mit und ohne Normale
        g_freeCellListID = g_wallCubeListID + 4;
        glNewList(g_freeCellListID, GL_COMPILE);
        drawFreeCell(GL_FALSE);
        glEndList();

        g_freeCellListNormalID = g_wallCubeListID + 5;
        glNewList(g_freeCellListNormalID, GL_COMPILE);
        drawFreeCell(GL_TRUE);
        glEndList();

        // Portal
        g_portalListID = g_wallCubeListID + 6;
        glNewList(g_portalListID, GL_COMPILE);
        drawPortal();
        glEndList();

        // Dunkel blaues Tetraeder mit und ohne Normale
        g_tetrahedronListID = g_wallCubeListID + 7;
        glNewList(g_tetrahedronListID, GL_COMPILE);
        drawTetrahedron(GL_FALSE, GL_FALSE);
        glEndList();

        g_tetrahedronListNormalID = g_wallCubeListID + 8;
        glNewList(g_tetrahedronListNormalID, GL_COMPILE);
        drawTetrahedron(GL_TRUE, GL_FALSE);
        glEndList();

        // Transparentes blaues Tetraeder mit und ohne Normale
        g_tetrahedronBlueTransListID = g_wallCubeListID + 9;
        glNewList(g_tetrahedronBlueTransListID, GL_COMPILE);
        drawTetrahedron(GL_FALSE, GL_TRUE);
        glEndList();

        g_tetrahedronBlueTransListNormalID = g_wallCubeListID + 10;
        glNewList(g_tetrahedronBlueTransListNormalID, GL_COMPILE);
        drawTetrahedron(GL_TRUE, GL_TRUE);
        glEndList();
    }
    else
    {
        exit(1);
    }
}


/**
 * Initialisierung der Szene (inbesondere der OpenGL-Statusmaschine).
 * Setzt Hintergrund- und Zeichenfarbe.
 * @return Rueckgabewert: im Fehlerfall 0, sonst 1.
 */
int initScene(void)
{
    glLineWidth(2.0f);

    initLogic(0);
    /* Setzen der Farbattribute */
    /* Hintergrundfarbe */
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    /* Zeichenfarbe */
    glColor3f(1.0f, 1.0f, 1.0f);

    initDisplayLists();

    /* Z-Buffer-Test aktivieren */
    glEnable(GL_DEPTH_TEST);

    /* Polygonrueckseiten nicht anzeigen */
    glCullFace(GL_BACK);
    glEnable(GL_CULL_FACE);
    glEnable(GL_NORMALIZE);

    /* Startposition der Kamera festlegen */
    cameraRadiussph = CAMERA_DEFAULT_RADIUS;
    cameraThetaSph = CAMERA_DEFAULT_THETA;
    cameraPhiSph = CAMERA_DEFAULT_PHI;

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

/**
 * (De-)aktiviert die Anzeige der Normalen.
 */
void toggleNormal(void)
{
    showNormal = !showNormal;
}

/**
 * (De-)aktiviert die fixe Lichtquelle.
 */
void toggleFixedLight(void)
{
    showFixedLight = !showFixedLight;
}

/**
 * (De-)aktiviert die Spotlight Lampe.
 */
void toggleSpotlight(void)
{
    showSpotlight = !showSpotlight;
}

/**
 * (De-)aktiviert die gesamte Lichtberechnung.
 */
void toggleLight(void)
{
    showLight = !showLight;
}

/**
 * Setzt den Kamera-Radius im KugelKoord.
 * Berechnet die Koord im kart. neu
 * 
 * @param[in] rad, Radius
 */
void setCameraRadius(GLfloat rad)
{
    cameraRadiussph = rad;
}

/**
 * Setzt den Kamera-Theta Winkel im KugelKoord.
 * Berechnet die Koord im kart. neu
 * 
 * @param[in] theta, winkel
 */
void setCameraTheta(GLfloat theta)
{
    cameraThetaSph = theta;
}

/**
 * Setzt den Kamera-Phi Winkel im KugelKoord.
 * Berechnet die Koord im kart. neu
 * 
 * @param[in] phi, winkel
 */
void setCameraPhi(GLfloat phi)
{
    cameraPhiSph = phi;
}

/**
 * Liefert den Kamera-Phi Winkel im KugelKoord.
 * 
 * @return phi
 */
GLfloat getCameraPhi(void)
{
    return cameraPhiSph;
}

/**
 * Liefert den Kamera-Theta Winkel im KugelKoord.
 * 
 * @return theta
 */
GLfloat getCameraTheta(void)
{
    return cameraThetaSph;
}

/**
 * Liefert den Kamera-Radius im KugelKoord.
 * 
 * @return radius
 */
GLfloat getCameraRadius(void)
{
    return cameraRadiussph;
}
