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

#include <stdio.h>
#include <math.h>
#include "types.h"
#include "scene.h"


#define M_PI 3.141592654

#define M_PI_2 (M_PI / 2.0f)

#define M_PI_4 (M_PI_2 / 2.0f)

#define SLICES (8)
#define NUM_SLICES_ISLAND (10)
/**Anzahl der Segmente fuer den Leuchtturm*/
#define NUM_SLICES_LIGHTHOUSE (25)
/**Hoehe der Insel*/
#define ISLAND_HEIGHT (0.4f)

#define NUM_QUADS (4)

CGVector3f circlePoints[NUM_SLICES_ISLAND];
GLboolean showNormal = GL_FALSE;

static const CGColor3f COLOR_BROWN = {0.59f, 0.29f, 0.0f};

/**
 * Zeichnet eine Linie mit der uebergebenen Farbe,
 * mit der Laenge 1 entlang der z-Achse
 * 
 * @param[in] color, Farbe des Quadrats
 */
static void drawLine(const CGColor3f color)
{
  /* Material des Wuerfels */
  float matShininess[] = {5.0f};

  /* Setzen der Material-Parameter */
  glMaterialfv(GL_FRONT, GL_AMBIENT, color);
  glMaterialfv(GL_FRONT, GL_DIFFUSE, color);
  glMaterialfv(GL_FRONT, GL_SPECULAR, color);
  glMaterialfv(GL_FRONT, GL_SHININESS, matShininess);

  glBegin(GL_LINES);
  {
    glColor3f(color[0], color[1], color[2]);
    glVertex3f(0.0f, 0.0f, -0.5f);
    glVertex3f(0.0f, 0.0f, 0.5f);
  }
  glEnd();
}

static void drawNormal(const CGColor3f color)
{
  glPushMatrix();
  {
    glLineWidth(2.0f);
    glRotatef(90, 1.0f, 0.0f, 0.0f);
    glScalef(0.5f, 1.0f, 1.0f);
    drawLine(color);
  }
  glPopMatrix();
}

/**
 * Zeichnet ein Quadrat mit der uebergebenen Farbe, 
 * mit Seitenlaenge 1 um den Ursprung rum
 * 
 * @param[in] color, Farbe des Quadrats
 */
static void drawRect(const CGColor3f color)
{
  /* Material des Wuerfels */
  float matShininess[] = {100.0f};

  /* Setzen der Material-Parameter */
  glMaterialfv(GL_FRONT, GL_AMBIENT, color);
  glMaterialfv(GL_FRONT, GL_DIFFUSE, color);
  glMaterialfv(GL_FRONT, GL_SPECULAR, color);
  glMaterialfv(GL_FRONT, GL_SHININESS, matShininess);

  glBegin(GL_QUADS);
  {
    glNormal3f(0.0f, 1.0f, 0.0f);
    glColor3f(color[0], color[1], color[2]);
    glVertex3f(-0.5f, 0.0f, -0.5f);
    glVertex3f(-0.5f, 0.0f, 0.5f);
    glVertex3f(0.5f, 0.0f, 0.5f);
    glVertex3f(0.5f, 0.0f, -0.5f);
  }
  glEnd();
}

/**
 * Zeichnet eine Flaeche mit der uebergebenen Farbe, 
 * mit Seitenlaenge 1 um den Ursprung rum.
 * Zusammengesetzt aus AMOUNT_PIXEL_PER_BLOCK kleineren Quadraten
 * 
 * @param[in] color, Farbe des Quadrats
 */
static void drawPlane(const CGColor3f color)
{
  GLint i = 0;
  GLint m = 0;

  glPushMatrix();
  {
    // Verschieben des Mittelpunktes der Grundflaeche in den Ursprung
    glTranslatef(-0.5f + 0.5f * 1.0f / NUM_QUADS, 0.0f, -0.5f + 0.5f * 1.0f / NUM_QUADS);
    for (i = 0; i < NUM_QUADS; i++)
    {
      glPushMatrix();
      {
        glTranslatef(0.0f, 0.0f, (1.0f / NUM_QUADS) * i);
        for (m = 0; m < NUM_QUADS; m++)
        {
          glPushMatrix();
          {
            glTranslatef((1.0f / NUM_QUADS) * m, 0.0f, 0.0f);
            glScalef(1.0f / NUM_QUADS, 1.0f, 1.0f / NUM_QUADS);
            drawRect(color);
          }
          glPopMatrix();
        }
      }
      glPopMatrix();
    }
  }
  glPopMatrix();
}

/**
 * Zeichnet einen Wuerfel mit Kantenlaenge 1.
 */
static void
drawCube(GLboolean normal)
{
  /* Unterseite */
  glPushMatrix();
  {
    glRotatef(180, 1.0f, 0.0f, 0.0f);
    drawPlane(COLOR_BROWN);
    if (showNormal)
    {
      drawNormal(COLOR_BROWN);
    }
  }
  glPopMatrix();

  /* rechte Seitenflaeche */
  glPushMatrix();
  {
    glTranslatef(0.5f, 0.5f, 0.0f);
    glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
    glRotatef(-90.0f, 0.0f, 0.0f, 1.0f);
    drawPlane(COLOR_BROWN);
    if (showNormal)
    {
      drawNormal(COLOR_BROWN);
    }
  }
  glPopMatrix();

  /* linke Seite */
  glPushMatrix();
  {
    glTranslatef(-0.5f, 0.5f, 0.0f);
    glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
    glRotatef(-90.0f, 0.0f, 0.0f, 1.0f);
    glRotatef(180, 1.0f, 0.0f, 0.0f);
    drawPlane(COLOR_BROWN);
    if (showNormal)
    {
      drawNormal(COLOR_BROWN);
    }
  }
  glPopMatrix();

  /* hintere Seite */
  glPushMatrix();
  {
    glTranslatef(0.0f, 0.5f, -0.5f);
    glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
    glRotatef(180, 1.0f, 0.0f, 0.0f);
    drawPlane(COLOR_BROWN);
    if (showNormal)
    {
      drawNormal(COLOR_BROWN);
    }
  }
  glPopMatrix();

  /* vordere Seite */
  glPushMatrix();
  {
    glTranslatef(0.0f, 0.5f, 0.5f);
    glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
    drawPlane(COLOR_BROWN);
    if (showNormal)
    {
      drawNormal(COLOR_BROWN);
    }
  }
  glPopMatrix();

  /* Bodenflaeche */
  glPushMatrix();
  {
    glTranslatef(0.0f, 1.0f, 0.0f);
    drawPlane(COLOR_BROWN);
    if (showNormal)
    {
      drawNormal(COLOR_BROWN);
    }
  }
  glPopMatrix();
}

void drawSphere(const CGColor3f color)
{
  /* Material der Kugel*/
  float matShininess[] = {128.0f};

  /* Setzen der Material-Parameter fuer die Beleuchtung */
  CGColor3f temp = {0.75f, 0.75f, 0.75f};
  glMaterialfv(GL_FRONT, GL_AMBIENT, temp);
  glMaterialfv(GL_FRONT, GL_DIFFUSE, temp);
  glMaterialfv(GL_FRONT, GL_SPECULAR, temp);
  glMaterialfv(GL_FRONT, GL_SHININESS, matShininess);

  const GLfloat radius = 0.5f;

  glPushMatrix();
  {
    glRotatef(-90, 1.0f, 0.0f, 0.0f);
    /* Quadric erzuegen */
    GLUquadricObj *qobj = gluNewQuadric();

    if (qobj != 0)
    {
      glColor3f(color[0], color[1], color[2]);
      /* Gefuellt darstellen */
      gluQuadricDrawStyle(qobj, GLU_FILL);
      /* Scheibe zeichnen */
      gluSphere(qobj, radius, SLICES, SLICES);
      gluQuadricNormals(qobj, GLU_SMOOTH);
      /* Loeschen des Quadrics nicht vergessen */
      gluDeleteQuadric(qobj);
    }
    else
    {
      fprintf(stderr, "Erzeugen der Kugeln fehlgeschlagen\n");
    }
  }
  glPopMatrix();
}

void calcCylinderPoints(void)
{
  GLint i = 0;
  GLdouble theta = 2 * M_PI;
  GLdouble step = 2 * M_PI / NUM_SLICES_ISLAND;
  while (theta > 0)
  {
    circlePoints[i][0] = cos(theta);
    circlePoints[i][1] = 0.0f;
    circlePoints[i][2] = sin(theta);
    theta -= step;
    i++;
  }
}

/**
 * Zeichnet einen Kreis mit Radius 1 um den Ursprung rum
 */
static void drawCircle(void)
{
  GLint i = 0;
  GLfloat theta = 0.0f;
  glNormal3f(0.0, 1.0f, 0.0f);
  glBegin(GL_POLYGON);
  {
    glColor3f(1.0f, 1.0f, 1.0f);
    for (i = 0; i < NUM_SLICES_ISLAND; i++)
    {
      theta = 2 * M_PI - ((float)i / NUM_SLICES_ISLAND) * 2 * M_PI;
      glTexCoord2f(cos(theta), sin(theta));
      glVertex3f(circlePoints[i][0], circlePoints[i][1], circlePoints[i][2]);
    }
  }
  glEnd();
}

void drawCylinder(const CGColor3f color, GLfloat lowerRadius, GLfloat upperRadius, GLfloat height, GLboolean texturing)
{
  /* Material des Wuerfels */
  float matShininess[] = {5.0f};
  CGColor3f temp = {0.75f, 0.75f, 0.75f};
  /* Setzen der Material-Parameter */
  glMaterialfv(GL_FRONT, GL_AMBIENT, temp);
  glMaterialfv(GL_FRONT, GL_DIFFUSE, temp);
  glMaterialfv(GL_FRONT, GL_SPECULAR, temp);
  glMaterialfv(GL_FRONT, GL_SHININESS, matShininess);

  glPushMatrix();
  {
    glRotatef(-90, 1.0f, 0.0f, 0.0f);
    /* Quadric erzuegen */
    GLUquadricObj *qobj = gluNewQuadric();

    if (qobj != 0)
    {

      glColor3f(color[0], color[1], color[2]);
      /* Gefuellt darstellen */
      gluQuadricDrawStyle(qobj, GLU_FILL);

      gluQuadricTexture(qobj, texturing);
      /* Scheibe zeichnen */
      gluCylinder(qobj, lowerRadius, upperRadius, height, NUM_SLICES_LIGHTHOUSE, NUM_SLICES_LIGHTHOUSE);

      gluQuadricNormals(qobj, GLU_SMOOTH);
      /* Loeschen des Quadrics nicht vergessen */
      gluDeleteQuadric(qobj);
    }
    else
    {
      fprintf(stderr, "Erzeugen des Zylinders fehlgeschlagen\n");
    }
  }
  glPopMatrix();
}

void drawIsland(void)
{
  GLint i = 0;
  GLfloat theta = 0.0f;
  //Oberer Kreis
  glPushMatrix();
  {
    glTranslatef(0.0f, ISLAND_HEIGHT / 2.0f, 0.0f);
    glScalef(0.25f, 1.0f, 0.25f);
    drawCircle();
  }
  glPopMatrix();

  //Tube als TRIANGLE_STRIP, abwechselnd ein Punkt auf dem oberen und ein PUnkt auf dem unteren Kreis
  glPushMatrix();
  {
    glScalef(0.25f, 1.0f, 0.25f);
    glBegin(GL_TRIANGLE_STRIP);
    {
      glColor3f(1.0f, 1.0f, 1.0f);
      for (i = 0; i <= NUM_SLICES_ISLAND; i++)
      {
        theta = 2 * M_PI - ((float)i / NUM_SLICES_ISLAND) * 2 * M_PI;
        glNormal3f(cos(theta), 0.0f, sin(theta));
        glTexCoord2f(sin(theta), cos(theta));
        glVertex3f(circlePoints[i % NUM_SLICES_ISLAND][0], circlePoints[i % NUM_SLICES_ISLAND][1] + ISLAND_HEIGHT / 2.0f, circlePoints[i % NUM_SLICES_ISLAND][2]);
        glTexCoord2f(sin(theta), cos(theta));
        glVertex3f(circlePoints[i % NUM_SLICES_ISLAND][0], circlePoints[i % NUM_SLICES_ISLAND][1] - ISLAND_HEIGHT / 2.0f, circlePoints[i % NUM_SLICES_ISLAND][2]);
      }
    }
    glEnd();
  }
  glPopMatrix();

  //Anzeigen der Normalen des Zylinders
  if (showNormal)
  {
    glPushMatrix();
    {
      glTranslatef(0.0f, ISLAND_HEIGHT / 2.0f - 0.01, 0.0f);
      glBegin(GL_LINES);
      {
        glColor3f(1.0f, 1.0f, 1.0f);
        for (i = 0; i <= NUM_SLICES_ISLAND; i++)
        {
          glPushMatrix();
          {
            theta = 2 * M_PI - ((float)i / NUM_SLICES_ISLAND) * 2 * M_PI;
            glTranslatef(circlePoints[i % NUM_SLICES_ISLAND][0], circlePoints[i % NUM_SLICES_ISLAND][1] + ISLAND_HEIGHT / 2.0f, circlePoints[i % NUM_SLICES_ISLAND][2]);
            glVertex3f(cos(theta) / 3.0f, 0.0f, sin(theta) / 3.0f);
            glVertex3f(0.0f, 0.0f, 0.0f);
          }
          glPopMatrix();
        }
      }
      glEnd();
    }
    glPopMatrix();
  }
}

void drawBoats(void)
{
  glPushName(boat1);
  {
    //Rumpf
    glPushMatrix();
    {
      glTranslatef(BOAT_1_X, getBoatCHeight(0), BOAT_1_Z);
      glScalef(0.35f, 0.1f, 0.25f);
      drawCube(showNormal);
    }
    glPopMatrix();

    //Spitze
    glPushMatrix();
    {
      glTranslatef(BOAT_1_X - 0.175f, getBoatCHeight(0), BOAT_1_Z);
      glRotatef(45, 0.0f, 1.0f - 0.00001f, 0.0f);
      glScalef(0.175f, 0.1f, 0.175f);
      drawCube(showNormal);
    }
    glPopMatrix();

    //Fracht
    glPushMatrix();
    {
      glTranslatef(BOAT_1_X + 0.1f, getBoatCHeight(0) + 0.075f, BOAT_1_Z);
      glScalef(0.15f, 0.15f, 0.15f);
      drawCube(showNormal);
    }
    glPopMatrix();
  }
  glPopName();

  glPushName(boat2);
  {
    //Rumpf
    glPushMatrix();
    {
      glTranslatef(BOAT_2_X, getBoatCHeight(1), BOAT_2_Z);
      glScalef(0.35f, 0.1f, 0.25f);
      drawCube(showNormal);
    }
    glPopMatrix();

    //Spitze
    glPushMatrix();
    {
      glTranslatef(BOAT_2_X - 0.175f, getBoatCHeight(1), BOAT_2_Z);
      glRotatef(45, 0.0f, 1.0f, 0.0f);
      glScalef(0.175f, 0.1f - 0.00001f, 0.175f);
      drawCube(showNormal);
    }
    glPopMatrix();

    //Fracht
    glPushMatrix();
    {
      glTranslatef(BOAT_2_X + 0.1f, getBoatCHeight(1) + 0.075f, BOAT_2_Z);
      glScalef(0.15f, 0.15f, 0.15f);
      drawCube(showNormal);
    }
    glPopMatrix();
  }
  glPopName();
}

GLboolean getShowNormal(void)
{
  return showNormal;
}

void setShowNormal(GLboolean normal)
{
  showNormal = normal;
}

GLfloat getIslandHeight(void)
{
  return ISLAND_HEIGHT;
}