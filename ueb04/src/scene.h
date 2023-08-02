#ifndef __SCENE_H__
#define __SCENE_H__
#include "types.h"
/**
 * @file
 * Schnittstelle des Darstellungs-Moduls.
 * Das Modul kapselt die Rendering-Funktionalitaet (insbesondere der OpenGL-
 * Aufrufe) des Programms.
 *
 *
 * @author Mario da Graca, Leonhard Brandes
 */

/**
 * Zeichen-Funktion.
 * Stellt die Szene dar
 */
void drawScene (void);

/**
 * Zeichen-Funktion fuer das Picking, die nur pickbare Objekte zeichnet
 * @param spheres wenn Kugeln gepickt werden koennen true, fuer Boote false
 */ 
void drawScenePickable(GLboolean spheres);

/**
 * Aktualisiert die Normalen fuer alle Kugeln
 */
void updateNormals(void);

/**
 * Aktualisiert das Vertex-Array, wenn die Aufloesung des Mesh veraendert wird
 * @param state ob vergroessert oder verkleinert wird
 */
void updateVertexArray(expandShrinkVertices state);

/**
 * Initialisierung der Szene (inbesondere der OpenGL-Statusmaschine).
 * Setzt Hintergrund- und Zeichenfarbe.
 * @return Rueckgabewert: im Fehlerfall 0, sonst 1.
 */
int initScene (void);

/**
 * (De-)aktiviert den Wireframe-Modus.
 */
void toggleWireframeMode (void);


/**
 * Setzt den Kameraradius auf den uebergebenen Wert
 * @param rad Radius der Kamera
 */
void setCameraRadius(GLfloat rad);

/**
 * Setzt den Polarwinkeln der Kamera im Kugelkoordinatensystem auf den uebergebenen Wert
 * @param theta Wert des Winkels im Bogenmass
 */
void setCameraTheta(GLfloat theta);

/**
 * Setzt den Azimutwinkel der Kamera im Kugelkoordinatensystem auf den uebergebenen Wert
 * @param phi Wert des Winkels im Bogenmass
 */
void setCameraPhi(GLfloat phi);

/**
 * liefert den Azimutwinkel der Kamera im Kugelkoordinatensystem
 * @return Azimutwinkel der Kamera im Kugelkoordinatensystem im Bogenmass
 */ 
GLfloat getCameraPhi(void);

/**
 * liefert den Polarwinkel der Kamera im Kugelkoordinatensystem
 * @return Polarwinkel der Kamera im Kugelkoordinatensystem im Bogenmass
 */ 
GLfloat getCameraTheta(void);

/**
 * liefert den Radius der Kamera im Kugelkoordinatensystem
 * @return Radius der Kamera im Kugelkoordinatensystem
 */ 
GLfloat getCameraRadius(void);

/**
 * Schaltet das ANzeigen der Normalen um
 */ 
void toggleNormal(void);

/**
 * Schaltet das Anzeigen der Kugeln um
 */
void toggleSpheres(void);

/**
 * Liefert die Anzahl der Vertices, die zum Zeichnen der Wasseroberflaeche benutzt werden 
 * 
 */
GLint getAmountVertices(void);

/**
 * Schaltet das Anzeigen der Hilfe um
 */ 
void toggleShowHelp(void);

/**
 * gibt den in der Scene reservierten Speicher wieder frei
 */
void freeAllocatedMem(void);

GLfloat getBoatCHeight(GLint index);

#endif
