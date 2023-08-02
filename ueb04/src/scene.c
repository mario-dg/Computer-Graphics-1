/**
 * @file
 * Darstellungs-Modul.
 * Das Modul kapselt die Rendering-Funktionalitaet (insbesondere der OpenGL-
 * Aufrufe) des Programms.
 *
 * Bestandteil eines Beispielprogramms fuer Textausgabe mit OpenGL & GLUT.
 *
 * @author copyright (C) Fachhochschule Wedel 1999-2011. All rights reserved.
 */

/* ---- System Header einbinden ---- */
#ifdef WIN32
#include <windows.h>
#endif

#ifdef __APPLE__
#include <OpenGL/gl.h>
#include <GLUT/glut.h>
#else
#include <GL/gl.h>
#include <GL/glut.h>
#endif

/* ---- Eigene Header einbinden ---- */
#include "scene.h"
#include "debugGL.h"
#include "stringOutput.h"
#include "logic.h"
#include "texture.h"
#include "sceneObjects.h"
#include <math.h>
#include <float.h>

/**Kameraposition im KugelKoordinatensystem */
static GLfloat cameraRadiussph = 0.0f;
static GLfloat cameraThetaSph = 0.0f;
static GLfloat cameraPhiSph = 0.0f;

static GLint g_amountVerticesSide;
static Vertex *g_vertices;

static GLuint *g_indices;

#define M_PI 3.141592654

#define M_PI_2 (M_PI / 2.0f)

#define M_PI_4 (M_PI_2 / 2.0f)

/** Startposition der Kamera */
#define CAMERA_DEFAULT_RADIUS 3.0f
#define CAMERA_DEFAULT_THETA M_PI_4
#define CAMERA_DEFAULT_PHI M_PI_2

#define LOWER_BORDER (0.3f)
#define UPPER_BORDER (0.65f)

#ifndef DEG2RAD
/** Winkelumrechnung von Grad nach Radiant */
#define DEG2RAD(x) ((x) / 180.0f * M_PI)
/** Winkelumrechnung von Radiant nach Grad */
#define RAD2DEG(x) ((x) / M_PI * 180.0f)
#endif

#define LIGHTHOUSE_HEIGHT (0.75f)
#define LIGHTHOUSE_RADIUS (0.15f)

CGVector3f g_boats[2] = {{BOAT_1_X, 0.0f, BOAT_1_Z}, {BOAT_2_X, 0.0f, BOAT_2_Z}};

/* Umschalten einiger Funktionen */
GLboolean showHelp = GL_FALSE;
GLboolean showSpheres = GL_TRUE;

/* Konstanten fuer Farben */
static const CGColor3f LOW_DARK_BLUE = {0.07f, 0.07f, 0.44f};
static const CGColor3f MEDIUM_GREEN = {0.0f, 1.0f, 0.0f};
static const CGColor3f HIGH_RED = {1.0f, 0.0f, 0.0f};
static const CGColor3f COLOR_RED = {1.0f, 0.0f, 0.0f};
static const CGColor3f COLOR_WHITE = {1.0f, 1.0f, 1.0f};

/**
* Gibt den Hilfetext aus.
*/
static void drawHelp()
{
  int i = 0;
  int size = 17;

  GLfloat color[3] = {1.0f, 0.2f, 0.8f};

  char *help[] = {"Hilfe",
                  "F1 - Wireframe an/aus",
                  "F2 - Normalen an/aus",
                  "F3 - Beleuchtungsberechnung an/aus",
                  "F4 - Punktlichtquelle an/aus",
                  "F5 - Spotlight an/aus",
                  "F6 - Picken Kugeln/Boote umschalten",
                  "F12 - Vollbildmodus an/aus",
                  "u,U/o,O - rein-/rauszoomen der Kamera",
                  "i,I/j,J/k,K/l,L - Bewegen der Kamera ",
                  "h/H - Hilfe an/aus",
                  "+/- - Anzahl der Punkt im Mesh vergößern/verringern",
                  "t/T - Texturierung an/aus",
                  "s/S - Anzeige der Kugeln an/aus",
                  "p/P - Simulation pausieren",
                  "ESC/q/Q - Ende",
                  "linke/rechte Maustaste - Picken von Kugeln und Booten"};

  drawString(0.4f, 0.1f, color, help[0]);

  for (i = 1; i < size; ++i)
  {
    drawString(0.2f, 0.15f + i * 0.05f, color, help[i]);
  }
}

/** Fuellt das Index-Array, welches die Zeichnreihenfolge der Vertices bestimmt
 * @param amountIndices Anzahl der Indizees, die zum zeichnen des Mesh noetig sind
 */
void fillIndexArray(GLint amountIndices)
{
  GLint i = 0;
  GLint countUpperTri = 0;
  GLint countLowerTri = 0;
  GLint rowCount = -1;
  GLboolean upperTri = GL_FALSE;
  for (i = 0; i < amountIndices; i += 3)
  {
    //Jede Reihe besteht aus einer Reihe normaer Dreiecke und einer Reihe die auf dem Kopf steht
    if (upperTri && (countUpperTri % (g_amountVerticesSide - 1) == 0))
    {
      upperTri = GL_FALSE;
    }
    else if (!upperTri && (countLowerTri % (g_amountVerticesSide - 1) == 0))
    {
      upperTri = GL_TRUE;
      rowCount++;
    }

    if (upperTri)
    {
      g_indices[i] = countUpperTri + rowCount;
      g_indices[i + 1] = countUpperTri + g_amountVerticesSide + rowCount;
      g_indices[i + 2] = countUpperTri + 1 + rowCount;
      countUpperTri++;
    }
    else
    {
      g_indices[i] = countLowerTri + 1 + rowCount;
      g_indices[i + 1] = countLowerTri + g_amountVerticesSide + rowCount;
      g_indices[i + 2] = countLowerTri + g_amountVerticesSide + 1 + rowCount;
      countLowerTri++;
    }
  }
}

GLint getClosestVertex(GLint index)
{
  GLint i = 0;
  GLint amountVerticesSide = getAmountVertices();
  CGVector2f vector = {0.0f, 0.0f};
  GLfloat shortestDistance = FLT_MAX;
  GLint closestVertex = -1;
  CGVector2f boatCoords;
  boatCoords[0] = g_boats[index][0];
  boatCoords[1] = g_boats[index][2];

  for (i = 0; i < SQUARE(amountVerticesSide); i++)
  {
    vector[0] = fabs(g_vertices[i][CX] - boatCoords[0]);
    vector[1] = fabs(g_vertices[i][CZ] - boatCoords[1]);
    GLfloat temp = sqrt(SQUARE(vector[0]) + SQUARE(vector[1]));
    if (temp < shortestDistance)
    {
      shortestDistance = temp;
      closestVertex = i;
    }
  }
  return closestVertex;
}

void updateBoatHeights(void)
{
  g_boats[0][1] = g_vertices[getClosestVertex(0)][CY];
  g_boats[1][1] = g_vertices[getClosestVertex(1)][CY];
}

/**
 * berechnet die Normale eines Punktes auf Basis der Nachbarpunkte, 
 * wenn ein Nachbar außerhalb des Feldes liegen wuerde, 
 * wird stattdessen der Punkt selbst fuer die Berechnung verwendet
 * @param i Index des Punktes, zu dem die Normale berechnet wird
 * @return Zeiger auf die Normale
 */
CGVector3f *calcNormal(GLint i)
{
  static CGVector3f toReturn = {0.0f, 0.0f, 0.0f};

  //Variablen fuer die Nachbarpunkte
  CGVector3f x1 = {0.0f, 0.0f, 0.0f};
  CGVector3f x2 = {0.0f, 0.0f, 0.0f};
  CGVector3f z1 = {0.0f, 0.0f, 0.0f};
  CGVector3f z2 = {0.0f, 0.0f, 0.0f};
  //Verbindungsvektor zwischen den Nachbarn in x- und in z-Richtung
  CGVector3f x1x2 = {0.0f, 0.0f, 0.0f};
  CGVector3f z1z2 = {0.0f, 0.0f, 0.0f};

  // Rechter Nachbar
  if ((i + 1) % g_amountVerticesSide == 0)
  {
    x2[0] = g_vertices[i][CX];
    x2[1] = g_vertices[i][CY];
    x2[2] = g_vertices[i][CZ];
  }
  else
  {
    x2[0] = g_vertices[i + 1][CX];
    x2[1] = g_vertices[i + 1][CY];
    x2[2] = g_vertices[i + 1][CZ];
  }

  //Linker Nachbar
  if ((i % g_amountVerticesSide) == 0)
  {
    x1[0] = g_vertices[i][CX];
    x1[1] = g_vertices[i][CY];
    x1[2] = g_vertices[i][CZ];
  }
  else
  {
    x1[0] = g_vertices[i - 1][CX];
    x1[1] = g_vertices[i - 1][CY];
    x1[2] = g_vertices[i - 1][CZ];
  }

  //Unterer Nachbar
  if ((i + g_amountVerticesSide) >= SQUARE(g_amountVerticesSide))
  {
    z2[0] = g_vertices[i][CX];
    z2[1] = g_vertices[i][CY];
    z2[2] = g_vertices[i][CZ];
  }
  else
  {
    z2[0] = g_vertices[i + g_amountVerticesSide][CX];
    z2[1] = g_vertices[i + g_amountVerticesSide][CY];
    z2[2] = g_vertices[i + g_amountVerticesSide][CZ];
  }

  //Oberer Nachbar
  if (i < g_amountVerticesSide)
  {
    z1[0] = g_vertices[i][CX];
    z1[1] = g_vertices[i][CY];
    z1[2] = g_vertices[i][CZ];
  }
  else
  {
    z1[0] = g_vertices[i - g_amountVerticesSide][CX];
    z1[1] = g_vertices[i - g_amountVerticesSide][CY];
    z1[2] = g_vertices[i - g_amountVerticesSide][CZ];
  }

  //Verbindungsvektor in X
  x1x2[0] = x2[0] - x1[0];
  x1x2[1] = x2[1] - x1[1];
  x1x2[2] = x2[2] - x1[2];

  //Verbindungsvektor in Z
  z1z2[0] = z1[0] - z2[0];
  z1z2[1] = z1[1] - z2[1];
  z1z2[2] = z1[2] - z2[2];

  //Kreuzprodukt der Vektoren berechnen
  toReturn[0] = x1x2[1] * z1z2[2] - x1x2[2] * z1z2[1];
  toReturn[1] = x1x2[2] * z1z2[0] - x1x2[0] * z1z2[2];
  toReturn[2] = x1x2[0] * z1z2[1] - x1x2[1] * z1z2[0];

  //Normieren
  GLfloat len = sqrt(toReturn[0] * toReturn[0] + toReturn[1] * toReturn[1] + toReturn[2] * toReturn[2]);
  toReturn[0] *= (1.0f / len);
  toReturn[1] *= (1.0f / len);
  toReturn[2] *= (1.0f / len);

  return &toReturn;
}

void updateNormals(void)
{
  GLint i = 0;
  CGVector3f *normal;
  for (i = 0; i < SQUARE(g_amountVerticesSide); i++)
  {
    normal = calcNormal(i);
    g_vertices[i][CNX] = (*normal)[0];
    g_vertices[i][CNY] = (*normal)[1];
    g_vertices[i][CNZ] = (*normal)[2];
  }
}

/**
 * Aktualisiert die Farben aller Kugeln. 
 * Die Farben sind vom Hoehenwert der jeweiligen Kugel abghaengig
 */
void updateColors()
{

  CGColor3f color = {1.0f, 1.0f, 1.0f};
  GLint i = 0;
  for (i = 0; i < SQUARE(g_amountVerticesSide); i++)
  {
    //Farbe fuer niedrige Hoehen
    if (g_vertices[i][CY] < LOWER_BORDER)
    {
      color[0] = LOW_DARK_BLUE[0];
      color[1] = LOW_DARK_BLUE[1];
      color[2] = LOW_DARK_BLUE[2];
    }
    //Farbe fuer mittlere Hoehen
    else if (g_vertices[i][CY] < UPPER_BORDER)
    {
      color[0] = MEDIUM_GREEN[0];
      color[1] = MEDIUM_GREEN[1];
      color[2] = MEDIUM_GREEN[2];
    }
    else
    //Farbe fuer grosse Hoehen
    {
      color[0] = HIGH_RED[0];
      color[1] = HIGH_RED[1];
      color[2] = HIGH_RED[2];
    }
    g_vertices[i][CR] = color[0];
    g_vertices[i][CG] = color[1];
    g_vertices[i][CB] = color[2];
  }
}

/**
 * Fuellt einen Eintrag im Vertex Array am uebergeben Index
 * @param i Index, an dem gearbeitet wird
 * @param currX X-Koordinate diese Punktes
 * @param geiht Y-Koordinate bzw. Hoehe diese Punktes
 * @param currZ Z-Koordinate dieses Punktes
 * @param currTexX X-Texturkoordinate
 * @param currTexY Y-Texturkoordinate
 */
void fillVertexArray(GLint i, GLfloat currX, GLfloat height, GLfloat currZ, GLfloat currTexX, GLfloat currTexY)
{
  //Koordinaten
  g_vertices[i][CX] = currX;
  g_vertices[i][CY] = height;
  g_vertices[i][CZ] = currZ;

  //Textur
  g_vertices[i][CTX] = currTexX;
  g_vertices[i][CTY] = currTexY;
}

/**
 * Initialisiert das Vertex-Array beim Programmstart
 */
void initVertexArray(void)
{
  GLint i = 0;
  // g_amountVerticesSide * g_amountVerticesSide Quadrate im Mesh
  // 2 Dreiecke pro Quadrat
  // 3 Vertices pro Dreieck
  GLint amountIndices = SQUARE(g_amountVerticesSide - 1) * 2 * 3;
  GLfloat currX = -1.0f;
  GLfloat currZ = -1.0f - (2.0f / (g_amountVerticesSide - 1));
  GLfloat currTexX = 0.0f;
  GLfloat currTexY = -1.0f / (g_amountVerticesSide - 1);
  GLfloat *heights = getHeights();
  g_vertices = malloc(sizeof(Vertex) * SQUARE(g_amountVerticesSide));
  if (g_vertices == NULL)
  {
    exit(1);
  }
  else
  {
    g_indices = malloc(sizeof(GLuint) * amountIndices);
    if (g_indices == NULL)
    {
      free(g_vertices);
      exit(1);
    }
  }

  fillIndexArray(amountIndices);

  for (i = 0; i < SQUARE(g_amountVerticesSide); i++)
  {
    if (i % g_amountVerticesSide == 0)
    {
      currX = -1.0f;
      currZ += 2.0f / (g_amountVerticesSide - 1);
      currTexX = 0.0f;
      currTexY += 1.0f / (g_amountVerticesSide - 1);
    }
    else
    {
      currX += 2.0f / (g_amountVerticesSide - 1);
      currTexX += 1.0f / (g_amountVerticesSide - 1);
    }
    fillVertexArray(i, currX, heights[i], currZ, currTexX, currTexY);
  }
  updateColors();
  updateNormals();
}

/**
 * Aktualisiert das Vertex-Array beim Verandern der Aufloesung des Mesh
 */
void updateScene(void)
{
  //Vertex und Color Array definieren und bei Änderungen aktualisieren
  glVertexPointer(3,                     //Komponenten pro Vertex (x,y,z)
                  GL_FLOAT,              //Typ der Komponenten
                  sizeof(Vertex),        //Offset zwischen 2 Vertizes im Array
                  &(g_vertices[0][CX])); //Zeiger auf die 1. Komponente
  glColorPointer(3, GL_FLOAT, sizeof(Vertex), &(g_vertices[0][CR]));
  glNormalPointer(GL_FLOAT, sizeof(Vertex), &(g_vertices[0][CNX]));
  glTexCoordPointer(2, GL_FLOAT, sizeof(Vertex), &(g_vertices[0][CTX]));
}

void updateVertexArray(expandShrinkVertices state)
{
  GLint i = 0;
  // 2 Dreiecke pro Quadrat
  // 3 Vertices pro Dreieck
  GLint amountIndices;
  GLfloat currX = -1.0f;
  GLfloat currZ;
  GLfloat currTexX = 0.0f;
  GLfloat currTexY;
  GLfloat *newHeights;

  g_amountVerticesSide += state;
  //neue Hoehen aus der Logik holen
  newHeights = calloc(SQUARE(g_amountVerticesSide), sizeof(GLfloat));
  if (newHeights != NULL)
  {
    for (i = 0; i < SQUARE(g_amountVerticesSide); i++)
    {
      newHeights[i] = getHeights()[i];
    }

    amountIndices = SQUARE(g_amountVerticesSide - 1) * 2 * 3;
    currZ = -1.0f - (2.0f / (g_amountVerticesSide - 1));
    currTexY = -1.0f / (g_amountVerticesSide - 1);
    //neue Reihenfolge der Indizees zum Zeichnen bestimmen
    g_indices = realloc(g_indices, sizeof(GLuint) * amountIndices);
    if (g_indices != NULL)
    {
      fillIndexArray(amountIndices);
      g_vertices = realloc(g_vertices, sizeof(Vertex) * SQUARE(g_amountVerticesSide));
      if (g_vertices == NULL)
      {
        exit(1);
      }
      else
      {
        //neue Texturkoordinaten berechnen
        for (i = 0; i < SQUARE(g_amountVerticesSide); i++)
        {
          if (i % g_amountVerticesSide == 0)
          {
            currX = -1.0f;
            currZ += 2.0f / (g_amountVerticesSide - 1);
            currTexX = 0.0f;
            currTexY += 1.0f / (g_amountVerticesSide - 1);
          }
          else
          {
            currX += 2.0f / (g_amountVerticesSide - 1);
            currTexX += 1.0f / (g_amountVerticesSide - 1);
          }
          fillVertexArray(i, currX, newHeights[i], currZ, currTexX, currTexY);
          updateNormals();
        }
        updateColors();
      }
    }
    else
    {
      exit(1);
    }
    updateScene();
  }
  else
  {
    exit(1);
  }
}

/**
 * Aktualisiert die Hoehen, indem die Werte aus der Logik uebernommen werden
 */
void updateHeights(void)
{
  GLint i = 0;
  for (i = 0; i < SQUARE(g_amountVerticesSide); i++)
  {
    g_vertices[i][CY] = getHeights()[i];
  }
}

void freeAllocatedMem(void)
{
  free(g_indices);
  free(g_vertices);
}

/**
 * Zeichnet eine Linie mit der uebergebenen Farbe,
 * mit der Laenge 1 entlang der z-Achse
 * 
 * @param[in] color, Farbe des Quadrats
 */
static void drawLine(GLint i, const CGColor3f color)
{
  glBegin(GL_LINES);
  {
    glColor3f(1, 1, 1);
    glVertex3f(g_vertices[i][CNX], g_vertices[i][CNY], g_vertices[i][CNZ]);
    glVertex3f(0.0f, 0.0f, 0.0f);
  }
  glEnd();
}

/**
 * Zeichnet eine Insel in Form eines Zylinders
 */

/**
 * Zeichnet eine Normale von einem Punkt im Vertex-Array
 * @param i Index des Punktes von dem die Normale gezeichnet wird
 * @param color Farbe, in der die Normale gezeichnet wird
 */
static void drawNormal(GLint i, const CGColor3f color)
{
  glPushMatrix();
  {
    glTranslatef(g_vertices[i][CX], g_vertices[i][CY], g_vertices[i][CZ]);
    glScalef(1.0f / 10, 1.0f / 10, 1.0f / 10);
    drawLine(i, color);
  }
  glPopMatrix();
}

/**
 * Zeichnet alle Kugeln an den entsprechenden Punkten basierend auf dem Vertex-Array
 */
static void drawAllSpheres(void)
{
  glPushMatrix();
  {
    glDisable(GL_TEXTURE_2D);
    for (int i = 0; i < SQUARE(g_amountVerticesSide); i++)
    {
      glPushMatrix();
      {

        glTranslatef(g_vertices[i][CX], g_vertices[i][CY], g_vertices[i][CZ]);
        glScalef(1.0f / 20, 1.0f / 20, 1.0f / 20);
        const CGColor3f color = {g_vertices[i][CR], g_vertices[i][CG], g_vertices[i][CB]};
        glPushName((GLuint)i);
        {
          drawSphere(color);
        }
        glPopName();
      }
      glPopMatrix();
    }
  }
  glPopMatrix();
}

/**
 * Zeichnet einen texturierten Leuchtturm
 */
static void drawLighthouse(void)
{
  glPushMatrix();
  {
    glTranslatef(0.0f, LIGHTHOUSE_HEIGHT, 0.0f);
    glRotatef(180, 1.0f, 0.0f, 0.0f);
    bindTexture(texLighthouse);
    drawCylinder(COLOR_WHITE, LIGHTHOUSE_RADIUS, LIGHTHOUSE_RADIUS, LIGHTHOUSE_HEIGHT, GL_TRUE);
  }
  glPopMatrix();

  glPushMatrix();
  {
    glTranslatef(0.0f, LIGHTHOUSE_HEIGHT, 0.0f);
    drawCylinder(COLOR_RED, LIGHTHOUSE_RADIUS, 0.0f, LIGHTHOUSE_HEIGHT / 4.0f, GL_FALSE);
  }
  glPopMatrix();
}

/**
 * Zeichen-Funktion, stellt die Szene dar
 */
void drawScene(void)
{
  if (showHelp)
  {
    drawHelp();
  }
  else
  {
    //Punkte aktualisieren
    updateHeights();
    updateBoatHeights();
    updateColors();
    /* Punktlichtquelle, die die Szene von oben beleuchtet */
    CGPoint4f lightPos0 = {0.0f, 25.0f, 0.0f, 0.0f};

    /* Spotlichtquelle des Leuchtturms, entweder rotierend oder auf ein Boot gerichtet */
    CGPoint4f lightPos1 = {0.0f, 2.0f, 0.0f, 1.0f};
    CGVector3f lightDirection1 = {cos(0), -3.0f, sin(0)};

    /* Rotationswinkel der Lichtquelle */
    GLfloat light1Rotation;

    if (getLightingState())
    {
      /* Lichtberechnung aktivieren */
      glEnable(GL_LIGHTING);

      if (getLight0State())
      {
        /* Position der ersten Lichtquelle setzen */
        glLightfv(GL_LIGHT0, GL_POSITION, lightPos0);
        /* Erste Lichtquelle aktivieren */
        glEnable(GL_LIGHT0);
      }

      if (getLight1State() != off)
      {
        if (getLight1State() == rotating)
        {
          glPushMatrix();
          {
            /* Richtung der zweiten Lichtquelle aus Rotationswinkel berechnen */
            light1Rotation = getLight1Rotation();
            lightDirection1[0] = cosf(DEG2RAD(light1Rotation));
            lightDirection1[2] = sinf(DEG2RAD(light1Rotation));
          }
          glPopMatrix();
        }
        else if (getLight1State() == boat1)
        {
          glPushMatrix();
          {
            lightDirection1[0] = g_boats[0][0] + 0.2f;
            lightDirection1[2] = g_boats[0][2] - 0.25f;
          }
          glPopMatrix();
        }
        else if (getLight1State() == boat2)
        {
          glPushMatrix();
          {
            lightDirection1[0] = g_boats[1][0];
            lightDirection1[2] = g_boats[1][2] + 0.3f;
          }
          glPopMatrix();
        }
        /* Zweite Lichtquelle positionieren */
        glLightfv(GL_LIGHT1, GL_POSITION, lightPos1);
        /* Richtung der zweiten Lichtquelle festlegen */
        glLightfv(GL_LIGHT1, GL_SPOT_DIRECTION, lightDirection1);

        /* Zweite Lichtquelle aktivieren */
        glEnable(GL_LIGHT1);
      }
    }

    if (getTexturingStatus())
    {
      /* Texturierung aktivieren */
      glEnable(GL_TEXTURE_2D);
      glDisableClientState(GL_COLOR_ARRAY);
    }
    else
    {
      glEnableClientState(GL_COLOR_ARRAY);
    }

    glEnable(GL_COLOR_MATERIAL);
    bindTexture(texWater);
    /* Wuerfel */
    glPushMatrix();
    {
      glColor3f(1, 1, 1);
      //Zeichnen (in der drawScene für jeden Frame)
      glDrawElements(GL_TRIANGLES,                             //Primitivtyp
                     SQUARE(g_amountVerticesSide - 1) * 3 * 2, //Anzahl Indizes zum Zeichnen
                     GL_UNSIGNED_INT,                          //Typ der Indizes
                     g_indices);                               //Index Array
    }
    glPopMatrix();

    bindTexture(texIsland);
    glPushMatrix();
    {
      drawIsland();
    }
    glPopMatrix();

    glPushMatrix();
    {
      glTranslatef(0.0f, getIslandHeight() / 2.0f, 0.0f);
      drawLighthouse();
    }
    glPopMatrix();

    if (showSpheres)
    {
      drawAllSpheres();
    }

    drawBoats();

    glPushMatrix();
    {
      if (getShowNormal())
      {
        for (int i = 0; i < SQUARE(g_amountVerticesSide); i++)
        {
          glPushMatrix();
          {
            drawNormal(i, COLOR_WHITE);
          }
          glPopMatrix();
        }
      }
    }
    glPopMatrix();
    /* Erste Lichtquelle deaktivieren */
    glDisable(GL_LIGHT0);
    /* Zweite Lichtquelle deaktivieren */
    glDisable(GL_LIGHT1);
    /* Lichtberechnung deaktivieren */
    glDisable(GL_LIGHTING);
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_COLOR_MATERIAL);

    glPopMatrix();

    /* DEBUG-Ausgabe */
    /*   GLSTATE; */
  }
}

void drawScenePickable(GLboolean spheres)
{
  if (spheres)
  {
    drawAllSpheres();
  }
  else
  {
    drawBoats();
  }
}

/**
 * Initialisierung der Lichtquellen.
 * Setzt Eigenschaften der Lichtquellen (Farbe, Oeffnungswinkel, ...)
 */
static void
initLight(void)
{
  /* Farbe der ersten Lichtquelle */
  CGColor4f lightColor0[3] =
      {{0.2f, 0.2f, 0.2f, 0.2f}, {0.2f, 0.2f, 0.2f, 0.2f}, {0.2f, 0.2f, 0.2f, 0.2f}};

  /* Farbe der zweiten Lichtquelle */
  CGColor4f lightColor1[3] =
      {{1.0f, 1.0f, 1.0f, 1.0f}, {1.0f, 1.0f, 1.0f, 1.0}, {1.0f, 1.0f, 1.0f, 1.0f}};

  /* Erste Lichtquelle aktivieren */
  glEnable(GL_LIGHT0);
  /* Zweite Lichtquelle aktivieren */
  glEnable(GL_LIGHT1);

  /* Farbe der ersten Lichtquelle setzen */
  glLightfv(GL_LIGHT0, GL_AMBIENT, lightColor0[0]);
  glLightfv(GL_LIGHT0, GL_DIFFUSE, lightColor0[1]);
  glLightfv(GL_LIGHT0, GL_SPECULAR, lightColor0[2]);

  /* Farbe der zweiten Lichtquelle setzen */
  glLightfv(GL_LIGHT1, GL_AMBIENT, lightColor1[0]);
  glLightfv(GL_LIGHT1, GL_DIFFUSE, lightColor1[1]);
  glLightfv(GL_LIGHT1, GL_SPECULAR, lightColor1[2]);

  /* Spotlight-Eigenschaften der zweiten Lichtquelle setzen */
  //expo : Intensitaetsverteilung des Lichts
  //cutoff :  oeffnungswinkel der Lichtquelle
  const float expo = 30.0f;
  const float *ptr2 = &expo;
  const float cutoff = 7.0f;
  const float *ptr = &cutoff;
  glLightfv(GL_LIGHT1, GL_SPOT_CUTOFF, ptr);
  glLightfv(GL_LIGHT1, GL_SPOT_EXPONENT, ptr2);
}

/**
 * Initialisierung der Szene (inbesondere der OpenGL-Statusmaschine).
 * Setzt Hintergrund- und Zeichenfarbe.
 * @return Rueckgabewert: im Fehlerfall 0, sonst 1.
 */
int initScene(void)
{
  /* Setzen der Farbattribute */
  /* Hintergrundfarbe */
  glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
  /* Zeichenfarbe */
  glColor3f(1.0f, 1.0f, 1.0f);

  /* Z-Buffer-Test aktivieren */
  glEnable(GL_DEPTH_TEST);

  /* Polygonrueckseiten nicht anzeigen */
  glCullFace(GL_BACK);
  glEnable(GL_CULL_FACE);
  glEnable(GL_NORMALIZE);

  /* Startposition der Kamera festlegen */
  cameraRadiussph = CAMERA_DEFAULT_RADIUS;
  cameraThetaSph = CAMERA_DEFAULT_THETA;
  cameraPhiSph = CAMERA_DEFAULT_PHI;

  //einmalig Vertex und Color Array aktivieren
  glEnableClientState(GL_VERTEX_ARRAY);
  glEnableClientState(GL_NORMAL_ARRAY);
  glEnableClientState(GL_TEXTURE_COORD_ARRAY);
  //glEnableClientState(GL_COLOR_ARRAY);
  glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);

  calcCylinderPoints();
  g_amountVerticesSide = START_AMOUNT_VERTICES;
  initLight();
  initVertexArray();
  updateScene();
  updateBoatHeights();

  glLineWidth(2.0f);

  /* Alles in Ordnung? */
  return (GLGETERROR == GL_NO_ERROR);
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

/**
 * (De-)aktiviert die Anzeige der Normalen.
 */
void toggleNormal(void)
{
  setShowNormal(!getShowNormal());
}

/**
 * (De-)aktiviert die Anzeige der Kugeln.
 */
void toggleSpheres(void)
{
  showSpheres = !showSpheres;
}

/**
 * Setzt den Kamera-Radius im KugelKoord.
 * Berechnet die Koord im kart. neu
 * 
 * @param[in] rad, Radius
 */
void setCameraRadius(GLfloat rad)
{
  cameraRadiussph = rad;
}

/**
 * Setzt den Kamera-Theta Winkel im KugelKoord.
 * Berechnet die Koord im kart. neu
 * 
 * @param[in] theta, winkel
 */
void setCameraTheta(GLfloat theta)
{
  cameraThetaSph = theta;
}

/**
 * Setzt den Kamera-Phi Winkel im KugelKoord.
 * Berechnet die Koord im kart. neu
 * 
 * @param[in] phi, winkel
 */
void setCameraPhi(GLfloat phi)
{
  cameraPhiSph = phi;
}

/**
 * Liefert den Kamera-Phi Winkel im KugelKoord.
 * 
 * @return phi
 */
GLfloat getCameraPhi(void)
{
  return cameraPhiSph;
}

/**
 * Liefert den Kamera-Theta Winkel im KugelKoord.
 * 
 * @return theta
 */
GLfloat getCameraTheta(void)
{
  return cameraThetaSph;
}

/**
 * Liefert den Kamera-Radius im KugelKoord.
 * 
 * @return radius
 */
GLfloat getCameraRadius(void)
{
  return cameraRadiussph;
}

GLint getAmountVertices(void)
{
  return g_amountVerticesSide;
}

void toggleShowHelp(void)
{
  showHelp = !showHelp;
}

GLfloat getBoatCHeight(GLint index)
{
  return g_boats[index][1];
}
