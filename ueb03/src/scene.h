#ifndef __SCENE_H__
#define __SCENE_H__
/**
 * @file
 * Schnittstelle des Darstellungs-Moduls.
 * Das Modul kapselt die Rendering-Funktionalitaet (insbesondere der OpenGL-
 * Aufrufe) des Programms.
 *
 * Bestandteil eines Beispielprogramms fuer Animationen mit OpenGL & GLUT.
 *
 * @author copyright (C) Fachhochschule Wedel 1999-2011. All rights reserved.
 * @author Leonhard Brandes, Mario da Graca
 */


/**
 * Zeichen-Funktionen in 3D
 */
void drawScene3D (void);

/**
 * Zeichen-Funktionen in 2D
 */
void drawScene2D (void);

/**
 * Initialisierung der Szene (inbesondere der OpenGL-Statusmaschine).
 * Setzt Hintergrund- und Zeichenfarbe.
 * @return Rueckgabewert: im Fehlerfall 0, sonst 1.
 */
int initScene (void);

void toggleWireframeMode (void);

void toggleNormal(void);

void toggleFixedLight(void);

void toggleSpotlight(void);

void toggleLight(void);

void setCameraRadius(GLfloat rad);

void setCameraTheta(GLfloat theta);

void setCameraPhi(GLfloat phi);

GLfloat getCameraPhi(void);

GLfloat getCameraTheta(void);

GLfloat getCameraRadius(void);

#endif
