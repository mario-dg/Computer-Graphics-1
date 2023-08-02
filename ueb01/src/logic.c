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
#include <assert.h>
#include <float.h>

/* ---- Eigene Header einbinden ---- */
#include "logic.h"
#include "types.h"

/* ---- Konstanten ---- */

/** Drehgeschwindigkeit des Dreiecks */
#define rotSpeed 60.0f

/** Geschwindigkeit des Schlaegers */
#define g_batSpeed 0.5f

/** Konstante fuer Berechnungen mit PI */
#define PI 3.141592653589793f

/** Punkte, die das Zerstoeren einer Box bringen */
#define pointsForBox 1

/** Wert, mit dem die Geschwindigkeit des Balles erhoeht werden soll, 
   wenn der Punkte Score ein Vielfaches von 10 erreicht */
#define speedInc 0.15f

/* Standard Geschwindigkeit des Balles */
#define defaultSpeed 0.61f

/** Standard Fallgeschwindigkeit der Extras*/
#define defaultExtraDropSpeed 0.5f

// Wahrscheinlichkeit, dass ein Extra spawnt
// (1 / den Wert)
#define spawnExtraChance 20
// Laengerer Schlaeger
#define longerBatValue 0.15f

// Langsamerer Ball
#define slowerBallValue 1.25f

// Bonus Punkte
#define bonusPointsValue 5

/* ---- Globale Daten ---- */

GLint amntNotDestroyedBoxes = ROWS * COLS;

/** der Mittelpunkt des Schlaegers */
static CGPoint2f g_batCenter = {0.0f, -0.9f};

/** Momentane Ball Geschwindigkeit */
GLfloat g_ballSpeed = defaultSpeed;

/** Alle Boxen **/
box g_allBoxes[ROWS][COLS];

/** Momentane Schlaeger-Breite */
GLfloat g_batWidth = QUAD_WIDTH_BAT;

/** Aktueller Punktestand */
GLint g_scoreBoard = 0;

/** Aktueller Lebenshausalt */
GLint g_lives = 3;

/** Beschreibt die Erlaubnis, damit der Schlaeger sich bewegen darf (Am Spielende auf FALSE) */
GLboolean g_allowMovement = GL_TRUE;

/**
 * Bewegungsstatus des Schlaegers. Gibt fuer beide Richtungen an, ob sich der 
 * Schlaeger in die jeweilige Richtung bewegt
 */
static GLboolean g_movement[2] = {GL_FALSE, GL_FALSE};

/* ---- Funktionen ---- */
/**
 * Liefert true, wenn das Extra erstellt werden soll
 * @param[in] GLint Spawn Wahrscheinlichkeit
 * @return GLboolean
 */
static GLint spawnChance(void)
{
  return rand() % spawnExtraChance;
}

/**
 * Erstellt ein neues Extra anhand seiner Wahrscheinlichkeit
 * @param[in] position and der das Extra erstellt werden soll
 * 
 */
CGExtra createExtra()
{
  CGExtra newExtra = e_normal;
  GLint spawn = spawnChance();

  switch (spawn)
  {
  case 0:
    newExtra = e_longerBat;
    break;
  case 1:
    newExtra = e_slowerBall;
    break;
  case 2:
    newExtra = e_bonusPoints;
    break;
  case 3:
    newExtra = e_extraBalls;
    break;
  }

  return newExtra;
}

/**
 * Erstellt einen Ball ueber dem Mittelpunkt des Schlaegers mit der Farbe Rot und gibt ihm eine
 * zufaellige Richtung (45° positiv und negativ ausgehend von der y-Achse im 0-Punkt)
 * 
 */
void createBall(void)
{
  if (g_allBalls.used == g_allBalls.size)
  {
    g_allBalls.array = realloc(g_allBalls.array, (g_allBalls.size + 2) * sizeof(ball));
    g_allBalls.size += 2;
  }
  CGColor3f color = {1.0f, 0.0f, 0.0f};
  GLint angle = (rand() % 91) - 45;
  CGVector2f direction = {sin((angle / 180.0f * PI)), cos(angle / 180.0f * PI)};
  ball currBall = {{g_batCenter[0], g_batCenter[1] + QUAD_HEIGHT_BAT / 2 + CIRCLE_RADIUS},
                   {direction[0], direction[1]},
                   {color[0], color[1], color[2]},
                   GL_TRUE};
  g_allBalls.array[g_allBalls.used] = currBall;
  g_allBalls.used++;
  g_allBalls.activeBalls++;
}

/**
 * Setzt den Bewegunsstatus des Rechtecks.
 * @param direction Bewegungsrichtung deren Status veraendert werden soll.
 * @param status neuer Status der Bewegung: GL_TRUE->Rechteck bewegt sich in Richtung
 * direction, GL_FALSE->Rechteck bewegt sich nicht in Richtung direction.
 */
void setMovement(CGDirection direction, GLboolean status)
{
  if (direction <= dirRight)
  {
    g_movement[direction] = status;
  }
}

void initBoxes(void)
{
  GLint x = 0;
  GLint y = 0;
  srand(time(0));

  /* Position des ersten Blocks*/
  CGPoint2f pos = {-1.0f + FRAME_WIDTH + QUAD_WIDTH_BLOCK,
                   1.0f - FRAME_WIDTH - QUAD_HEIGHT_BLOCK - spaceX};

  for (x = 0; x < ROWS; x++)
  {
    for (y = 0; y < COLS; y++)
    {
      // Damit die Bloecke nicht zu dunkel sind, Gruen-Wert auf ein Minimum setzen
      CGColor3f boxColor = {
          (float)rand() / RAND_MAX,
          0.5f,
          (float)rand() / RAND_MAX,
      };

      extra currExtraType;
      currExtraType.type = createExtra();
      currExtraType.center[0] = g_allBoxes[x][y].center[0];
      currExtraType.center[1] = g_allBoxes[x][y].center[1];
      currExtraType.direction[0] = 0.0f;
      currExtraType.direction[1] = 0.0f;
      currExtraType.visible = GL_FALSE;
      currExtraType.angle = 15.0f;
      CGColor3f extraColor;
      // Eine Farbe passen zum Extra setzen
      switch (currExtraType.type)
      {
      case e_longerBat:
        // Pink
        extraColor[0] = 1.0f;
        extraColor[1] = 0.0f;
        extraColor[2] = 1.0f;
        break;
      case e_slowerBall:
        // Tuerkis
        extraColor[0] = 0.0f;
        extraColor[1] = 1.0f;
        extraColor[2] = 1.0f;
        break;
      case e_bonusPoints:
        // Gruen
        extraColor[0] = 0.0f;
        extraColor[1] = 1.0f;
        extraColor[2] = 0.0f;
        break;
      case e_extraBalls:
        // Gelb
        extraColor[0] = 1.0f;
        extraColor[1] = 1.0f;
        extraColor[2] = 0.0f;
        break;
      default:
        extraColor[0] = 1.0f;
        extraColor[1] = 1.0f;
        extraColor[2] = 1.0f;
      }
      currExtraType.color[0] = extraColor[0];
      currExtraType.color[1] = extraColor[1];
      currExtraType.color[2] = extraColor[2];

      box currBox = {{pos[0], pos[1]}, {boxColor[0], boxColor[1], boxColor[2]}, 1, currExtraType};
      g_allBoxes[x][y] = currBox;
      // x-Position um eine Block-Breite und den horizontalen Abstand vergroeßern
      pos[0] += QUAD_WIDTH_BLOCK + spaceX;
    }
    // x-Position wieder an den Anfang einer neuen Zeile setzen
    pos[0] = -1.0f + FRAME_WIDTH + QUAD_WIDTH_BLOCK;
    // y-Position um eine Block-Hoehe und den vertikalen Abstand vergroeßern
    pos[1] -= QUAD_HEIGHT_BLOCK + spaceY;
  }
}

/**
 * Deaktiviert die momentan aktiven Extras
 * 
 */
void deactivateExtras()
{
  g_batWidth = QUAD_WIDTH_BAT;
  g_ballSpeed = defaultSpeed;
}

/**
 * Gibt dem zu aktivierendem Extra einen Bewegungsvektor und macht es sichtbar
 * 
 * @param[in] CGPosition, Koordinaten des zerstoerten Blockes,
 * aus dem das Extra spawnen soll
 */
void startDropExtra(CGPosition pos)
{
  g_allBoxes[pos[0]][pos[1]].extraType.visible = GL_TRUE;
  g_allBoxes[pos[0]][pos[1]].extraType.direction[1] = -1.0f;
}

void updateOuput(void)
{
  // Punkte ausgeben
  system("clear");
  printf("Aktueller Punktestand: %i             Leben uebrig: %i\n", g_scoreBoard, g_lives);
}

/**
 * Aktiviert das eingesammelte Extra
 * 
 * @param[in], CGPosition, Koordinaten des zerstoerten Blockes,
 * aus dem das Extra gespawnt ist
 */
void activateExtra(CGPosition pos)
{
  if (g_allBoxes[pos[0]][pos[1]].extraType.type == e_longerBat)
  {
    g_batWidth += longerBatValue;
  }
  else if (g_allBoxes[pos[0]][pos[1]].extraType.type == e_slowerBall)
  {
    g_ballSpeed /= slowerBallValue;
  }
  else if (g_allBoxes[pos[0]][pos[1]].extraType.type == e_bonusPoints)
  {
    g_scoreBoard += bonusPointsValue;
    updateOuput();
  }
  else if (g_allBoxes[pos[0]][pos[1]].extraType.type == e_extraBalls)
  {
    createBall();
    createBall();
  }

  // Setzt das Extra wieder auf unsichtbar und stoppt seine Bewegung
  g_allBoxes[pos[0]][pos[1]].extraType.visible = GL_FALSE;
  g_allBoxes[pos[0]][pos[1]].extraType.direction[1] = 0.0f;
}

/**
 * Prueft, ob das Spiel vorbei ist
 * 
 * @return GLboolean, true = vorbei; false = laeuft noch
 */
GLboolean checkEndOfGame(void)
{
  return ((amntNotDestroyedBoxes == 0) || (g_lives == 0));
}

/**
 * Kuemmert sich um die Beendigung des Spiels
 * 
 */
void handleEndOfGame(void)
{
  GLint x = 0;
  GLint y = 0;
  // Haelt alle Baelle an
  g_ballSpeed = 0.0f;
  // Haelt alle Extras an, falls beim Spielende noch Extras
  // am fallen sind
  for (x = 0; x < ROWS; x++)
  {
    for (y = 0; y < COLS; y++)
    {
      g_allBoxes[x][y].extraType.direction[1] = 0.0f;
    }
  }
  g_allowMovement = GL_FALSE;
}

/**
 * Prueft, ob der uebergebene Ball mit dem Schlaeger kollidiert ist oder
 * ins Aus geflogen ist
 * 
 * @param[in] Index des Balles aus dem Ball Array
 * @return 1000: Ball kollidiert nicht mit dem Schlaeger
 * @return 900 : Ball ist ins Aus geflogen
 * @return alpha: Winkel, indem der Ball vom Schlaeger abprallt
 */
static GLfloat
checkBatCollision(GLint ballIdx)
{
  GLfloat alpha = 1000;
  // Im y-Bereich niedriger als die Oberseite des Schlaegers
  if (g_allBalls.array[ballIdx].direction[1] < 0.0f && g_allBalls.array[ballIdx].center[1] - CIRCLE_RADIUS <= g_batCenter[1] + QUAD_HEIGHT_BAT / 2)
  {
    // Richtiger X Bereich (Zwischen den beiden vertikalen Enden)
    /** Momentane Schlaeger-Breite */
    if ((g_allBalls.array[ballIdx].center[0] + CIRCLE_RADIUS >= g_batCenter[0] - g_batWidth / 2) && (g_allBalls.array[ballIdx].center[0] - CIRCLE_RADIUS <= g_batCenter[0] + g_batWidth / 2))
    {
      // Ball hat den Schlaeger getroffen
      alpha = ((g_allBalls.array[ballIdx].center[0] - g_batCenter[0]) / (g_batWidth / 2 + CIRCLE_RADIUS)) * 45;
    }
    else
    {
      // Ball ist ins Aus geflogen
      alpha = 900;
    }
  }

  return alpha;
}

/**
 * Handelt passend zur Rueckgabe der Kollisionspruefung mit dem Schlaeger
 * 
 * @param[in] angle: Winkel in dem der Ball vom Schlaeger abprallen soll, wenn er kollidiert ist
 * @param[in] ballIdx: Momentan gepruefter Ball
 * @param[in] ErrorCode: Rueckgabewert der Kollisionpruefung
 * 
 */
void handleBatCollision(GLfloat angle, GLint ballIdx)
{
  // Der Ball ist vom Schlaeger abgeprallt, liefert also einen Winkel zwischen -45° und +45°
  if (angle < 900)
  {
    g_allBalls.array[ballIdx].direction[0] = sin((angle * PI) / 180);
    g_allBalls.array[ballIdx].direction[1] = cos((angle * PI) / 180);
  }
  else
  {
    // Ball ist ins Aus geflogen
    g_allBalls.activeBalls--;
    // Ball unsichtbar machen
    g_allBalls.array[ballIdx].visible = GL_FALSE;
    // Nicht mehr bewegen
    g_allBalls.array[ballIdx].direction[0] = 0.0f;
    g_allBalls.array[ballIdx].direction[1] = 0.0f;

    // Keine Extra Balls vorhanden -> Leben verloren
    if (g_allBalls.activeBalls == 0)
    {
      g_lives--;
      deactivateExtras();
      // Solange der Spieler noch Leben hat, wird ein neuer Ball gespawnt
      if (checkEndOfGame())
      {
        handleEndOfGame();
        printf("\n\n\nGAME OVER!\n Punktestand: %d\n", g_scoreBoard);
      }
      else
      {
        createBall();
        g_ballSpeed = defaultSpeed;
      }
    }
  }
}

/**
 * Prueft, ob der Ball mit einer Box kollidiert ist 
 * @param[in] ballIdx, des momentan geprueften Balles
 * @return Box-Kante, mit der kollidiert wurde
 */
static CGSide
checkBoxCollision(GLint ballIdx)
{
  CGSide
      res = sideNone;
  GLint x = 0;
  GLint y = 0;
  // Distanz vom Kreis-Mittelpunkt zur oberen Kante
  GLfloat distTop = FLT_MAX;
  // Distanz vom Kreis-Mittelpunkt zur unteren Kante
  GLfloat distBot = FLT_MAX;
  // Distanz vom Kreis-Mittelpunkt zur linken Kante
  GLfloat distLeft = FLT_MAX;
  // Distanz vom Kreis-Mittelpunkt zur rechten Kante
  GLfloat distRight = FLT_MAX;

  for (x = 0; (x < ROWS) && (res == sideNone); x++)
  {
    for (y = 0; (y < COLS) && (res == sideNone); y++)
    {
      // Kollisionspruefung nur, wenn die Box noch nicht zerstoert wurde
      if (g_allBoxes[x][y].visible)
      {
        distLeft = fabs((g_allBoxes[x][y].center[0] - (QUAD_WIDTH_BLOCK / 2)) - g_allBalls.array[ballIdx].center[0]);
        distRight = fabs((g_allBoxes[x][y].center[0] + (QUAD_WIDTH_BLOCK / 2)) - g_allBalls.array[ballIdx].center[0]);
        distTop = fabs((g_allBoxes[x][y].center[1] + (QUAD_HEIGHT_BLOCK / 2)) - g_allBalls.array[ballIdx].center[1]);
        distBot = fabs((g_allBoxes[x][y].center[1] - (QUAD_HEIGHT_BLOCK / 2)) - g_allBalls.array[ballIdx].center[1]);

        //Ball fliegt nach rechts und trifft auf die linke Kante einer Box
        if ((distLeft <= CIRCLE_RADIUS) && ((g_allBalls.array[ballIdx].center[1] >= g_allBoxes[x][y].center[1] - (QUAD_HEIGHT_BLOCK / 2)) && (g_allBalls.array[ballIdx].center[1] <= g_allBoxes[x][y].center[1] + (QUAD_HEIGHT_BLOCK / 2))) && g_allBalls.array[ballIdx].direction[0] > 0.0f)
        {
          res = sideLeft;
        }

        // Ball fliegt nach links und trifft auf die rechte Kante einer Box
        else if ((distRight <= CIRCLE_RADIUS) && ((g_allBalls.array[ballIdx].center[1] >= g_allBoxes[x][y].center[1] - (QUAD_HEIGHT_BLOCK / 2)) && (g_allBalls.array[ballIdx].center[1] <= g_allBoxes[x][y].center[1] + (QUAD_HEIGHT_BLOCK / 2))) && g_allBalls.array[ballIdx].direction[0] < 0.0f)
        {
          res = sideRight;
        }

        // Ball fliegt nach oben und trifft auf die untere Kante einer Box
        else if ((distBot <= CIRCLE_RADIUS) && ((g_allBalls.array[ballIdx].center[0] >= g_allBoxes[x][y].center[0] - (QUAD_WIDTH_BLOCK / 2)) && (g_allBalls.array[ballIdx].center[0] <= g_allBoxes[x][y].center[0] + (QUAD_WIDTH_BLOCK / 2))) && g_allBalls.array[ballIdx].direction[1] > 0.0f)
        {
          res = sideBot;
        }

        // Ball fliegt nach unten und trifft auf die obere Kante einer Box
        else if ((distTop <= CIRCLE_RADIUS) && ((g_allBalls.array[ballIdx].center[0] >= g_allBoxes[x][y].center[0] - (QUAD_WIDTH_BLOCK / 2)) && (g_allBalls.array[ballIdx].center[0] <= g_allBoxes[x][y].center[0] + (QUAD_WIDTH_BLOCK / 2))) && g_allBalls.array[ballIdx].direction[1] < 0.0f)
        {
          res = sideTop;
        }

        //Kollision mit einer Box
        if (res != sideNone)
        {

          // Reduziert die Anzahl der unzerstoerten Boxen
          amntNotDestroyedBoxes--;
          // Entfernt Box ausm Feld
          g_allBoxes[x][y].visible = 0;
          CGPosition currPos = {x, y};
          startDropExtra(currPos);
          // Punkte fuer die zerstoerte Box hinzufuegen
          g_scoreBoard += pointsForBox;
          updateOuput();
          // Ball Geschwindigkeit alle 10 Punkte erhoehen
          if ((g_scoreBoard % 10) == 0)
          {
            g_ballSpeed += speedInc;
          }
          // Leben alle 20 Punkte
          if ((g_scoreBoard % 20) == 0)
          {
            g_lives++;
            updateOuput();
          }
          if (checkEndOfGame())
          {
            handleEndOfGame();
            printf("\n\n\nGEWONNEN!\nPunktestand: %d\n", g_scoreBoard);
          }
        }
      }
    }
  }

  return res;
}

/**
 * Prueft die Kollision von Extras mit dem Schlaeger
 * 
 * @return 0: faellt noch
 * @return 1: aufgefangen
 * @return -1: weggefallen
 * @return 2: Kein Extra in der Box
 */
GLint checkExtraCollision(box box)
{
  GLint result = 0;

  // Kollision nur mit sichtbare und nicht normalen Extras
  if (box.extraType.visible && box.extraType.type != e_normal)
  {
    // Kollision erfolgt
    if (((box.center[0] >= g_batCenter[0] - g_batWidth / 2) && box.center[0] <= g_batCenter[0] + g_batWidth / 2) && (box.center[1] <= g_batCenter[1] + QUAD_HEIGHT_BAT / 2) && (box.center[1] >= g_batCenter[1] - QUAD_HEIGHT_BAT))
    {
      result = 1;
    }

    // Extra ist am Schlaeger vorbei ins Aus geflogen
    if (box.center[1] < g_batCenter[1] - QUAD_HEIGHT_BAT / 2)
    {
      result = -1;
    }
  }
  return result;
}

/**
 * Prueft, ob das Rechteck mit dem Rahmen kollidiert ist.
 * @param[in] ballIdx, des momentan geprueften Balles
 * @return Rahmenseite, mit der kollidiert wurde.
 */
static CGSide
checkCollision(GLint ballIdx)
{
  CGSide
      res = sideNone;

  /* Quadrat fliegt nach rechts und
     die rechte Seite des Quadrats ueberschreitet den rechten Rand */
  if (g_allBalls.array[ballIdx].direction[0] > 0.0f &&
      g_allBalls.array[ballIdx].center[0] + CIRCLE_RADIUS >= BORDER_RIGHT)
  {
    res = sideRight;
  }

  /* Quadrat fliegt nach links und
     die linke Seite des Quadrats ueberschreitet den linken Rand */
  else if (g_allBalls.array[ballIdx].direction[0] < 0.0f &&
           g_allBalls.array[ballIdx].center[0] - CIRCLE_RADIUS <= BORDER_LEFT)
  {
    res = sideLeft;
  }

  /* Quadrat fliegt nach oben und
     die obere Seite des Quadrats ueberschreitet den oberen Rand */
  else if (g_allBalls.array[ballIdx].direction[1] > 0.0f &&
           g_allBalls.array[ballIdx].center[1] + CIRCLE_RADIUS >= BORDER_TOP)
  {
    res = sideTop;
  }

  return res;
}

/**
 * Reagiert auf Kollisionen des Rechtecks mit dem Rahmen.
 * @param[in] side Rahmenseite, mit der kollidiert wurde.
 * @param[in] ballIdx, des momentan geprueften Balles
 */
static void
handleCollision(CGSide side, GLint ballIdx)
{
  /* Bewegung in X-Richtung umkehren */
  if (side == sideLeft || side == sideRight)
  {
    g_allBalls.array[ballIdx].direction[0] *= -1;
  }

  /* Bewegung in Y-Richtung umkehren */
  if (side == sideTop || side == sideBot)
  {
    g_allBalls.array[ballIdx].direction[1] *= -1;
  }
}

/**
 * Berechnet neue Position des Rechtecks.
 * @param interval Dauer der Bewegung in Sekunden.
 */
void calcPosition(double interval)
{
  GLint idx = 0;
  // Position aller Baelle neu berechnen
  for (idx = 0; idx < g_allBalls.used; idx++)
  {
    // Berechnung nur, wenn der Ball noch im Spiel ist
    if (g_allBalls.array[idx].visible)
    {

      CGSide
          side = sideNone;

      // Rahmenkollision
      side = checkCollision(idx);

      // Abprallwinkel vom Schlaeger
      GLfloat bounceOfBatAngle = checkBatCollision(idx);

      // Ball ist vom Schlaeger abgeprallt oder ins Aus geflogen
      if (bounceOfBatAngle != 1000)
      {
        handleBatCollision(bounceOfBatAngle, idx);
      }
      else if (side != sideNone)
      {
        // Ball am Rahmen kollidiert
        handleCollision(side, idx);
      }
      else
      {
        // Ball ist mit der Box kollidiert
        side = checkBoxCollision(idx);
        if (side != sideNone)
        {
          handleCollision(side, idx);
        }
      }

      // Neu Berechnung der Ball-Center-Koordinaten
      g_allBalls.array[idx].center[0] += g_allBalls.array[idx].direction[0] * (float)interval * g_ballSpeed;
      g_allBalls.array[idx].center[1] += g_allBalls.array[idx].direction[1] * (float)interval * g_ballSpeed;
    }
  }
}

void calcPositionExtra(double interval)
{
  GLint x = 0;
  GLint y = 0;

  for (x = 0; x < ROWS; x++)
  {
    for (y = 0; y < COLS; y++)
    {
      // Anpassung nur von sichtbaren, nicht normalenExtras
      if (g_allBoxes[x][y].extraType.visible && g_allBoxes[x][y].extraType.type != e_normal)
      {
        GLint extraStatus = checkExtraCollision(g_allBoxes[x][y]);
        CGPosition currPos = {x, y};

        // Bei Auffangen des Extras -> Aktivieren
        if (extraStatus == 1)
        {
          activateExtra(currPos);
        }
        else if (extraStatus == -1)
        {
          // Beim nicht auffangen -> unsichtbar und unbeweglich machen
          g_allBoxes[x][y].extraType.visible = GL_FALSE;
          g_allBoxes[x][y].extraType.direction[1] = 0.0f;
        }
        // Neu Positionierung und Rotation nur waehrend des aktivem Spiels
        if (g_allowMovement)
        {
          g_allBoxes[x][y].center[1] += g_allBoxes[x][y].extraType.direction[1] * (float)interval * defaultExtraDropSpeed;
          g_allBoxes[x][y].extraType.angle += rotSpeed * (float)interval;
        }
      }
    }
  }
}

void calcPositionBat(double interval)
{
  if (g_allowMovement)
  {
    if (g_movement[dirLeft] && g_batCenter[0] >= BORDER_LEFT)
    {
      g_batCenter[0] -= g_batSpeed * (float)interval;
    }
    if (g_movement[dirRight] && g_batCenter[0] <= BORDER_RIGHT)
    {
      g_batCenter[0] += g_batSpeed * (float)interval;
    }
  }
}

void initGame(void)
{
  initBoxes();
  g_allBalls.array = calloc(10, sizeof(ball));
  g_allBalls.size = 10;
  g_allBalls.activeBalls = 0;
  g_allBalls.used = 0;
  createBall();
}

ball *getBall(GLint ballIdx)
{
  return &g_allBalls.array[ballIdx];
}

GLint getAmountUsedBalls(void)
{
  return g_allBalls.used;
}

CGPoint2f *
getBatCenter(void)
{
  return &g_batCenter;
}

GLfloat getBatWidth(void)
{
  return g_batWidth;
}

box getBox(GLint row, GLint col)
{
  assert((row >= 0) && (row < ROWS) && (col >= 0) && (col < COLS));
  {
    return g_allBoxes[row][col];
  }
}
