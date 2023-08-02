
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
#include <float.h>

/* ---- Eigene Header einbinden ---- */
#include "logic.h"
#include "scene.h"

/** Anzahl der Drehungen des rotierenden Lichtes pro Sekunde */
#define LIGHT1_ROTATIONS_PS 0.25f

#define ATTENUATION (0.999f)
#define WAVE_SPEED (1.0f)
#define WAVE_WIDTH ((2.0f) / (getAmountVertices() - (1)))

#define PICK_HEIGHT (0.5f)

/* ---- Konstanten ---- */

/** Status der der Lichtberechnung (an/aus) */
static GLboolean g_lightingState = GL_TRUE;

/** Status der ersten Lichtquelle (an/aus) */
static GLboolean g_light0State = 1;

/** Status der zweiten Lichtquelle (rotierend/aus/boot1/boot2) */
static light1State g_light1State = 0;

/** Rotationswinkel der zweiten Lichtquelle. */
static float g_light1RotationAngle = 0.0f;

/** Hoehen der Wassersaeulen in einem dynamisch alloziierten Array*/
static GLfloat *heights;

/** Hoehen der Wassersaeulen in einem dynamisch alloziierten Array*/
static GLfloat *velocities;

void freeAllocatedMemLogic(void)
{
    free(heights);
    free(velocities);
}

void initLogic(void)
{
    GLint amountVerticesSide = START_AMOUNT_VERTICES;
    heights = calloc(SQUARE(amountVerticesSide), sizeof(GLfloat));
    if (heights != NULL)
    {
        velocities = calloc(SQUARE(amountVerticesSide), sizeof(GLfloat));
        if (velocities == NULL)
        {
            free(heights);
            exit(1);
        }
    }
    else
    {
        exit(1);
    }
}


void updateLogic(expandShrinkVertices state)
{
    GLint i = 0;
    GLint rowCount = -1;
    GLint oldAmountVerticesSide = getAmountVertices();
    GLint newAmountVerticesSide = oldAmountVerticesSide + state;

    //alte Hoehen speichern um diese beim Vergroeßern oder Verkleinern nicht zu verlieren
    GLfloat *oldHeights = calloc(SQUARE(oldAmountVerticesSide), sizeof(GLfloat));
    if (oldHeights == NULL)
    {
        exit(1);
    }

    //alte Hoehen speichernm um diese beim Vergroeßern oder Verkleinern nicht zu verlieren
    GLfloat *oldVelocities = calloc(SQUARE(oldAmountVerticesSide), sizeof(GLfloat));
    if (oldVelocities == NULL)
    {
        exit(1);
    }

    for (i = 0; i < SQUARE(oldAmountVerticesSide); i++)
    {
        oldHeights[i] = heights[i];
        oldVelocities[i] = velocities[i];
    }

    //Speicher des Hoehenarrays realloziieren
    heights = realloc(heights, SQUARE(newAmountVerticesSide) * sizeof(GLfloat));
    if (heights == NULL)
    {
        exit(1);
    }
    //Speicher des Geschwindigkeitsarrays realloziieren
    velocities = realloc(velocities, SQUARE(newAmountVerticesSide) * sizeof(GLfloat));
    if (velocities == NULL)
    {
        exit(1);
    }

    //Fuellen des neuen Hoehen- und Geschwindigkeitsrrays
    for (i = 0; i < SQUARE(newAmountVerticesSide); i++)
    {
        if ((i % newAmountVerticesSide) == 0)
        {
            rowCount++;
        }

        if (state == expand)
        {
            //neue Punkte haben die Hoehe und Geschwindigkeit 0
            if (((i % newAmountVerticesSide) == (newAmountVerticesSide - 1)) || ((i / newAmountVerticesSide) == (newAmountVerticesSide - 1)))
            {
                heights[i] = 0.0f;
                velocities[i] = 0.0f;
            }
            //alte Punkte behalten ihre Werte
            else
            {
                heights[i] = oldHeights[i - rowCount];
                velocities[i] = oldVelocities[i - rowCount];
            }
        }
        //Beim Verkleinern werden die Randpunkte einfach abgeschnitten
        else
        {
            heights[i] = oldHeights[i + rowCount];
            velocities[i] = oldVelocities[i + rowCount];
        }
    }

    //Freigabe der Zwischenspeicher
    free(oldHeights);
    free(oldVelocities);
}

/**
 * Bildet eine gewichtete Summe der Hoehen von einem Punkt und seinen Nachbarn, 
 * wenn ein Nachbar außerhalb liegen wuerde, wird der Punkt selbst stattdessen genommen
 * @param i Index des Punktes, zu dem die Nachbarn gesucht sind
 * @param amountVerticesSide Anzahl der Punkte entlang einer Seite,
 *        fuer die Bestimmung, ob ein Nachbar außerhalb waere
 * @return gewichtete Summe der Hoehen: Nachbarn mit +1 und der Punkt selbst mit -4
 */
GLfloat getNeighbourHeights(GLint i, GLint amountVerticesSide)
{
    GLfloat toReturn = 0.0f;
    // Rechter Nachbar
    if (((i + 1) % amountVerticesSide) == 0)
    {
        toReturn += heights[i];
    }
    else
    {
        toReturn += heights[i + 1];
    }

    //Linker Nachbar
    if ((i % amountVerticesSide) == 0)
    {
        toReturn += heights[i];
    }
    else
    {
        toReturn += heights[i - 1];
    }

    //Unterer Nachbar
    if ((i + amountVerticesSide) >= SQUARE(amountVerticesSide))
    {
        toReturn += heights[i];
    }
    else
    {
        toReturn += heights[i + amountVerticesSide];
    }

    //Oberer Nachbar
    if (i < amountVerticesSide)
    {
        toReturn += heights[i];
    }
    else
    {
        toReturn += heights[i - amountVerticesSide];
    }

    return (toReturn - (4 * heights[i]));
}

void simulateWater(double idleInterval)
{
    GLint i = 0;
    GLint amountVerticesSide = getAmountVertices();
    GLfloat f = 0.0f;

    //Zwischenspeicher fuer die neu berechneten Hoehen,
    //damit diese nicht die folgenden Berechnungen beeinflussen
    GLfloat *newHeight = calloc(SQUARE(amountVerticesSide), sizeof(GLfloat));
    if (newHeight == NULL)
    {
        exit(1);
    }

    //Berechnung der Wassersimulation
    for (i = 0; i < SQUARE(amountVerticesSide); i++)
    {
        //Kraft haengt von den Hoehen ab
        f = SQUARE(WAVE_SPEED) * (getNeighbourHeights(i, amountVerticesSide)) / SQUARE(WAVE_WIDTH);
        //Geschwindigkeit haengt von der Kraft ab und der Abschwaechung
        velocities[i] = (velocities[i] + f * idleInterval) * ATTENUATION;
        //Aenderung der Hoehen haengt von der Geschwindigkeit und dem Zeitintervall ab
        newHeight[i] = heights[i] + velocities[i] * idleInterval;
    }

    //Uebernehem der neuen Hoehenwerte
    for (i = 0; i < SQUARE(amountVerticesSide); i++)
    {
        heights[i] = newHeight[i];
    }
    free(newHeight);
}

void pickedVertex(GLuint index, mouseButtons click)
{
    heights[index] += click * PICK_HEIGHT;
}

/**
 * Liefert den Status der Lichtberechnung.
 * @return Status der Lichtberechnung (an/aus).
 */
int getLightingState(void)
{
    return g_lightingState;
}

/**
 * Setzt den Status der Lichtberechnung.
 * @param status Status der Lichtberechnung (an/aus).
 */
void setLightingState(int status)
{
    g_lightingState = status;
}

/**
 * Liefert den Status der ersten Lichtquelle.
 * @return Status der ersten Lichtquelle (an/aus).
 */
int getLight0State(void)
{
    return g_light0State;
}

/**
 * Setzt den Status der ersten Lichtquelle.
 * @param status Status der ersten Lichtquelle (an/aus).
 */
void setLight0State(int status)
{
    g_light0State = status;
}

/**
 * Berechnet aktuellen Rotationswinkel der zweiten Lichtquelle.
 * @param interval Dauer der Bewegung in Sekunden.
 */
void calcLight1Rotation(double interval)
{
    if ((g_light1State == rotating) && g_lightingState)
    {
        g_light1RotationAngle += 360.0f * LIGHT1_ROTATIONS_PS * (float)interval;
    }
}

/**
 * Liefert den aktuellen Rotationswinkel der zweiten Lichtquelle.
 * @return aktueller Rotationswinkel der zweiten Lichtquelle.
 */
float getLight1Rotation(void)
{
    return g_light1RotationAngle;
}

/**
 * Liefert den aktuellen Rotationsstatus der zweiten Lichtquelle.
 * @return aktueller Rotationsstatus der zweiten Lichtquelle.
 */
light1State
getLight1State(void)
{
    return g_light1State;
}

/**
 * Setzt den Rotationsstatus der zweiten Lichtquelle.
 * @param rotationStatus Rotationsstatus der zweiten Lichtquelle.
 */
void setLight1State(light1State lightState)
{
    g_light1State = lightState;
}

GLfloat *getHeights(void)
{
    return heights;
}

GLfloat *getVelocities(void)
{
    return velocities;
}
