#ifndef __TYPES_H__
#define __TYPES_H__
/**
 * @file
 * Typenschnittstelle.
 * Das Modul kapselt die Typdefinitionen und globalen Konstanten des Programms.
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

/* ---- Konstanten ---- */
// Default Fenster Groesse
#define DEFAULT_WINDOW_WIDTH 1500
#define DEFAULT_WINDOW_HEIGHT 800

/* Hoehe/Breite der Level */
#define LEVELSIZE_1 (11)
#define LEVELSIZE_3 (13)
#define LEVELSIZE_2_X (10)
#define LEVELSIZE_2_Y (11)

// Anzahl der Level
#define AMOUNT_LEVELS (3)


/* ---- Typedeklarationen ---- */

/** RGB-Farbwert */
typedef GLfloat CGColor3f[3];

typedef GLfloat CGColor4f[4];

/** Punkt im 2D-Raum */
typedef GLfloat CGPoint2f[2];

typedef GLfloat CGPoint4f[4];

typedef GLfloat CGVector3f[3];

/** Koordinate im 2D-Raum */
typedef GLint CGCoord2i[2];

/*
 * Seiten des Tetraeders
 */
typedef enum
{
    bottom,
    back,
    frontLeft,
    frontRight
} tetrahedronSites;

/*
 * Art von Levelfeldern
 */
typedef enum
{
    P_HOUSE,
    P_OPEN_HOUSE,
    P_FREE,
    P_WALL,
    P_BOX,
    P_TARGET,
    P_MOVEABLE_TRIANGLE,
    P_TARGET_TRIANGLE,
    P_DOOR,
    P_OPEN_DOOR,
    P_DOOR_SWITCH,
    P_BOX_DOOR_SWITCH,
    P_TRIANGLE_OPEN_DOOR,
    P_PORTAL
} pushyFieldType;

/* Spielfelder */
typedef pushyFieldType pushyLevel1Square[LEVELSIZE_1][LEVELSIZE_1];
typedef pushyFieldType pushyLevelRect[LEVELSIZE_2_Y][LEVELSIZE_2_X];
typedef pushyFieldType pushyLevel3Square[LEVELSIZE_3][LEVELSIZE_3];

/**
 * Datentyp fuer die Darstellung einer Zelle
*/
typedef struct cell
{
    pushyFieldType fieldType;
    CGPoint2f center;
} cell;

// Datentyp fuer die Darstellung eines Levels
typedef cell **pushyLevel;

typedef struct level
{
    pushyLevel currLevel;
    GLint dimensions[2];
    GLint pushyPosStart[2];
    GLint amountMoveableTriangle;
    GLint time;
} level;

// Speichern aller Level
level g_allLevels[AMOUNT_LEVELS];

/** Bewegungsrichtungen. */
enum e_Direction
{
    dirUp,
    dirLeft,
    dirDown,
    dirRight
};
/** Datentyp fuer Bewegungsrichtungen. */
typedef enum e_Direction CGDirection;

// Datentyp fuer den Spielstatus
enum e_GameState {
    running,
    timesUp,
    won
};
typedef enum e_GameState gameState;

#endif
