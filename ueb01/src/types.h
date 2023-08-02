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
/** x-Koordinate linker Rand */
#define BORDER_LEFT -0.95f
/** x-Koordinate rechter Rand */
#define BORDER_RIGHT 0.95f
/** y-Koordinate oberer Rand */
#define BORDER_TOP 0.95f
/** y-Koordinate unterer Rand */
#define BORDER_BOTTOM -0.95f

/** Breite der Rahmen */
#define FRAME_WIDTH 0.05f

/** Radius des Spielballs */
#define CIRCLE_RADIUS 0.04f

/** Breite des Schlaegers */
#define QUAD_WIDTH_BAT 0.3f
/** Hoehe des Schlaegers */
#define QUAD_HEIGHT_BAT 0.05f

/** Breite eines Blocks */
#define QUAD_WIDTH_BLOCK 0.17f
/** Hoehe eines Blocks */
#define QUAD_HEIGHT_BLOCK 0.115f

/** Seitenlaengen eines Extras */
#define TRI_SIDE_LENGTH 0.075f

/** Anzahl der Spalten **/
#define COLS 9
/** Anzahl der Reihen **/
#define ROWS 7

/** Abstaender der Boxen horizontal und vertikal**/
#define spaceX 0.025f
#define spaceY 0.025f

/* ---- Typedeklarationen ---- */
/** Punkt im 2D-Raum */
typedef GLfloat CGPoint2f[2];

/** Vektor im 2D-Raum */
typedef GLfloat CGVector2f[2];

/** RGB-Farbwert */
typedef GLfloat CGColor3f[3];

/** Koordinate */
typedef GLint CGPosition[2];

/** Kollisionsseiten. */
enum e_Side
{
    sideNone,
    sideLeft,
    sideRight,
    sideTop,
    sideBot
};

/** Datentyp fuer Kollisionsseiten. */
typedef enum e_Side CGSide;

/** Bewegungsrichtungen. */
enum e_Direction
{
    dirLeft,
    dirRight
};

/** Datentyp fuer Bewegungsrichtungen. */
typedef enum e_Direction CGDirection;

/** Extras */
enum e_extrasType
{
    e_longerBat,
    e_slowerBall,
    e_bonusPoints,
    e_extraBalls,
    e_normal
};

/** Datentyp fuer Extras */
typedef enum e_extrasType CGExtra;

typedef struct extra
{
    CGExtra type;
    CGPoint2f center;
    CGVector2f direction;
    CGColor3f color;
    GLboolean visible;
    GLfloat angle;
} extra;

/** Datentyp fuer die Darstellung einer Box */
typedef struct box
{
    CGPoint2f center;
    CGColor3f color;
    GLbyte visible;
    extra extraType;
} box;

/** Datentyp fuer die Darstellung eines Balles */
typedef struct ball
{
    CGPoint2f center;
    CGVector2f direction;
    CGColor3f color;
    GLboolean visible;
} ball;

/** Datentyp fuer die Darstellung aller Baelle im Spiel */
typedef struct ballArray
{
    ball *array;
    // Anzahl aller jemals belegten Felder
    GLint used;
    // Aktuelle maximale Groeße des Arrays
    GLint size;
    // Anzahl momentan aktiver Spielbaelle
    GLint activeBalls;
} ballArray;

/** Alle Baelle - Global definiert, um beim schließen des Programms 
 * den Speicher auch wieder freigeben zu können
*/
ballArray g_allBalls;

#endif
