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

/**
 * Alle Daten eines Vertexes.
 */
typedef struct
{
    float x, y, z;    /**< Position */
    float s, t;       /**< Textur-Koordinate */
    float nx, ny, nz; /**< Normale */
} Vertex;

/**
 * Enum zum hin- und herschalten der Effekte
 * 
*/
typedef enum e_effectState
{
    lights_out,
    lights_on,
    cartoon,
    sepia,
    gray
} effectState;

/**
 * Enum zum hin- und herschalten der Debug-Effekte
 * 
*/
typedef enum e_debugState
{
    no_debug,
    normals,
    heightmap
} debugState;

#endif