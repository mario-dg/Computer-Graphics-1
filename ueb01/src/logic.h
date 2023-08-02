#ifndef __LOGIC_H__
#define __LOGIC_H__
/**
 * @file
 * Schnittstelle des Logik-Moduls.
 * Das Modul kapselt die Programmlogik. Wesentliche Programmlogik ist die
 * Verwaltung und Aktualisierung der Position und Farbe eines Rechtecks. Die
 * Programmlogik ist weitgehend unabhaengig von Ein-/Ausgabe (io.h/c) und
 * Darstellung (scene.h/c).
 *
 * Bestandteil eines Beispielprogramms fuer Animationen mit OpenGL & GLUT.
 *
 * @author copyright (C) Fachhochschule Wedel 1999-2011. All rights reserved.
 * @author Leonhard Brandes, Mario da Graca
 */

/* ---- Eigene Header einbinden ---- */
#include "types.h"

/**
 * Fuellt das Array mit allen Boxen, anhand von fixen Positionen
 * und gibt jeder Box eine zufaellige Farbe. Speichert ebenso alle
 * Infos fuer die Extras ab
 * 
 */
void initBoxes(void);

/**
 * Erstellt ein neues Spiel
 * 
 */
void initGame(void);

/**
 * Berechnet neue Position des Rechtecks.
 * @param interval Dauer der Bewegung in Sekunden.
 */
void calcPosition (double interval);

/**
 * Berechnet die neuen Positionen der fallenden Extras
 * 
 * @param[in] Dauer der Bewegung in Sekunden 
 */ 
void calcPositionExtra (double interval);

/**
 * Berechnet neue Position des Schlaegers.
 * @param interval Dauer der Bewegung in Sekunden.
 */
void calcPositionBat (double interval);


/**
 * Liefert aktuelle Postion (des Mittelpunktes) des Rechtecks.
 * @return Postion (des Mittelpunktes) des Rechtecks.
 */
ball *getBall (GLint ballIdx);

/**
 * Liefert die Anzahl der extra Baelle
 * 
 * @return Anzahl der extra Baelle
 */
GLint getAmountUsedBalls(void);

/**
 * Liefert aktuelle Postion (des Mittelpunktes) des Rechtecks.
 * @return Postion (des Mittelpunktes) des Rechtecks.
 */
CGPoint2f *getBatCenter (void);

/**
 * Liefert die momentane Schlaeger-Breite
 * 
 * @return Schlaeger-Breite
 */
GLfloat getBatWidth(void);

/**
 * Liefert die Box an den uebergebenen Koordinaten zurueck
 * 
 * @param[in] row, Reihe der Box
 * @param[in] col, Spalte der Box
 * @return box
 */
box getBox(GLint row, GLint col);



/**
 * Setzt den Bewegunsstatus des Rechtecks.
 * @param direction Bewegungsrichtung deren Status veraendert werden soll.
 * @param status neuer Status der Bewegung: GL_TRUE->Rechteck bewegt sich in Richtung
 * direction, GL_FALSE->Rechteck bewegt sich nicht in Richtung direction.
 */
void
setMovement (CGDirection direction, GLboolean status);
#endif
