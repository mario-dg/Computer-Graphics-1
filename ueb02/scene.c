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
#include <GL/glu.h>
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

/** Zum Konstruieren, eines gleichseitigen Dreiecks */
#define TRIANGLE_VALUE 0.3661f
#define TRIANGLE_SCALING_VALUE 0.67f

/** Konstanten für die Erstellung einer Mauer */
#define AMOUNT_ROWS_WALL 4
#define EVEN_ROWS_WALL 3
#define ODD_ROWS_WALL 4
#define WALL_ROW_OFFSET (3.0f / 8.0f)

/** Konstanten für die Erstellung eines Portals */
#define RADIUS_1 0.9f
#define RADIUS_2 0.7f
#define RADIUS_3 0.5f
#define RADIUS_4 0.3f
#define RADIUS_5 0.15f

/** Konstanten fuer die Textausgabe */
#define TEXT_Y 0.99f
#define TEXT_TIME_X 0.05f
#define TEXT_LEVEL_X 0.75f
#define TEXT_END_X 0.4f
#define TEXT_END_Y 0.5f
#define TEXT_HELP_X 0.25f
#define TEXT_HELP_Y_START 0.25f
#define TEXT_HELP_Y_SPACING 0.05f

/** KOnstanten fuer die Erstellung einer Kiste*/
#define AMOUNT_BOXES_ROW 7

/** Anzahl der Punkte aus dem der Kreis gebildet wird */
#define CIRCLE_VERTICES 16

/** Displaylist IDs fuer alle Objekte, die mehrfach gezeichnet werden */
static GLuint g_wallListID = 0;
static GLuint g_boxListID = 0;
static GLuint g_freeCellListID = 0;
static GLuint g_portalListID = 0;
static GLuint g_moveableTriangleListID = 0;
static GLuint g_gridListID = 0;

/** Alle Farben, die verwendet werden */
static const CGColor3f COLOR_DARK_GREY = {0.25f, 0.25f, 0.25f};
static const CGColor3f COLOR_LIGHT_GREY = {0.78f, 0.78f, 0.78f};
static const CGColor3f COLOR_BROWN = {0.75f, 0.5f, 0.25f};
static const CGColor3f COLOR_DARK_BLUE = {0.1f, 0.0f, 0.6f};
static const CGColor3f COLOR_LIGHT_BLUE = {0.0f, 0.78f, 1.0f};
static const CGColor3f COLOR_RED = {1.0f, 0.0f, 0.0f};
static const CGColor3f COLOR_ORANGE = {1.0f, 0.5f, 0.0f};
static const CGColor3f COLOR_YELLOW = {1.0f, 1.0f, 0.0f};
static const CGColor3f COLOR_GREEN = {0.0f, 1.0f, 0.0f};
static const CGColor3f COLOR_BLUE = {0.0f, 0.0f, 1.0f};
static const CGColor3f COLOR_PINK = {1.0f, 0.2f, 0.8f};
static const CGColor3f COLOR_WHITE = {1.0f, 1.0f, 1.0f};
static const CGColor3f COLOR_BLACK = {0.0f, 0.0f, 0.0f};
static const CGColor3f COLOR_DARK_GREEN = {0.0f, 0.3f, 0.0f};
static const CGColor3f COLOR_SKIN = {1.0f, 0.8f, 0.6f};

/**
 * Zeichnet ein Quadrat mit der uebergebenen Farbe, 
 * mit Seitenlaenge 1 um den Ursprung rum
 * 
 * @param[in] color, Farbe des Quadrats
 */
static void drawRect(const CGColor3f color)
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
 * Zeichnet Linien in Form eines Quadrat mit der uebergebenen Farbe, 
 * mit Seitenlaenge 1 um den Ursprung rum
 * 
 * @param[in] color, Farbe des Quadrats
 */
static void drawLines(const CGColor3f color)
{

    glBegin(GL_LINE_LOOP);
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
 * Zeichnet ein Dreieck mit Katenlaengen 1
 * 
 * @param[in] CGColor3f, Farbe des Dreiecks
 */
static void drawTriangle(const CGColor3f color)
{
    glBegin(GL_TRIANGLES);
    {
        glColor3f(color[0], color[1], color[2]);
        glVertex2f(0.5f, -TRIANGLE_VALUE);
        glVertex2f(0.0f, 0.5f);
        glVertex2f(-0.5f, -TRIANGLE_VALUE);
    }
    glEnd();
}

/**
 * Zeichnet einen Kreis mit Radius 1 um den Ursprung rum
 * 
 * @param[in] CGColor3f, Farbe des Dreiecks
 */
static void drawCircle(const CGColor3f color)
{
    GLdouble theta = 2 * M_PI;
    GLdouble step = 2 * M_PI / CIRCLE_VERTICES;
    glBegin(GL_POLYGON);
    {
        glColor3f(color[0], color[1], color[2]);
        while (theta > 0)
        {
            glVertex2f(cos(theta) * 0.5f, sin(theta) * 0.5f);
            theta -= step;
        }
    }
    glEnd();
}

/**
 * Zeichnet den SpielBlock "Mauer"
 * 
 */
static void drawWall(void)
{
    GLint rowCount = 0;
    GLint i = 0;
    // Anzahl an Ziegeln in geraden Reihen
    GLfloat widthEvenRow = 1.0f / EVEN_ROWS_WALL;
    // Anzahl an Ziegeln in ungeraden Reihen
    GLfloat widthOddRow = 1.0f / ODD_ROWS_WALL;
    // Höhe einer Reihe
    GLfloat heightRow = 1.0f / AMOUNT_ROWS_WALL;

    glPushMatrix();
    {
        // Alle Reihen linksbuendig
        glTranslatef(-widthEvenRow, WALL_ROW_OFFSET, 0.0f);
        for (rowCount = 0; rowCount < AMOUNT_ROWS_WALL; rowCount++)
        {
            glPushMatrix();
            {
                // Zeichnen der geraden Reihen
                glTranslatef(0.0f, -heightRow * rowCount, 0.0f);
                if (rowCount % 2 == 0)
                {

                    for (i = 0; i < EVEN_ROWS_WALL; i++)
                    {
                        glPushMatrix();
                        {
                            glTranslatef(widthEvenRow * i, 0.0f, 0.0f);
                            glScalef(widthEvenRow, heightRow, 1.0f);
                            drawRect(COLOR_DARK_GREY);
                            drawLines(COLOR_LIGHT_GREY);
                        }
                        glPopMatrix();
                    }
                }
                else
                {
                    // Zeichnen der ungeraden Reihen
                    for (i = 0; i < ODD_ROWS_WALL; i++)
                    {
                        glPushMatrix();
                        {
                            glTranslatef(widthOddRow * i - ((widthEvenRow - widthOddRow) / 2), 0.0f, 0.0f);
                            glScalef(widthOddRow, heightRow, 1.0f);
                            drawRect(COLOR_DARK_GREY);
                            drawLines(COLOR_LIGHT_GREY);
                        }
                        glPopMatrix();
                    }
                }
            }
            glPopMatrix();
        }
    }
    glPopMatrix();
}

/**
 * Zeichnet den SpielBlock "Kiste"
 * 
 * 
 */
static void drawBox(void)
{
    // Höhe der Balken Oben und unten & Breite der Balken in der Mitte
    GLfloat shortSide = 1.0f / AMOUNT_BOXES_ROW;
    // Hoehe der Balken in der Mitte
    GLfloat height = 1.0f - 2.0f * shortSide;
    GLint i = 0;

    glPushMatrix();
    {
        // Oberer Balken
        glTranslatef(-(1.0f / 2.0f - (shortSide / 2.0f)), 0.0f, 0.0f);
        glPushMatrix();
        {
            glTranslatef(1.0f / 2.0f - (shortSide / 2.0f), 1.0f / 2.0f - (shortSide / 2.0f), 0.0f);
            glScalef(1.0f, shortSide, 1.0f);
            drawRect(COLOR_BROWN);
            drawLines(COLOR_BLACK);
        }
        glPopMatrix();

        // Balken in der Mitte
        for (i = 0; i < AMOUNT_BOXES_ROW; i++)
        {
            glPushMatrix();
            {
                glTranslatef(shortSide * i, 0.0f, 0.0f);
                glScalef(shortSide, height, 1.0f);
                drawRect(COLOR_BROWN);
                drawLines(COLOR_BLACK);
            }
            glPopMatrix();
        }

        // Untere Balken
        glPushMatrix();
        {
            glTranslatef(1.0f / 2.0f - (shortSide / 2.0f), -(1.0f / 2.0f - (shortSide / 2.0f)), 0.0f);
            glScalef(1.0f, shortSide, 1.0f);
            drawRect(COLOR_BROWN);
            drawLines(COLOR_BLACK);
        }
        glPopMatrix();
    }
    glPopMatrix();
}

/**
 * Zeichnet den Spielblock "Freies Feld"
 * 
 */
static void drawFreeCell(void)
{
    glPushMatrix();
    {
        drawRect(COLOR_LIGHT_GREY);
        drawLines(COLOR_BLACK);
    }
    glPopMatrix();
}

/**
 * Zeichnet den Spielblock "Dreieck" 
 * 
 */
static void drawMoveableTriangle(void)
{
    glCallList(g_freeCellListID);

    glPushMatrix();
    {
        glRotatef(-90, 0.0f, 0.0f, 1.0f);
        glScalef(TRIANGLE_SCALING_VALUE, TRIANGLE_SCALING_VALUE, 1.0f);
        drawTriangle(COLOR_DARK_BLUE);
    }
    glPopMatrix();
}

/**
 * Zeichnet den Spielblock "Zielfeld"
 * 
 */
static void drawTarget(void)
{
    glPushMatrix();
    {
        drawRect(COLOR_DARK_BLUE);
    }
    glPopMatrix();
}

/**
 * Zeichnet den Spielblock "Tür"
 * Besteht aus 2 uebereinanderliegenden Rechtecken
 *  
 */
static void drawDoor(void)
{
    // Tuer Breite
    GLfloat doorWidth = 1.0f / 3.0f;
    // Halbe Tuer Hoehe
    GLfloat halfDoorHeight = 1.0f / 2;

    glCallList(g_freeCellListID);

    glPushMatrix();
    {
        glTranslatef(0.0f, halfDoorHeight / 2, 0.0f);
        glScalef(doorWidth, halfDoorHeight, 1.0f);
        drawRect(COLOR_BROWN);
        drawLines(COLOR_BLACK);
    }
    glPopMatrix();

    glPushMatrix();
    {
        glTranslatef(0.0f, -halfDoorHeight / 2, 0.0f);
        glScalef(doorWidth, halfDoorHeight, 1.0f);
        drawRect(COLOR_BROWN);
        drawLines(COLOR_BLACK);
    }
    glPopMatrix();
}

static void drawOpenDoor(void)
{
    // Tuer Breite
    GLfloat doorWidth = 1.0f / 3.0f;

    glCallList(g_freeCellListID);

    glPushMatrix();
    {
        glTranslatef(0.0f, 0.45f, 0.0f);
        glScalef(doorWidth, 0.1f, 1.0f);
        drawRect(COLOR_BROWN);
        drawLines(COLOR_BLACK);
    }
    glPopMatrix();

    glPushMatrix();
    {
        glTranslatef(0.0f, -0.45f, 0.0f);
        glScalef(doorWidth, 0.1f, 1.0f);
        drawRect(COLOR_BROWN);
        drawLines(COLOR_BLACK);
    }
    glPopMatrix();
}

/**
 * Zeichnet einen Pfeil
 * Bestehend aus einem Rechteck und einem Dreieck
 * 
 */
static void drawArrow(void)
{
    glPushMatrix();
    {
        glScalef(0.5f, 1.0f, 1.0f);
        drawRect(COLOR_LIGHT_BLUE);
    }
    glPopMatrix();

    glPushMatrix();
    {
        glTranslatef(0.0f, 0.5f + TRIANGLE_VALUE, 0.0f);
        drawTriangle(COLOR_LIGHT_BLUE);
    }
    glPopMatrix();
}

/**
 * Zeichnet den Spielblock "Tuerschalter"
 * 2 Diagonal gegenueberliegende Pfeilfe
 * 
 */
static void drawDoorSwitch(void)
{
    glCallList(g_freeCellListID);

    glPushMatrix();
    {
        glTranslatef(1.0f / 6.0f, 1.0f / 6.0f, 0.0f);
        glRotatef(-45, 0.0f, 0.0f, 1.0f);
        glScalef(0.3f, 0.3f, 1.0f);
        drawArrow();
    }
    glPopMatrix();

    glPushMatrix();
    {
        glTranslatef(-1.0f / 6.0f, -1.0f / 6.0f, 0.0f);
        glRotatef(135, 0.0f, 0.0f, 1.0f);
        glScalef(0.3f, 0.3f, 1.0f);
        drawArrow();
    }
    glPopMatrix();
}

/**
 * Zeichnet den Spielblock "Portal" 
 * Besteht aus 5 ineinanderliegenden Kreisen
 * 
 */
static void drawPortal()
{
    glPushMatrix();
    {
        glScalef(RADIUS_1, RADIUS_1, 1.0f);
        drawCircle(COLOR_RED);
    }
    glPopMatrix();

    glPushMatrix();
    {
        glScalef(RADIUS_2, RADIUS_2, 1.0f);
        drawCircle(COLOR_ORANGE);
    }
    glPopMatrix();

    glPushMatrix();
    {
        glScalef(RADIUS_3, RADIUS_3, 1.0f);
        drawCircle(COLOR_YELLOW);
    }
    glPopMatrix();

    glPushMatrix();
    {
        glScalef(RADIUS_4, RADIUS_4, 1.0f);
        drawCircle(COLOR_GREEN);
    }
    glPopMatrix();

    glPushMatrix();
    {
        glScalef(RADIUS_5, RADIUS_5, 1.0f);
        drawCircle(COLOR_BLUE);
    }
    glPopMatrix();
}

/**
 * Zeichnet den SpielBlock "Haus"
 * 
 */
static void drawHouse(const CGColor3f color)
{
    glCallList(g_freeCellListID);

    // Haus-Rumpf
    glPushMatrix();
    {
        glTranslatef(0.0f, -1.0f / 3.5f, 0.0f);
        glScalef(0.6f, 0.4f, 1.0f);
        drawRect(COLOR_WHITE);
    }
    glPopMatrix();

    // Dach
    glPushMatrix();
    {
        glTranslatef(0.0f, 1.0f * 0.4f / 2.65f, 0.0f);
        glScalef(0.65f, 0.65f, 1.0f);
        drawTriangle(color);
    }
    glPopMatrix();

    // Tür
    glPushMatrix();
    {
        glTranslatef(0.0f, -1.0f / 3.0f, 0.0f);
        glScalef(1.0f / 7, 1.0f / 3.25f, 1.0f);
        drawRect(color);
    }
    glPopMatrix();

    //Fenster
    glPushMatrix();
    {
        glTranslatef(1.0f / 5.5f, -1.0f / 4.0f, 0.0f);
        glScalef(1.0f / 7, 1.0f / 7, 1.0f);
        drawRect(COLOR_LIGHT_BLUE);
    }
    glPopMatrix();

    glPushMatrix();
    {
        glTranslatef(-1.0f / 5.5f, -1.0f / 4.0f, 0.0f);
        glScalef(1.0f / 7, 1.0f / 7, 1.0f);
        drawRect(COLOR_LIGHT_BLUE);
    }
    glPopMatrix();
}

/**
 * Zeichnet die Spielfigur 
 * 
 */
static void drawPlayer(void)
{
    //Arme
    glPushMatrix();
    {
        glTranslatef(-1.0f / 6.0f, 1.0f / 9.0f, 0.0f);
        glRotatef(-55, 0.0f, 0.0f, 1.0f);
        glScalef(1.0f / 15.0f, 1.0f / 3.0f, 1.0f);
        drawRect(COLOR_RED);
    }
    glPopMatrix();

    glPushMatrix();
    {
        glTranslatef(1.0f / 6.0f, 1.0f / 9.0f, 0.0f);
        glRotatef(55, 0.0f, 0.0f, 1.0f);
        glScalef(1.0f / 15.0f, 1.0f / 3.0f, 1.0f);
        drawRect(COLOR_RED);
    }
    glPopMatrix();

    //Beine
    glPushMatrix();
    {
        glTranslatef(-1.0f / 15.0f, -1.0f / 3.5f, 0.0f);
        glRotatef(-15, 0.0f, 0.0f, 1.0f);
        glScalef(1.0f / 15.0f, 1.0f / 3.0f, 1.0f);
        drawRect(COLOR_DARK_BLUE);
    }
    glPopMatrix();

    glPushMatrix();
    {
        glTranslatef(1.0f / 15.0f, -1.0f / 3.5f, 0.0f);
        glRotatef(15, 0.0f, 0.0f, 1.0f);
        glScalef(1.0f / 15.0f, 1.0f / 3.0f, 1.0f);
        drawRect(COLOR_DARK_BLUE);
    }
    glPopMatrix();

    //Rumpf
    glPushMatrix();
    {
        glTranslatef(0.0f, 0.0f, 0.0f);
        glScalef(1.0f / 3.5f, 1.0f / 2.5f, 1.0f);
        drawCircle(COLOR_RED);
    }
    glPopMatrix();

    //Kopf
    glPushMatrix();
    {
        glTranslatef(0.0f, 1.0f / 3.0f, 0.0f);
        glScalef(1.0f / 4.0f, 1.0f / 4.0f, 1.0f);
        drawCircle(COLOR_SKIN);
    }
    glPopMatrix();
}

static void drawTriangleOpenDoor(void)
{
    drawOpenDoor();

    glPushMatrix();
    {
        glRotatef(-90, 0.0f, 0.0f, 1.0f);
        glScalef(TRIANGLE_SCALING_VALUE, TRIANGLE_SCALING_VALUE, 1.0f);
        drawTriangle(COLOR_DARK_BLUE);
    }
    glPopMatrix();
}

/**
 * Zeichnet aus allen Spielbloecken das gewaehlte Level 
 * 
 */
static void drawLevel(void)
{
    // Gewaehltes Level
    GLuint currIndexLevel = getLevelIndex();
    level playingField = getLevel(currIndexLevel);
    // Position der Spielfigur
    GLint pushyPos[2];
    pushyPos[0] = getPushyPos()[0];
    pushyPos[1] = getPushyPos()[1];
    // Radius der Portale, da diese animiert sein sollen
    GLfloat radius = getRadiusPortal();
    GLint y = 0;
    GLint x = 0;
    for (y = 0; y < playingField.dimensions[1]; y++)
    {
        for (x = 0; x < playingField.dimensions[0]; x++)
        {
            glPushMatrix();
            {
                // Position der momentan zu zeichnenden Kachel
                glTranslatef(playingField.currLevel[y][x].center[0], playingField.currLevel[y][x].center[1], 0.0f);
                glScalef(g_cellSize, g_cellSize, 1.0f);
                switch (playingField.currLevel[y][x].fieldType)
                {
                case P_HOUSE:
                    drawHouse(COLOR_PINK);
                    break;
                case P_OPEN_HOUSE:
                    drawHouse(COLOR_DARK_GREEN);
                    break;
                case P_FREE:
                    glCallList(g_freeCellListID);
                    break;
                case P_WALL:
                    glCallList(g_wallListID);
                    break;
                case P_BOX_DOOR_SWITCH:
                case P_BOX:
                    glCallList(g_boxListID);
                    break;
                case P_TARGET:
                    drawTarget();
                    break;
                case P_MOVEABLE_TRIANGLE:
                    glCallList(g_moveableTriangleListID);
                    break;
                case P_TARGET_TRIANGLE:
                    break;
                case P_TRIANGLE_OPEN_DOOR:
                    drawTriangleOpenDoor();
                    break;
                case P_DOOR:
                    drawDoor();
                    break;
                case P_OPEN_DOOR:
                    drawOpenDoor();
                    break;
                case P_DOOR_SWITCH:
                    drawDoorSwitch();
                    break;
                case P_PORTAL:
                    // Im Hintergrund eine leere zelle zeichnen,
                    // damit nur das Portal animiert ist
                    glCallList(g_freeCellListID);
                    glPushMatrix();
                    {
                        glScalef(radius, radius, 1.0f);
                        glCallList(g_portalListID);
                    }
                    glPopMatrix();
                    break;
                }
            }
            glPopMatrix();

            // Umrandung jeder Kachel
            glPushMatrix();
            {
                glTranslatef(playingField.currLevel[y][x].center[0], playingField.currLevel[y][x].center[1], 0.0f);
                glScalef(g_cellSize, g_cellSize, 1.0f);
                glCallList(g_gridListID);
            }
            glPopMatrix();
        }
    }
    // Zeichnen der Spielfigur
    glPushMatrix();
    {
        glTranslatef(playingField.currLevel[pushyPos[1]][pushyPos[0]].center[0], playingField.currLevel[pushyPos[1]][pushyPos[0]].center[1], 0.0f);
        glScalef(g_cellSize, g_cellSize, 1.0f);
        drawPlayer();
    }
    glPopMatrix();
}

/**
 * Zeichen-Funktion.
 * Stellt die Szene dar.
 */
void drawScene(void)
{
    // Prueft, ob das Spiel vorbei ist, da auch beim nicht bewegen, die Zeit ablaufen kann
    checkEndOfGame();
    CGColor3f white = {1.0f, 1.0f, 1.0f};
    CGColor3f pink = {1.0f, 0.3f, 0.7f};
    // Sollte die Hilfe angefordert oder das Spiel vorbei sein,
    // wird das Spielfeld ausgeblendet
    if (g_allowMovement)
    {
        glLoadIdentity();

        drawLevel();

        drawString(TEXT_TIME_X, TEXT_Y, white, "Zeit uebrig: %i", g_allLevels[getLevelIndex()].time - (int)g_timer);
        drawString(TEXT_LEVEL_X, TEXT_Y, white, "LEVEL: %i", getLevelIndex() + 1);
    }

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

    // Hilfeausgabe
    if (getShowHelp())
    {
        drawString(TEXT_HELP_X, TEXT_HELP_Y_START, pink, "q, Q,ESC: Spiel beenden");
        drawString(TEXT_HELP_X, TEXT_HELP_Y_START + 1 * TEXT_HELP_Y_SPACING, pink, "r,R: Neustart des aktuellen Levels");
        drawString(TEXT_HELP_X, TEXT_HELP_Y_START + 2 * TEXT_HELP_Y_SPACING, pink, "h, H: Zeigt die Hilfe an");
        drawString(TEXT_HELP_X, TEXT_HELP_Y_START + 3 * TEXT_HELP_Y_SPACING, pink, "F1: Wireframe an/aus");
        drawString(TEXT_HELP_X, TEXT_HELP_Y_START + 4 * TEXT_HELP_Y_SPACING, pink, "F2: Schaltet zwischen Vollbild und Fenster-Ansicht um");
        drawString(TEXT_HELP_X, TEXT_HELP_Y_START + 5 * TEXT_HELP_Y_SPACING, pink, "Pfeiltasten: Steuern des Spielers\n");
        drawString(TEXT_HELP_X, TEXT_HELP_Y_START + 6 * TEXT_HELP_Y_SPACING, pink, "p, P: Pause");
        drawString(TEXT_HELP_X, TEXT_HELP_Y_START + 7 * TEXT_HELP_Y_SPACING, pink, "1,2,3 Direkter Zugriff auf die 3 Level");
    }
}

/**
 * Erzeugen der Display-Listen fuer Wuerfel und Koordinatenachsen.
 */
static void
initDisplayLists(void)
{
    /* Sechs Display-Listen anfordern */
    g_wallListID = glGenLists((GLsizei)6);
    if (g_wallListID != 0)
    {
        /* Erste Diplay-Liste */
        glNewList(g_wallListID, GL_COMPILE);
        drawWall();
        glEndList();

        /* Zweite Diplay-Liste */
        g_boxListID = g_wallListID + 1;
        glNewList(g_boxListID, GL_COMPILE);
        drawBox();
        glEndList();

        /* Dritte Diplay-Liste */
        g_freeCellListID = g_wallListID + 2;
        glNewList(g_freeCellListID, GL_COMPILE);
        drawFreeCell();
        glEndList();

        /* Vierte Diplay-Liste */
        g_portalListID = g_wallListID + 3;
        glNewList(g_portalListID, GL_COMPILE);
        drawPortal();
        glEndList();

        /* Fünfte Diplay-Liste */
        g_moveableTriangleListID = g_wallListID + 4;
        glNewList(g_moveableTriangleListID, GL_COMPILE);
        drawMoveableTriangle();
        glEndList();

        /* Sechste Diplay-Liste */
        g_gridListID = g_wallListID + 5;
        glNewList(g_gridListID, GL_COMPILE);
        drawLines(COLOR_BLACK);
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
    initLogic(0);
    /* Setzen der Farbattribute */
    /* Hintergrundfarbe */
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    /* Zeichenfarbe */
    glColor3f(1.0f, 1.0f, 1.0f);

    /* Linienbreite */
    glLineWidth(3.0f);

    /* Displaylisten initialisieren */
    initDisplayLists();
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
