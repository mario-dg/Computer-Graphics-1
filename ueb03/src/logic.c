/**
 * @file
 * Logik-Modul.
 * Das Modul kapselt die Programmlogik. Wesentliche Programmlogik ist die
 * Verwaltung und Aktualisierung der Position und Bewegung eines Rechtecks. Die
 * Programmlogik ist weitgehend unabhaengig von Ein-/Ausgabe (io.h/c) und
 * Darstellung (scene.h/c).
 *
 * Bestandteil eines Beispielprogramms fuer Animationen mit OpenGL & GLUT.
 *
 * @author copyright (C) Fachhochschule Wedel 1999-2011. All rights reserved.
 * @author Leonhard Brandes, Mario da Graca
 */

/* ---- Standard Header einbinden ---- */
#include <time.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <float.h>

/* ---- Eigene Header einbinden ---- */
#include "logic.h"
#include "types.h"

/* ---- Konstanten ---- */
#define START_X_LEVEL_1 7
#define START_Y_LEVEL_1 7
#define START_X_LEVEL_2 9
#define START_Y_LEVEL_2 8
#define START_X_LEVEL_3 1
#define START_Y_LEVEL_3 1

#define TIME_LEVEL_1 50
#define TIME_LEVEL_2 100
#define TIME_LEVEL_3 150

/* Skalierungsfaktor der Portale */
#define SHRINK_FACTOR 1.0f

/* Delay nach jedem Zug */
#define MOVE_DELAY_NON_ANIMATION 0.33f
#define ANIMATION_TIME 1.0f
#define MOVE_DELAY_ANIMATION ANIMATION_TIME

/* Makro zur Bestimmung des groesseren Wertes */
#define MAX(x, y) (((x) > (y)) ? (x) : (y))

/* ---- Globale Daten ---- */

/* Aktueller Radius der Portale */
static GLfloat g_radiusPortal = 1.0f;

/* Aktueller Skalierungszustand */
static GLboolean g_radiusShrink = GL_FALSE;

/* Gibt an, ob die Hilfe angefordert wurde */
static GLboolean g_showHelp = GL_FALSE;

/** Gibt an, ob das Spiel pausiert wurde */
static GLboolean g_isPaused;

/** Gibt an, ob Pushy sich noch in einer Animation befindet */
static GLboolean isInAnimation = GL_FALSE;

// Aufbau der spielbaren Level
static pushyLevel1Square level1 =
    {
        {P_WALL, P_WALL, P_WALL, P_WALL, P_WALL, P_WALL, P_WALL, P_WALL, P_WALL, P_WALL, P_WALL},
        {P_WALL, P_HOUSE, P_FREE, P_FREE, P_FREE, P_TARGET, P_FREE, P_MOVEABLE_TRIANGLE, P_PORTAL, P_WALL, P_WALL},
        {P_WALL, P_FREE, P_FREE, P_WALL, P_FREE, P_WALL, P_FREE, P_FREE, P_WALL, P_FREE, P_WALL},
        {P_WALL, P_FREE, P_WALL, P_FREE, P_FREE, P_BOX, P_FREE, P_WALL, P_FREE, P_FREE, P_WALL},
        {P_WALL, P_FREE, P_FREE, P_FREE, P_MOVEABLE_TRIANGLE, P_WALL, P_WALL, P_FREE, P_FREE, P_FREE, P_WALL},
        {P_WALL, P_FREE, P_WALL, P_FREE, P_FREE, P_DOOR, P_FREE, P_FREE, P_WALL, P_FREE, P_WALL},
        {P_WALL, P_FREE, P_PORTAL, P_FREE, P_WALL, P_WALL, P_FREE, P_FREE, P_FREE, P_FREE, P_WALL},
        {P_WALL, P_FREE, P_FREE, P_FREE, P_DOOR_SWITCH, P_FREE, P_BOX, P_FREE, P_WALL, P_BOX, P_WALL},
        {P_WALL, P_FREE, P_WALL, P_FREE, P_FREE, P_WALL, P_FREE, P_WALL, P_FREE, P_FREE, P_WALL},
        {P_WALL, P_WALL, P_FREE, P_FREE, P_FREE, P_FREE, P_BOX, P_FREE, P_FREE, P_FREE, P_WALL},
        {P_WALL, P_WALL, P_WALL, P_WALL, P_WALL, P_WALL, P_WALL, P_WALL, P_WALL, P_WALL, P_WALL}};

static pushyLevel3Square level2 =
    {
        {P_WALL, P_WALL, P_WALL, P_WALL, P_WALL, P_WALL, P_WALL, P_WALL, P_WALL, P_WALL, P_WALL, P_WALL, P_WALL},
        {P_WALL, P_FREE, P_FREE, P_WALL, P_FREE, P_FREE, P_WALL, P_WALL, P_WALL, P_WALL, P_WALL, P_FREE, P_WALL},
        {P_WALL, P_FREE, P_HOUSE, P_WALL, P_BOX, P_FREE, P_WALL, P_BOX, P_WALL, P_FREE, P_FREE, P_FREE, P_WALL},
        {P_WALL, P_FREE, P_FREE, P_WALL, P_FREE, P_FREE, P_WALL, P_FREE, P_FREE, P_MOVEABLE_TRIANGLE, P_WALL, P_FREE, P_WALL},
        {P_WALL, P_WALL, P_DOOR, P_FREE, P_FREE, P_FREE, P_FREE, P_FREE, P_FREE, P_FREE, P_PORTAL, P_FREE, P_WALL},
        {P_WALL, P_FREE, P_FREE, P_WALL, P_FREE, P_FREE, P_FREE, P_FREE, P_FREE, P_BOX, P_WALL, P_FREE, P_WALL},
        {P_WALL, P_FREE, P_FREE, P_WALL, P_FREE, P_FREE, P_WALL, P_FREE, P_FREE, P_FREE, P_WALL, P_FREE, P_WALL},
        {P_WALL, P_FREE, P_FREE, P_BOX, P_FREE, P_FREE, P_WALL, P_FREE, P_FREE, P_FREE, P_WALL, P_FREE, P_WALL},
        {P_WALL, P_WALL, P_FREE, P_FREE, P_FREE, P_FREE, P_WALL, P_FREE, P_FREE, P_FREE, P_BOX, P_FREE, P_WALL},
        {P_WALL, P_WALL, P_MOVEABLE_TRIANGLE, P_FREE, P_WALL, P_WALL, P_WALL, P_FREE, P_FREE, P_FREE, P_MOVEABLE_TRIANGLE, P_FREE, P_WALL},
        {P_WALL, P_WALL, P_FREE, P_FREE, P_FREE, P_BOX, P_FREE, P_DOOR_SWITCH, P_WALL, P_FREE, P_FREE, P_FREE, P_WALL},
        {P_WALL, P_FREE, P_FREE, P_FREE, P_PORTAL, P_BOX, P_FREE, P_WALL, P_WALL, P_TARGET, P_FREE, P_FREE, P_WALL},
        {P_WALL, P_WALL, P_WALL, P_WALL, P_WALL, P_WALL, P_WALL, P_WALL, P_WALL, P_WALL, P_WALL, P_WALL, P_WALL}};

static pushyLevelRect level3 =
    {
        {P_WALL, P_WALL, P_WALL, P_WALL, P_WALL, P_WALL, P_WALL, P_WALL, P_WALL, P_WALL},
        {P_WALL, P_FREE, P_FREE, P_FREE, P_FREE, P_FREE, P_FREE, P_FREE, P_FREE, P_WALL},
        {P_WALL, P_FREE, P_BOX, P_FREE, P_FREE, P_FREE, P_MOVEABLE_TRIANGLE, P_FREE, P_FREE, P_WALL},
        {P_WALL, P_WALL, P_BOX, P_FREE, P_FREE, P_FREE, P_WALL, P_WALL, P_WALL, P_WALL},
        {P_WALL, P_FREE, P_FREE, P_MOVEABLE_TRIANGLE, P_FREE, P_BOX, P_FREE, P_MOVEABLE_TRIANGLE, P_FREE, P_WALL},
        {P_WALL, P_FREE, P_FREE, P_FREE, P_FREE, P_WALL, P_FREE, P_PORTAL, P_FREE, P_WALL},
        {P_WALL, P_FREE, P_BOX, P_FREE, P_FREE, P_FREE, P_TARGET, P_WALL, P_WALL, P_WALL},
        {P_WALL, P_BOX, P_BOX, P_BOX, P_FREE, P_FREE, P_WALL, P_WALL, P_WALL, P_WALL},
        {P_WALL, P_FREE, P_PORTAL, P_FREE, P_WALL, P_FREE, P_FREE, P_WALL, P_WALL, P_WALL},
        {P_WALL, P_BOX, P_BOX, P_BOX, P_WALL, P_DOOR_SWITCH, P_FREE, P_DOOR, P_HOUSE, P_WALL},
        {P_WALL, P_WALL, P_WALL, P_WALL, P_WALL, P_WALL, P_WALL, P_WALL, P_WALL, P_WALL}};

// Aktuelles gespieltes Level
static GLuint indexLevel = 0;

// Position der Spielfigur
static GLint g_PushyPosCoords[2] = {9, 9};
static GLfloat g_PushyPos[2] = {0.0f, 0.0f};

// Animation von Pushy (de-)aktiviert
GLboolean showAnimation = GL_FALSE;

// Hat Pushy sich nach dem letzten versuchten bewegen wiklich bewegt
GLboolean didMove = GL_FALSE;

/* ---- Funktionen ---- */

void calcRadiusPortal(GLfloat interval)
{
    if (g_radiusShrink)
    {
        g_radiusPortal -= interval * SHRINK_FACTOR;
    }
    else
    {
        g_radiusPortal += interval * SHRINK_FACTOR;
    }
    if (g_radiusPortal > 1)
    {
        g_radiusShrink = GL_TRUE;
    }
    else if (g_radiusPortal < 0)
    {
        g_radiusShrink = GL_FALSE;
    }
}

void calcTimer(GLfloat interval)
{
    g_timer += interval;
}

void initLogic(GLint index)
{
    GLint i = 0;
    GLint m = 0;
    GLint k = 0;
    // Default-Werte setzen
    g_timer = 0.0f;
    g_gameState = running;
    g_allowMovement = GL_TRUE;
    g_isPaused = GL_FALSE;
    // Direkt ziehen, kein warten am Start des Spiels
    g_timeLastMove = -MOVE_DELAY_NON_ANIMATION;
    indexLevel = index;
    lastMoveDir = dirUp;

    g_allLevels[0].pushyPosStart[0] = START_X_LEVEL_1;
    g_allLevels[0].pushyPosStart[1] = START_Y_LEVEL_1;
    g_allLevels[1].pushyPosStart[0] = START_X_LEVEL_2;
    g_allLevels[1].pushyPosStart[1] = START_Y_LEVEL_2;
    g_allLevels[2].pushyPosStart[0] = START_X_LEVEL_3;
    g_allLevels[2].pushyPosStart[1] = START_Y_LEVEL_3;

    g_allLevels[0].time = TIME_LEVEL_1;
    g_allLevels[1].time = TIME_LEVEL_2;
    g_allLevels[2].time = TIME_LEVEL_3;

    g_allLevels[0].dimensions[0] = LEVELSIZE_1;
    g_allLevels[0].dimensions[1] = LEVELSIZE_1;
    g_allLevels[1].dimensions[0] = LEVELSIZE_3;
    g_allLevels[1].dimensions[1] = LEVELSIZE_3;
    g_allLevels[2].dimensions[0] = LEVELSIZE_2_X;
    g_allLevels[2].dimensions[1] = LEVELSIZE_2_Y;

    g_PushyPosCoords[0] = g_allLevels[indexLevel].pushyPosStart[0];
    g_PushyPosCoords[1] = g_allLevels[indexLevel].pushyPosStart[1];

    g_cellSize = 10.0f / (MAX(g_allLevels[indexLevel].dimensions[0], g_allLevels[indexLevel].dimensions[1]));

    for (m = 0; m < AMOUNT_LEVELS; m++)
    {
        GLfloat zOff = (g_cellSize / 2.0f);
        GLfloat xOff = (g_cellSize / 2.0f);
        CGPoint2f startPos = {-5.0f + xOff,
                              -5.0f + zOff};

        g_allLevels[m].amountMoveableTriangle = 0;

        // Speicher für alle Level reservieren
        g_allLevels[m].currLevel = malloc(sizeof(cell *) * g_allLevels[m].dimensions[1]);
        for (i = 0; i < g_allLevels[m].dimensions[1]; i++)
        {
            g_allLevels[m].currLevel[i] = malloc(sizeof(cell) * g_allLevels[m].dimensions[0]);
        }

        for (i = 0; i < g_allLevels[m].dimensions[1]; i++)
        {
            for (k = 0; k < g_allLevels[m].dimensions[0]; k++)
            {
                switch (m)
                {
                case 0:
                    g_allLevels[m].currLevel[i][k].fieldType = level1[i][k];
                    break;
                case 1:
                    g_allLevels[m].currLevel[i][k].fieldType = level2[i][k];
                    break;
                case 2:
                    g_allLevels[m].currLevel[i][k].fieldType = level3[i][k];
                    break;
                }
                if (g_allLevels[m].currLevel[i][k].fieldType == P_MOVEABLE_TRIANGLE)
                {
                    g_allLevels[m].amountMoveableTriangle++;
                }
                g_allLevels[m].currLevel[i][k].center[0] = startPos[0];
                g_allLevels[m].currLevel[i][k].center[1] = startPos[1];
                startPos[0] += g_cellSize;
            }
            startPos[0] = -5.0f + xOff;
            startPos[1] += g_cellSize;
        }
    }
    g_PushyPos[0] = g_allLevels[indexLevel].currLevel[g_PushyPosCoords[1]][g_PushyPosCoords[0]].center[0];
    g_PushyPos[1] = g_allLevels[indexLevel].currLevel[g_PushyPosCoords[1]][g_PushyPosCoords[0]].center[1];
}

/**
 * Liefert den Zelltyp zurueck, die sich im Weg der Spielfigur befindet
 * 
 * @param dir, Bewegungsrichtung der Spielfigur
 * @return Zelltyp 
 */
static pushyFieldType checkCollision(CGDirection dir)
{
    pushyFieldType collidedCell;
    // Pushy kann nicht ganz am Rand stehen -> Prüfung auf nachbarfeld problemlos machbar
    switch (dir)
    {
    case dirLeft:
        collidedCell = g_allLevels[indexLevel].currLevel[g_PushyPosCoords[1]][g_PushyPosCoords[0] - 1].fieldType;
        break;
    case dirRight:
        collidedCell = g_allLevels[indexLevel].currLevel[g_PushyPosCoords[1]][g_PushyPosCoords[0] + 1].fieldType;
        break;
    case dirUp:
        collidedCell = g_allLevels[indexLevel].currLevel[g_PushyPosCoords[1] - 1][g_PushyPosCoords[0]].fieldType;
        break;
    case dirDown:
        collidedCell = g_allLevels[indexLevel].currLevel[g_PushyPosCoords[1] + 1][g_PushyPosCoords[0]].fieldType;
        break;
    }
    return collidedCell;
}


void calcPushyPos(CGDirection dir, GLfloat interval)
{
    if (showAnimation)
    {
        isInAnimation = g_timer - g_timeLastMove <= ANIMATION_TIME;
        if (isInAnimation && didMove)
        {
            switch (dir)
            {
            case dirUp:
                g_PushyPos[0] -= (interval * g_cellSize) * (1.0f / ANIMATION_TIME);
                break;
            case dirDown:
                g_PushyPos[0] += (interval * g_cellSize) * (1.0f / ANIMATION_TIME);
                break;
            case dirLeft:
                g_PushyPos[1] -= (interval * g_cellSize) * (1.0f / ANIMATION_TIME);
                break;
            case dirRight:
                g_PushyPos[1] += (interval * g_cellSize) * (1.0f / ANIMATION_TIME);
                break;
            }
        }
        else
        {
            g_PushyPos[1] = g_allLevels[indexLevel].currLevel[g_PushyPosCoords[0]][g_PushyPosCoords[1]].center[1];
            g_PushyPos[0] = g_allLevels[indexLevel].currLevel[g_PushyPosCoords[0]][g_PushyPosCoords[1]].center[0];
            didMove = GL_FALSE;
        }
    }
    else
    {
        g_PushyPos[1] = g_allLevels[indexLevel].currLevel[g_PushyPosCoords[0]][g_PushyPosCoords[1]].center[1];
        g_PushyPos[0] = g_allLevels[indexLevel].currLevel[g_PushyPosCoords[0]][g_PushyPosCoords[1]].center[0];
    }
}

/**
 * Bewegt die Spielfigur in die uebergebene Richtung 
 * 
 * @param dir, Bewegungsrichtung
 */
static void movePushy(CGDirection dir)
{
    switch (dir)
    {
    case dirLeft:
        g_PushyPosCoords[0]--;
        break;
    case dirRight:
        g_PushyPosCoords[0]++;
        break;
    case dirUp:
        g_PushyPosCoords[1]--;
        break;
    case dirDown:
        g_PushyPosCoords[1]++;
        break;
    }

    didMove = GL_TRUE;
}

/**
 * Bewegung der Spielfigur auf ein Freies Feld 
 */
static void handleCollisionFreeCell(CGDirection dir)
{
    movePushy(dir);
}

CGCoord2i *getFieldPos(pushyFieldType type)
{
    GLint i = 0;
    GLint k = 0;
    static CGCoord2i pos;
    pos[0] = -1;
    pos[1] = -1;
    GLboolean found = GL_FALSE;
    for (i = 0; i < g_allLevels[indexLevel].dimensions[1] && !found; i++)
    {
        for (k = 0; k < g_allLevels[indexLevel].dimensions[0] && !found; k++)
        {
            if (g_allLevels[indexLevel].currLevel[i][k].fieldType == type)
            {
                pos[0] = k;
                pos[1] = i;
                found = GL_TRUE;
            }
        }
    }
    return &pos;
}

/**
 * Liefert die Koordinate des nicht betrenen Portals 
 * 
 * @return int*, Koordinate des Portals
 */
static int *getOtherPortalPos(void)
{
    GLint i = 0;
    GLint k = 0;
    static GLint pos[2];
    GLboolean found = GL_FALSE;
    for (i = 0; i < g_allLevels[indexLevel].dimensions[1] && !found; i++)
    {
        for (k = 0; k < g_allLevels[indexLevel].dimensions[0] && !found; k++)
        {
            if (g_allLevels[indexLevel].currLevel[i][k].fieldType == P_PORTAL && g_PushyPosCoords[0] != k && g_PushyPosCoords[1] != i)
            {
                pos[0] = k;
                pos[1] = i;
                found = GL_TRUE;
            }
        }
    }
    return pos;
}

/**
 * Schliessen der Tuer, wenn die Kiste wieder vom Schalter runterbewegt wird
 *  
 * @param dir, Bewegungsrichtung
 */
static void handleCollisionBoxDoorSwitch(CGDirection dir)
{
    CGCoord2i posDoor;
    posDoor[0] = (*getFieldPos(P_OPEN_DOOR))[0];
    posDoor[1] = (*getFieldPos(P_OPEN_DOOR))[1];

    // Wenn auf der offenen Tuer ein Dreieck steht und die Tuer geschlossen wird,
    // wird das Dreieck zerstoert und das Level muss neu gestartet werden
    if (posDoor[0] == -1)
    {
        posDoor[0] = (*getFieldPos(P_TRIANGLE_OPEN_DOOR))[0];
        posDoor[1] = (*getFieldPos(P_TRIANGLE_OPEN_DOOR))[1];
    }
    switch (dir)
    {
    case dirLeft:
        // Feld hinter der Kiste Frei
        if (g_allLevels[indexLevel].currLevel[g_PushyPosCoords[1]][g_PushyPosCoords[0] - 2].fieldType == P_FREE)
        {
            // Verschieben der Box und der Spielfigur, neu anzeigen des Schalters, Schliessen der Tür
            g_allLevels[indexLevel].currLevel[g_PushyPosCoords[1]][g_PushyPosCoords[0] - 2].fieldType = P_BOX;
            g_allLevels[indexLevel].currLevel[g_PushyPosCoords[1]][g_PushyPosCoords[0] - 1].fieldType = P_DOOR_SWITCH;
            g_allLevels[indexLevel].currLevel[posDoor[1]][posDoor[0]].fieldType = P_DOOR;
            movePushy(dir);
        }
        break;
    case dirRight:
        if (g_allLevels[indexLevel].currLevel[g_PushyPosCoords[1]][g_PushyPosCoords[0] + 2].fieldType == P_FREE)
        {
            g_allLevels[indexLevel].currLevel[g_PushyPosCoords[1]][g_PushyPosCoords[0] + 2].fieldType = P_BOX;
            g_allLevels[indexLevel].currLevel[g_PushyPosCoords[1]][g_PushyPosCoords[0] + 1].fieldType = P_DOOR_SWITCH;
            g_allLevels[indexLevel].currLevel[posDoor[1]][posDoor[0]].fieldType = P_DOOR;
            movePushy(dir);
        }
        break;
    case dirUp:
        if (g_allLevels[indexLevel].currLevel[g_PushyPosCoords[1] - 2][g_PushyPosCoords[0]].fieldType == P_FREE)
        {
            g_allLevels[indexLevel].currLevel[g_PushyPosCoords[1] - 2][g_PushyPosCoords[0]].fieldType = P_BOX;
            g_allLevels[indexLevel].currLevel[g_PushyPosCoords[1] - 1][g_PushyPosCoords[0]].fieldType = P_DOOR_SWITCH;
            g_allLevels[indexLevel].currLevel[posDoor[1]][posDoor[0]].fieldType = P_DOOR;
            movePushy(dir);
        }
        break;
    case dirDown:
        if (g_allLevels[indexLevel].currLevel[g_PushyPosCoords[1] + 2][g_PushyPosCoords[0]].fieldType == P_FREE)
        {
            g_allLevels[indexLevel].currLevel[g_PushyPosCoords[1] + 2][g_PushyPosCoords[0]].fieldType = P_BOX;
            g_allLevels[indexLevel].currLevel[g_PushyPosCoords[1] + 1][g_PushyPosCoords[0]].fieldType = P_DOOR_SWITCH;
            g_allLevels[indexLevel].currLevel[posDoor[1]][posDoor[0]].fieldType = P_DOOR;
            movePushy(dir);
        }
        break;
    }
}

/**
 * Verschieben einer Kiste in die Bewegungsrichtung 
 * 
 * @param dir, Bewegungsrichtung
 */
static void handleCollisionBox(CGDirection dir)
{
    // Position der Tuer
    CGCoord2i posDoor;
    posDoor[0] = (*getFieldPos(P_DOOR))[0];
    posDoor[1] = (*getFieldPos(P_DOOR))[1];
    //printf("Door: %d;%d\n", posDoor[0], posDoor[1]);

    switch (dir)
    {
    case dirLeft:
        // Feld hinter der Kiste frei
        if (g_allLevels[indexLevel].currLevel[g_PushyPosCoords[1]][g_PushyPosCoords[0] - 2].fieldType == P_FREE)
        {
            // Verschieben der Box und der Spielfigur
            g_allLevels[indexLevel].currLevel[g_PushyPosCoords[1]][g_PushyPosCoords[0] - 2].fieldType = P_BOX;
            g_allLevels[indexLevel].currLevel[g_PushyPosCoords[1]][g_PushyPosCoords[0] - 1].fieldType = P_FREE;
            movePushy(dir);
        }
        else if (g_allLevels[indexLevel].currLevel[g_PushyPosCoords[1]][g_PushyPosCoords[0] - 2].fieldType == P_DOOR_SWITCH)
        {
            // Oeffnen der Tuer, beim Verschieben der Kiste auf einen Schalter
            g_allLevels[indexLevel].currLevel[g_PushyPosCoords[1]][g_PushyPosCoords[0] - 2].fieldType = P_BOX_DOOR_SWITCH;
            g_allLevels[indexLevel].currLevel[g_PushyPosCoords[1]][g_PushyPosCoords[0] - 1].fieldType = P_FREE;
            g_allLevels[indexLevel].currLevel[posDoor[1]][posDoor[0]].fieldType = P_OPEN_DOOR;
            movePushy(dir);
        }
        break;
    case dirRight:
        if (g_allLevels[indexLevel].currLevel[g_PushyPosCoords[1]][g_PushyPosCoords[0] + 2].fieldType == P_FREE)
        {
            g_allLevels[indexLevel].currLevel[g_PushyPosCoords[1]][g_PushyPosCoords[0] + 2].fieldType = P_BOX;
            g_allLevels[indexLevel].currLevel[g_PushyPosCoords[1]][g_PushyPosCoords[0] + 1].fieldType = P_FREE;
            movePushy(dir);
        }
        else if (g_allLevels[indexLevel].currLevel[g_PushyPosCoords[1]][g_PushyPosCoords[0] + 2].fieldType == P_DOOR_SWITCH)
        {
            g_allLevels[indexLevel].currLevel[g_PushyPosCoords[1]][g_PushyPosCoords[0] + 2].fieldType = P_BOX_DOOR_SWITCH;
            g_allLevels[indexLevel].currLevel[g_PushyPosCoords[1]][g_PushyPosCoords[0] + 1].fieldType = P_FREE;
            g_allLevels[indexLevel].currLevel[posDoor[1]][posDoor[0]].fieldType = P_OPEN_DOOR;
            movePushy(dir);
        }
        break;
    case dirUp:
        if (g_allLevels[indexLevel].currLevel[g_PushyPosCoords[1] - 2][g_PushyPosCoords[0]].fieldType == P_FREE)
        {
            g_allLevels[indexLevel].currLevel[g_PushyPosCoords[1] - 2][g_PushyPosCoords[0]].fieldType = P_BOX;
            g_allLevels[indexLevel].currLevel[g_PushyPosCoords[1] - 1][g_PushyPosCoords[0]].fieldType = P_FREE;
            movePushy(dir);
        }
        else if (g_allLevels[indexLevel].currLevel[g_PushyPosCoords[1] - 2][g_PushyPosCoords[0]].fieldType == P_DOOR_SWITCH)
        {
            g_allLevels[indexLevel].currLevel[g_PushyPosCoords[1] - 2][g_PushyPosCoords[0]].fieldType = P_BOX_DOOR_SWITCH;
            g_allLevels[indexLevel].currLevel[g_PushyPosCoords[1] - 1][g_PushyPosCoords[0]].fieldType = P_FREE;
            g_allLevels[indexLevel].currLevel[posDoor[1]][posDoor[0]].fieldType = P_OPEN_DOOR;
            movePushy(dir);
        }
        break;
    case dirDown:
        if (g_allLevels[indexLevel].currLevel[g_PushyPosCoords[1] + 2][g_PushyPosCoords[0]].fieldType == P_FREE)
        {
            g_allLevels[indexLevel].currLevel[g_PushyPosCoords[1] + 2][g_PushyPosCoords[0]].fieldType = P_BOX;
            g_allLevels[indexLevel].currLevel[g_PushyPosCoords[1] + 1][g_PushyPosCoords[0]].fieldType = P_FREE;
            movePushy(dir);
        }
        else if (g_allLevels[indexLevel].currLevel[g_PushyPosCoords[1] + 2][g_PushyPosCoords[0]].fieldType == P_DOOR_SWITCH)
        {
            g_allLevels[indexLevel].currLevel[g_PushyPosCoords[1] + 2][g_PushyPosCoords[0]].fieldType = P_BOX_DOOR_SWITCH;
            g_allLevels[indexLevel].currLevel[g_PushyPosCoords[1] + 1][g_PushyPosCoords[0]].fieldType = P_FREE;
            g_allLevels[indexLevel].currLevel[posDoor[1]][posDoor[0]].fieldType = P_OPEN_DOOR;
            movePushy(dir);
        }
        break;
    }
}

/**
 * Verschieben der Dreiecke in Bewegungsrichtung
 * 
 * @param dir, Bewegungsrichtung
 */
static void handleCollisionMoveableTriangle(CGDirection dir)
{
    // Koordinate des Hauses
    CGCoord2i posHouse;
    posHouse[0] = (*getFieldPos(P_HOUSE))[0];
    posHouse[1] = (*getFieldPos(P_HOUSE))[1];
    switch (dir)
    {
    case dirLeft:
        // Feld hinter dem Dreieck frei
        if (g_allLevels[indexLevel].currLevel[g_PushyPosCoords[1]][g_PushyPosCoords[0] - 2].fieldType == P_FREE)
        {
            // Verschieben des Dreiecks und der Spielfigur
            g_allLevels[indexLevel].currLevel[g_PushyPosCoords[1]][g_PushyPosCoords[0] - 2].fieldType = P_MOVEABLE_TRIANGLE;
            g_allLevels[indexLevel].currLevel[g_PushyPosCoords[1]][g_PushyPosCoords[0] - 1].fieldType = P_FREE;
            movePushy(dir);
        }
        else if (g_allLevels[indexLevel].currLevel[g_PushyPosCoords[1]][g_PushyPosCoords[0] - 2].fieldType == P_OPEN_DOOR)
        {
            g_allLevels[indexLevel].currLevel[g_PushyPosCoords[1]][g_PushyPosCoords[0] - 2].fieldType = P_TRIANGLE_OPEN_DOOR;
            g_allLevels[indexLevel].currLevel[g_PushyPosCoords[1]][g_PushyPosCoords[0] - 1].fieldType = P_FREE;
            movePushy(dir);
        }
        else if (g_allLevels[indexLevel].currLevel[g_PushyPosCoords[1]][g_PushyPosCoords[0] - 2].fieldType == P_TARGET)
        {
            // Beim Verschieben der Dreiecke ins Zielfeld, verschwindet dies
            g_allLevels[indexLevel].currLevel[g_PushyPosCoords[1]][g_PushyPosCoords[0] - 1].fieldType = P_FREE;
            g_allLevels[indexLevel].amountMoveableTriangle--;
            movePushy(dir);
        }
        break;
    case dirRight:
        if (g_allLevels[indexLevel].currLevel[g_PushyPosCoords[1]][g_PushyPosCoords[0] + 2].fieldType == P_FREE)
        {
            g_allLevels[indexLevel].currLevel[g_PushyPosCoords[1]][g_PushyPosCoords[0] + 2].fieldType = P_MOVEABLE_TRIANGLE;
            g_allLevels[indexLevel].currLevel[g_PushyPosCoords[1]][g_PushyPosCoords[0] + 1].fieldType = P_FREE;
            movePushy(dir);
        }
        else if (g_allLevels[indexLevel].currLevel[g_PushyPosCoords[1]][g_PushyPosCoords[0] + 2].fieldType == P_OPEN_DOOR)
        {
            g_allLevels[indexLevel].currLevel[g_PushyPosCoords[1]][g_PushyPosCoords[0] + 2].fieldType = P_TRIANGLE_OPEN_DOOR;
            g_allLevels[indexLevel].currLevel[g_PushyPosCoords[1]][g_PushyPosCoords[0] + 1].fieldType = P_FREE;
            movePushy(dir);
        }
        else if (g_allLevels[indexLevel].currLevel[g_PushyPosCoords[1]][g_PushyPosCoords[0] + 2].fieldType == P_TARGET)
        {
            g_allLevels[indexLevel].currLevel[g_PushyPosCoords[1]][g_PushyPosCoords[0] + 1].fieldType = P_FREE;
            g_allLevels[indexLevel].amountMoveableTriangle--;
            movePushy(dir);
        }
        break;
    case dirUp:
        if (g_allLevels[indexLevel].currLevel[g_PushyPosCoords[1] - 2][g_PushyPosCoords[0]].fieldType == P_FREE)
        {
            g_allLevels[indexLevel].currLevel[g_PushyPosCoords[1] - 2][g_PushyPosCoords[0]].fieldType = P_MOVEABLE_TRIANGLE;
            g_allLevels[indexLevel].currLevel[g_PushyPosCoords[1] - 1][g_PushyPosCoords[0]].fieldType = P_FREE;
            movePushy(dir);
        }
        else if (g_allLevels[indexLevel].currLevel[g_PushyPosCoords[1] - 2][g_PushyPosCoords[0]].fieldType == P_OPEN_DOOR)
        {
            g_allLevels[indexLevel].currLevel[g_PushyPosCoords[1] - 2][g_PushyPosCoords[0]].fieldType = P_TRIANGLE_OPEN_DOOR;
            g_allLevels[indexLevel].currLevel[g_PushyPosCoords[1] - 1][g_PushyPosCoords[0]].fieldType = P_FREE;
            movePushy(dir);
        }
        else if (g_allLevels[indexLevel].currLevel[g_PushyPosCoords[1] - 2][g_PushyPosCoords[0]].fieldType == P_TARGET)
        {
            g_allLevels[indexLevel].currLevel[g_PushyPosCoords[1] - 1][g_PushyPosCoords[0]].fieldType = P_FREE;
            g_allLevels[indexLevel].amountMoveableTriangle--;
            movePushy(dir);
        }
        break;
    case dirDown:
        if (g_allLevels[indexLevel].currLevel[g_PushyPosCoords[1] + 2][g_PushyPosCoords[0]].fieldType == P_FREE)
        {
            g_allLevels[indexLevel].currLevel[g_PushyPosCoords[1] + 2][g_PushyPosCoords[0]].fieldType = P_MOVEABLE_TRIANGLE;
            g_allLevels[indexLevel].currLevel[g_PushyPosCoords[1] + 1][g_PushyPosCoords[0]].fieldType = P_FREE;
            movePushy(dir);
        }
        else if (g_allLevels[indexLevel].currLevel[g_PushyPosCoords[1] + 2][g_PushyPosCoords[0]].fieldType == P_OPEN_DOOR)
        {
            g_allLevels[indexLevel].currLevel[g_PushyPosCoords[1] + 2][g_PushyPosCoords[0]].fieldType = P_TRIANGLE_OPEN_DOOR;
            g_allLevels[indexLevel].currLevel[g_PushyPosCoords[1] + 1][g_PushyPosCoords[0]].fieldType = P_FREE;
            movePushy(dir);
        }
        else if (g_allLevels[indexLevel].currLevel[g_PushyPosCoords[1] + 2][g_PushyPosCoords[0]].fieldType == P_TARGET)
        {
            g_allLevels[indexLevel].currLevel[g_PushyPosCoords[1] + 1][g_PushyPosCoords[0]].fieldType = P_FREE;
            g_allLevels[indexLevel].amountMoveableTriangle--;
            movePushy(dir);
        }
        break;
    }
    // Alle Dreiecke im Zielfeld -> Oeffnen des Hauses
    if (g_allLevels[indexLevel].amountMoveableTriangle == 0)
    {
        g_allLevels[indexLevel].currLevel[posHouse[1]][posHouse[0]].fieldType = P_OPEN_HOUSE;
    }
}

static void handleCollisionTriangleOpenDoor(CGDirection dir)
{
    switch (dir)
    {
    case dirLeft:
        // Feld hinter dem Dreieck frei
        if (g_allLevels[indexLevel].currLevel[g_PushyPosCoords[1]][g_PushyPosCoords[0] - 2].fieldType == P_FREE)
        {
            // Verschieben des Dreiecks und der Spielfigur
            g_allLevels[indexLevel].currLevel[g_PushyPosCoords[1]][g_PushyPosCoords[0] - 2].fieldType = P_MOVEABLE_TRIANGLE;
            g_allLevels[indexLevel].currLevel[g_PushyPosCoords[1]][g_PushyPosCoords[0] - 1].fieldType = P_OPEN_DOOR;
            movePushy(dir);
        }
        else if (g_allLevels[indexLevel].currLevel[g_PushyPosCoords[1]][g_PushyPosCoords[0] - 2].fieldType == P_TARGET)
        {
            // Beim Verschieben der Dreiecke ins Zielfeld, verschwindet dies
            g_allLevels[indexLevel].currLevel[g_PushyPosCoords[1]][g_PushyPosCoords[0] - 1].fieldType = P_OPEN_DOOR;
            g_allLevels[indexLevel].amountMoveableTriangle--;
            movePushy(dir);
        }
        break;
    case dirRight:
        if (g_allLevels[indexLevel].currLevel[g_PushyPosCoords[1]][g_PushyPosCoords[0] + 2].fieldType == P_FREE)
        {
            g_allLevels[indexLevel].currLevel[g_PushyPosCoords[1]][g_PushyPosCoords[0] + 2].fieldType = P_MOVEABLE_TRIANGLE;
            g_allLevels[indexLevel].currLevel[g_PushyPosCoords[1]][g_PushyPosCoords[0] + 1].fieldType = P_OPEN_DOOR;
            movePushy(dir);
        }
        else if (g_allLevels[indexLevel].currLevel[g_PushyPosCoords[1]][g_PushyPosCoords[0] + 2].fieldType == P_TARGET)
        {
            g_allLevels[indexLevel].currLevel[g_PushyPosCoords[1]][g_PushyPosCoords[0] + 1].fieldType = P_OPEN_DOOR;
            g_allLevels[indexLevel].amountMoveableTriangle--;
            movePushy(dir);
        }
        break;
    case dirUp:
        if (g_allLevels[indexLevel].currLevel[g_PushyPosCoords[1] - 2][g_PushyPosCoords[0]].fieldType == P_FREE)
        {
            g_allLevels[indexLevel].currLevel[g_PushyPosCoords[1] - 2][g_PushyPosCoords[0]].fieldType = P_MOVEABLE_TRIANGLE;
            g_allLevels[indexLevel].currLevel[g_PushyPosCoords[1] - 1][g_PushyPosCoords[0]].fieldType = P_OPEN_DOOR;
            movePushy(dir);
        }
        else if (g_allLevels[indexLevel].currLevel[g_PushyPosCoords[1] - 2][g_PushyPosCoords[0]].fieldType == P_TARGET)
        {
            g_allLevels[indexLevel].currLevel[g_PushyPosCoords[1] - 1][g_PushyPosCoords[0]].fieldType = P_OPEN_DOOR;
            g_allLevels[indexLevel].amountMoveableTriangle--;
            movePushy(dir);
        }
        break;
    case dirDown:
        if (g_allLevels[indexLevel].currLevel[g_PushyPosCoords[1] + 2][g_PushyPosCoords[0]].fieldType == P_FREE)
        {
            g_allLevels[indexLevel].currLevel[g_PushyPosCoords[1] + 2][g_PushyPosCoords[0]].fieldType = P_MOVEABLE_TRIANGLE;
            g_allLevels[indexLevel].currLevel[g_PushyPosCoords[1] + 1][g_PushyPosCoords[0]].fieldType = P_OPEN_DOOR;
            movePushy(dir);
        }
        else if (g_allLevels[indexLevel].currLevel[g_PushyPosCoords[1] + 2][g_PushyPosCoords[0]].fieldType == P_TARGET)
        {
            g_allLevels[indexLevel].currLevel[g_PushyPosCoords[1] + 1][g_PushyPosCoords[0]].fieldType = P_OPEN_DOOR;
            g_allLevels[indexLevel].amountMoveableTriangle--;
            movePushy(dir);
        }
        break;
    }
}

void checkEndOfGame(void)
{
    if (g_allLevels[indexLevel].currLevel[g_PushyPosCoords[1]][g_PushyPosCoords[0]].fieldType == P_OPEN_HOUSE)
    {
        g_gameState = won;
    }
    else if (g_timer >= g_allLevels[indexLevel].time)
    {
        g_gameState = timesUp;
    }

    if (g_gameState == timesUp || g_gameState == won)
    {
        g_allowMovement = GL_FALSE;
    }
}

/**
 * Bewegen der Spielfigur aufs geoeffnete Haus 
 * 
 * @param dir, Bewegungsrichtung
 */
static void handleCollisionOpenHouse(CGDirection dir)
{
    movePushy(dir);
}

/**
 * Bewegen der Spielfigur auf ein Portal
 * 
 * @param dir, Bewegungsrichtung
 */
static void handleCollisionPortal(CGDirection dir)
{
    movePushy(dir);
    GLint *portalPos = getOtherPortalPos();
    g_PushyPosCoords[0] = portalPos[0];
    g_PushyPosCoords[1] = portalPos[1];
}

void move(CGDirection direction)
{
    if ((showAnimation && ((g_timer - g_timeLastMove) >= MOVE_DELAY_ANIMATION)) || (!showAnimation && ((g_timer - g_timeLastMove) >= MOVE_DELAY_NON_ANIMATION)))
    {
        if (g_allowMovement && !g_isPaused)
        {
            pushyFieldType collidedCell = checkCollision(direction);

            switch (collidedCell)
            {
            case P_WALL:
            case P_DOOR:
                break;
            case P_FREE:
            case P_OPEN_DOOR:
            case P_DOOR_SWITCH:
            case P_HOUSE:
                handleCollisionFreeCell(direction);
                lastMoveDir = direction;
                break;
            case P_BOX:
                handleCollisionBox(direction);
                lastMoveDir = direction;
                break;
            case P_BOX_DOOR_SWITCH:
                handleCollisionBoxDoorSwitch(direction);
                lastMoveDir = direction;
                break;
            case P_MOVEABLE_TRIANGLE:
                handleCollisionMoveableTriangle(direction);
                lastMoveDir = direction;
                break;
            case P_OPEN_HOUSE:
                handleCollisionOpenHouse(direction);
                lastMoveDir = direction;
                break;
            case P_PORTAL:
                handleCollisionPortal(direction);
                lastMoveDir = direction;
                break;
            case P_TRIANGLE_OPEN_DOOR:
                handleCollisionTriangleOpenDoor(direction);
                lastMoveDir = direction;
                break;
            default:
                break;
            }
            checkEndOfGame();
        }
        g_timeLastMove = g_timer;
    }
}

void toggleShowHelp(void)
{
    g_showHelp = !g_showHelp;
}

void toggleAnimation(void)
{
    showAnimation = !showAnimation;
}

GLuint
getLevelIndex(void)
{
    return indexLevel;
}

level getLevel(GLint indexOfLevel)
{
    return g_allLevels[indexOfLevel];
}

GLint *getPushyPosCoords(void)
{
    return g_PushyPosCoords;
}

GLfloat *getPushyPosFloats(void)
{
    return g_PushyPos;
}

GLfloat getRadiusPortal(void)
{
    return g_radiusPortal;
}

GLboolean getShowHelp(void)
{
    return g_showHelp;
}

void setPaused(GLboolean bool)
{
    g_isPaused = bool;
}

GLboolean getIsPaused(void)
{
    return g_isPaused;
}

GLboolean getShowAnimation(void)
{
    return showAnimation;
}

GLboolean getShowAnimation(void);