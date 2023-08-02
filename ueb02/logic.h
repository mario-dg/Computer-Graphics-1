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

// Globaler Countdown
GLfloat g_timer;

// Globaler Spielstatus
gameState g_gameState;

// Globale Zustand, ob Bewegung erlaubt ist
GLboolean g_allowMovement;

// Speichert den Zeitpunkt des letztens getätigten Zuges
GLfloat g_timeLastMove;

GLfloat g_cellSize;

/**
 * Berechnet den Radius des Portals (Zur Animation) 
 * 
 * @param interval Dauer der Skalierung in Sekunden
 */
void calcRadiusPortal(GLfloat interval);

/**
 * Berechnet den Countdown 
 * 
 * @param interval Dauer eines Intervalls
 */
void calcTimer(GLfloat interval);

/**
 * Initialisiert die Logik 
 * 
 * @param index, Index des Levels
 */
void initLogic(GLint index);

/**
 * Prueft, ob das Spiel in irgendeiner Form vorbei ist
 * 
 */ 
void checkEndOfGame(void);

/**
 * Behandelt jede Bewegung der Spielfigur 
 * 
 * @param direction Bewegungsrichtung der Spielfigur
 */
void move(CGDirection direction);

/**
 * Gibt den Index des ausggewählten Levels zurück
*/
GLuint 
getLevelIndex(void);

/**
 * Gibt das Level zum uebergebenen Index zurueck
 * @param indexOfLevel, Index des Levels
 * @return Level
*/
level getLevel(GLint indexOfLevel);

/**
 * Liefert die Aktuelle Position der Spielfigur
 * 
 * @return GLint*, Zeiger auf Position der Spielfigur
 */
GLint* getPushyPos(void);

/**
 * Liefert den aktuellen Radius der Portale 
 * 
 * @return GLfloat, Radius
 */
GLfloat getRadiusPortal(void);

/**
 * Setzt den Wert, ob die Hilfe angefordert wurde oder nicht 
 * 
 * @param bool, angefordert oder nicht
 */
void setShowHelp(GLboolean bool);

/**
 * Liefert den Wert, ob die Hilfe angefordert wurde 
 * 
 * @return GLboolean, angefordert oder nicht
 */
GLboolean getShowHelp(void);

/**
 * Setzt den Wert, ob das Spiel pausiert wurde oder nicht 
 * 
 * @param bool, pausiert oder nicht
 */
void setPaused(GLboolean bool);

/**
 * Liefert den Wert, ob das Spiel pausiert ist oder nicht 
 * 
 * @return GLboolean, pausiert oder nicht
 */
GLboolean getIsPaused(void);
#endif
