/**
 * @file
 * Ein-/Ausgabe-Modul.
 * Das Modul kapselt die Ein- und Ausgabe-Funktionalitaet (insbesondere die GLUT-
 * Callbacks) des Programms.
 *
 * Bestandteil eines Beispielprogramms fuer Animationen mit OpenGL & GLUT.
 *
 * @author copyright (C) Fachhochschule Wedel 1999-2011. All rights reserved.
 * @author Leonhard Brandes, Mario da Graca
 */

/* ---- System Header einbinden ---- */
#include <stdlib.h>
#include <stdio.h>

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

/* ---- Eigene Header einbinden ---- */
#include "io.h"
#include "types.h"
#include "logic.h"
#include "scene.h"
#include "stringOutput.h"
#include <math.h>

/* ---- Konstanten ---- */
/** Anzahl der Aufrufe der Timer-Funktion pro Sekunde */
#define TIMER_CALLS_PS 60
#define CAMERA_MOVEMENT_STEPS 2.0f
#define PUSHY_STEPS_PS 10.0f

//#define M_PI_2 1.570796327
//#define M_PI_4 0.785398163

#define MIN_THETA 0.001f
#define MAX_THETA M_PI_2

#define MIN_RADIUS 1.0f
#define MAX_RADIUS 75.0f

#define NEAR_CLIPPING_PLANE 0.1f
#define FAR_CLIPPING_PLANE 100.0f

double interval = 0.0f;

GLboolean isFullscreen = GL_FALSE;
GLboolean egoPerspective = GL_FALSE;
GLboolean pushyIsMoving = GL_FALSE;
GLfloat startPushyMovement = 0.0f;

/**
 * Verarbeitung eines Tasturereignisses.
 * Pfeiltasten steuern die Position des angezeigten Rechtecks.
 * F1-Taste (de-)aktiviert Wireframemodus.
 * F2-Taste schaltet zwischen Fenster und Vollbilddarstellung um.
 * ESC-Taste und q, Q beenden das Programm.
 * Falls Debugging aktiviert ist, wird jedes Tastaturereignis auf stdout
 * ausgegeben.
 * @param key Taste, die das Ereignis ausgeloest hat. (ASCII-Wert oder WERT des
 *        GLUT_KEY_<SPECIAL>.
 * @param status Status der Taste, GL_TRUE=gedrueckt, GL_FALSE=losgelassen.
 * @param isspecialKey ist die Taste eine Spezialtaste?
 */
static void
handleKeyboardEvent(int key, int status, GLboolean isspecialKey)
{
  /** Keycode der ESC-Taste */
#define ESC 27
  GLint i = 0;
  GLint k = 0;

  /* Taste gedrueckt */
  if (status == GLUT_DOWN)
  {
    /* Spezialtaste gedrueckt */
    if (isspecialKey)
    {
      switch (key)
      {
        /* (De-)Aktivieren des Wireframemode */
      case GLUT_KEY_F1:
        toggleWireframeMode();
        glutPostRedisplay();
        break;
        /* Normalen anzeigen */
      case GLUT_KEY_F2:
        toggleNormal();
        break;
        /* Fixe Lichtquelle anzeigen */
      case GLUT_KEY_F3:
        toggleFixedLight();
        break;
        /* Spotlight anzeigen */
      case GLUT_KEY_F4:
        toggleSpotlight();
        break;
        /* Lichtberechnung aktivieren */
      case GLUT_KEY_F5:
        toggleLight();
        break;
        // Togglen des Vollbildmodus
      case GLUT_KEY_F6:
        isFullscreen = !isFullscreen;
        if (isFullscreen)
        {
          glutFullScreen();
        }
        else
        {
          glutReshapeWindow(DEFAULT_WINDOW_WIDTH, DEFAULT_WINDOW_HEIGHT);
        }
        break;
        /* Bewegung des Spielers in entsprechende Richtung starten */
      case GLUT_KEY_LEFT:
        move(dirLeft);
        break;
      case GLUT_KEY_RIGHT:
        move(dirRight);
        break;
      case GLUT_KEY_UP:
        move(dirUp);
        break;
      case GLUT_KEY_DOWN:
        move(dirDown);
        break;
        // Gewonnen oder Verloren Ausgabe mit einer beliebigen
        // noch nicht belegten special Taste beenden
      default:
        if (g_gameState == won)
        {
          if (getLevelIndex() < 2)
          {
            initLogic(getLevelIndex() + 1);
          }
          else
          {
            initLogic(getLevelIndex());
          }
        }
        else if (g_gameState == timesUp)
        {
          initLogic(getLevelIndex());
        }
      }
    }
    /* normale Taste gedrueckt */
    else
    {
      switch (key)
      {
        /* Programm beenden */
      case 'q':
      case 'Q':
      case ESC:
        for (i = 0; i < AMOUNT_LEVELS; i++)
        {
          for (k = 0; k < g_allLevels[i].dimensions[0]; k++)
          {
            free(g_allLevels[i].currLevel[k]);
          }
          free(g_allLevels[i].currLevel);
        }
        exit(0);
        break;
        // Level neustarten
      case 'r':
      case 'R':
        initLogic(getLevelIndex());
        break;
        // pausieren
      case 'p':
      case 'P':
        setPaused(!getIsPaused());
        break;
        // Hilfe ausgeben togglen
      case 'h':
      case 'H':
        if (g_gameState == running)
        {
          g_allowMovement = !g_allowMovement;
        }
        toggleShowHelp();
        break;
        // Level auswaehlen
      case '1':
        initLogic(0);
        break;
      case '2':
        initLogic(1);
        break;
      case '3':
        initLogic(2);
        break;
        /* ranzoomen */
      case 'u':
      case 'U':
        if (getCameraRadius() > MIN_RADIUS)
        {
          setCameraRadius(getCameraRadius() - (15 * CAMERA_MOVEMENT_STEPS * interval));
          if ( getCameraRadius() < MIN_RADIUS){
            setCameraRadius(MIN_RADIUS);
          }
        }
        break;
        /* rauszoomen */
      case 'o':
      case 'O':
        if (getCameraRadius() < MAX_RADIUS)
        {
          setCameraRadius(getCameraRadius() + (15 * CAMERA_MOVEMENT_STEPS * interval));
          if ( getCameraRadius() > MAX_RADIUS){
            setCameraRadius(MAX_RADIUS);
          }
        }
        break;
      case 'i':
      case 'I':
        if (getCameraTheta() > MIN_THETA)
        {
          setCameraTheta(getCameraTheta() - (CAMERA_MOVEMENT_STEPS * interval));
          if (getCameraTheta() < MIN_THETA)
          {
            setCameraTheta(MIN_THETA);
          }
        }
        break;
        /* Kamera bewegen */
      case 'j':
      case 'J':
        setCameraPhi(getCameraPhi() + (CAMERA_MOVEMENT_STEPS * interval));
        break;
      case 'k':
      case 'K':
        if (getCameraTheta() < MAX_THETA)
        {
          setCameraTheta(getCameraTheta() + (CAMERA_MOVEMENT_STEPS * interval));
          if (getCameraTheta() > MAX_THETA)
          {
            setCameraTheta(MAX_THETA);
          }
        }
        break;
      case 'l':
      case 'L':
        setCameraPhi(getCameraPhi() - (CAMERA_MOVEMENT_STEPS * interval));
        break;
        /* Perspektive wechseln */
      case 'c':
      case 'C':
        egoPerspective = !egoPerspective;
        break;
        /* Spieleranimation aktivieren */
      case 'a':
      case 'A':
        toggleAnimation();
        break;
        // Gewonnen oder Verloren Ausgabe mit einer beliebigen
        // noch nicht belegten nicht special Taste beenden
      default:
        if (g_gameState == won)
        {
          // Bei gewonnenem Spiel, naechstes Level starten
          // sofern vorhanden
          if (getLevelIndex() < 2)
          {
            initLogic(getLevelIndex() + 1);
          }
          else
          {
            initLogic(getLevelIndex());
          }
        }
        else if (g_gameState == timesUp)
        {
          // Bei verlorenem Spiel, erneut versuchen
          initLogic(getLevelIndex());
        }
      }
    }
  }
}

/**
 * Callback fuer Tastendruck.
 * Ruft Ereignisbehandlung fuer Tastaturereignis auf.
 * @param key betroffene Taste (In).
 */
static void
cbKeyboard(unsigned char key, int x, int y)
{
  handleKeyboardEvent(key, GLUT_DOWN, GL_FALSE);
}

/**
 * Callback fuer Druck auf Spezialtasten.
 * Ruft Ereignisbehandlung fuer Tastaturereignis auf.
 * @param key betroffene Taste (In).
 */
static void
cbSpecial(int key, int x, int y)
{
  handleKeyboardEvent(key, GLUT_DOWN, GL_TRUE);
}

/**
 * Timer-Callback.
 * Initiiert Berechnung des Portalradius und des Countdowns und anschliessendes
 * Neuzeichnen, setzt sich selbst erneut als Timer-Callback.
 * @param lastCallTime Zeitpunkt, zu dem die Funktion als Timer-Funktion
 *   registriert wurde (In).
 */
static void
cbTimer(int lastCallTime)
{
  /* Seit dem Programmstart vergangene Zeit in Millisekunden */
  int thisCallTime = glutGet(GLUT_ELAPSED_TIME);

  /* Seit dem letzten Funktionsaufruf vergangene Zeit in Sekunden */
  interval = (double)(thisCallTime - lastCallTime) / 1000.0f;
  if (getIsPaused())
  {
    interval = 0;
  }

  // Berechnung der Skalierung des Radius
  calcRadiusPortal(interval);

  // Berechnung des Countdowns
  calcTimer(interval);

  // Berechnung der Pushy Position
  calcPushyPos(lastMoveDir, interval);

  /* Wieder als Timer-Funktion registrieren */
  glutTimerFunc(1000 / TIMER_CALLS_PS, cbTimer, thisCallTime);

  /* Neuzeichnen anstossen */
  glutPostRedisplay();
}

/**
 * Setzt einen Viewport fuer 3-dimensionale Darstellung
 * mit perspektivischer Projektion und legt eine Kamera fest.
 * Ruft das zeichnen der Szene in diesem Viewport auf.
 * 
 * @param x, y Position des Viewports im Fenster - (0, 0) ist die untere linke Ecke
 * @param width, height Breite und Hoehe des Viewports
 * @param lookAt enthaelt die fuer glLookAt benoetigten Daten zur Kamera (Augpunkt, Zentrum, Up-Vektor)
 */
static void
set3DViewport(GLint x, GLint y, GLint width, GLint height, GLdouble lookAt[9])
{
  /* Seitenverhaeltnis bestimmen */
  double aspect = (double)width / height;

  /* Folge Operationen beeinflussen die Projektionsmatrix */
  glMatrixMode(GL_PROJECTION);

  /* Einheitsmatrix laden */
  glLoadIdentity();

  /* Viewport-Position und -Ausdehnung bestimmen */
  glViewport(x, y, width, height);

  /* Perspektivische Darstellung */
  gluPerspective(70,     /* oeffnungswinkel */
                 aspect, /* Seitenverhaeltnis */
                 NEAR_CLIPPING_PLANE,    /* nahe Clipping-Ebene */
                 FAR_CLIPPING_PLANE);   /* ferne Clipping-Ebene */

  /* Folge Operationen beeinflussen die Modelviewmatrix */
  glMatrixMode(GL_MODELVIEW);

  /* Einheitsmatrix laden */
  glLoadIdentity();

  CGVector3f lookDir = {0.0f, 0.0f, 0.0f};
  switch (lastMoveDir)
  {
  case dirUp:
    lookDir[0] = getPushyPosFloats()[1];
    lookDir[1] = 0.0f;
    lookDir[2] = -10.0f;
    break;
  case dirDown:
    lookDir[0] = getPushyPosFloats()[1];
    ;
    lookDir[1] = 0.0f;
    lookDir[2] = 10.0f;
    break;
  case dirLeft:
    lookDir[0] = -10.0f;
    lookDir[1] = 0.0f;
    lookDir[2] = getPushyPosFloats()[0];
    break;
  case dirRight:
    lookDir[0] = 10.0f;
    lookDir[1] = 0.0f;
    lookDir[2] = getPushyPosFloats()[0];
    break;
  }

  if (egoPerspective)
  {
    gluLookAt(getPushyPosFloats()[1], g_cellSize + g_cellSize / 2, getPushyPosFloats()[0], /* Augpunkt */
              lookDir[0], lookDir[1], lookDir[2],                                          /* Zentrum */
              lookAt[6], lookAt[7], lookAt[8]);                                            /* Up-Vektor */
  }
  else
  {
    /* Kameraposition */
    gluLookAt(lookAt[0], lookAt[1], lookAt[2],  /* Augpunkt */
              lookAt[3], lookAt[4], lookAt[5],  /* Zentrum */
              lookAt[6], lookAt[7], lookAt[8]); /* Up-Vektor */
  }
  /* Szene zeichnen lassen */
  drawScene3D();
}

/**
 * Setzt einen Viewport fuer 2-dimensionale Darstellung.
 * Ruft das zeichnen der Szene in diesem Viewport auf.
 * 
 * @param x, y Position des Viewports im Fenster - (0, 0) ist die untere linke Ecke
 * @param width, height Breite und Hoehe des Viewports
 */
static void
set2DViewport(GLint x, GLint y, GLint width, GLint height)
{
  /* Seitenverhaeltnis bestimmen */
  double aspect = (double)width / height;

  /* Folge Operationen beeinflussen die Projektionsmatrix */
  glMatrixMode(GL_PROJECTION);

  /* Einheitsmatrix laden */
  glLoadIdentity();

  /* Viewport-Position und -Ausdehnung bestimmen */
  glViewport(x, y, width, height);

  /* Das Koordinatensystem bleibt immer quadratisch */
  if (aspect <= 1)
  {
    glOrtho(-8, 8,                             /* left, right */
            -8 / aspect, 8 / aspect, -3, 0.2); /* bottom, top */
  }
  else
  {
    glOrtho(-8 * aspect, 8 * aspect, /* left, right */
            -8, 8, -3, 0.2);         /* bottom, top */
  }

  /* Folge Operationen beeinflussen die Modelviewmatrix */
  glMatrixMode(GL_MODELVIEW);

  /* Einheitsmatrix laden */
  glLoadIdentity();

  /* Szene zeichnen lassen */
  drawScene2D();
}

/**
 * Callback fuer Aenderungen der Fenstergroesse.
 * Initiiert Anpassung der Projektionsmatrix an veraenderte Fenstergroesse.
 * @param w Fensterbreite (In).
 * @param h Fensterhoehe (In).
 */
static void
cbReshape(int w, int h)
{
  (void)w;
  (void)h;
  /* 
   * Viewport-Anpassung hier nun unnoetig,
   * da sie in der cbDisplay sowieso staendig neu gesetzt werden.
   */
}

/**
 * Zeichen-Callback.
 * Loescht die Buffer, ruft das Zeichnen der Szene auf und tauscht den Front-
 * und Backbuffer.
 */
static void
cbDisplay(void)
{
  /* Fensterdimensionen auslesen */
  int width = glutGet(GLUT_WINDOW_WIDTH);
  int height = glutGet(GLUT_WINDOW_HEIGHT);

  GLfloat cameraX = getCameraRadius() * sin(getCameraTheta()) * cos(getCameraPhi());
  GLfloat cameraY = getCameraRadius() * cos(getCameraTheta());
  GLfloat cameraZ = getCameraRadius() * sin(getCameraTheta()) * sin(getCameraPhi());

  /* Kamera von vorne */
  GLdouble lookAt1[9] = {cameraX, cameraY, cameraZ, /* Augpunkt */
                         0.0, 0.0, 0.0,             /* Zentrum */
                         0.0, 1.0, 0.0};            /* Up-Vektor */

  /* Framewbuffer und z-Buffer zuruecksetzen */
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  /* 3D-Viewports setzen */
  set3DViewport(0, 0,              /* x, y */
                width / 2, height, /* breite, hoehe */
                lookAt1);

  /* 2D-Viewport setzen */
  set2DViewport(width / 2, 0,       /* x, y */
                width / 2, height); /* breite, hoehe */

  glutSwapBuffers(); /* fuer DoubleBuffering */
}

/**
 * Registrierung der GLUT-Callback-Routinen.
 */
static void
registerCallbacks(void)
{
  /* Tasten-Druck-Callback - wird ausgefuehrt, wenn eine Taste gedrueckt wird */
  glutKeyboardFunc(cbKeyboard);

  /* Spezialtasten-Druck-Callback - wird ausgefuehrt, wenn Spezialtaste
   * (F1 - F12, Links, Rechts, Oben, Unten, Bild-Auf, Bild-Ab, Pos1, Ende oder
   * Einfuegen) gedrueckt wird */
  glutSpecialFunc(cbSpecial);

  /* Automat. Tastendruckwiederholung ignorieren */
  glutIgnoreKeyRepeat(0);

  /* Timer-Callback - wird einmalig nach msescs Millisekunden ausgefuehrt */
  glutTimerFunc(1000 / TIMER_CALLS_PS,       /* msecs - bis Aufruf von func */
                cbTimer,                     /* func  - wird aufgerufen    */
                glutGet(GLUT_ELAPSED_TIME)); /* value - Parameter, mit dem
                                                   func aufgerufen wird */

  /* Reshape-Callback - wird ausgefuehrt, wenn neu gezeichnet wird (z.B. nach
   * Erzeugen oder Groessenaenderungen des Fensters) */
  glutReshapeFunc(cbReshape);

  /* Display-Callback - wird an mehreren Stellen imlizit (z.B. im Anschluss an
   * Reshape-Callback) oder explizit (durch glutPostRedisplay) angestossen */
  glutDisplayFunc(cbDisplay);
}

/**
 * Initialisiert das Programm (inkl. I/O und OpenGL) und startet die
 * Ereignisbehandlung.
 * @param title Beschriftung des Fensters
 * @param width Breite des Fensters
 * @param height Hoehe des Fensters
 * @return ID des erzeugten Fensters, 0 im Fehlerfall
 */
int initAndStartIO(char *title, int width, int height)
{
  int windowID = 0;

  /* Kommandozeile immitieren */
  int argc = 1;
  char *argv = "cmd";

  /* Glut initialisieren */
  glutInit(&argc, &argv);

  /* Initialisieren des Fensters */
  /* RGB-Framewbuffer, Double-Buffering und z-Buffer anfordern */
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
  glutInitWindowSize(width, height);
  glutInitWindowPosition(0, 0);

  /* Fenster erzeugen */
  windowID = glutCreateWindow(title);

  if (windowID)
  {

    if (initScene())
    {
      registerCallbacks();

      glutMainLoop();
    }
    else
    {

      glutDestroyWindow(windowID);
      windowID = 0;
    }
  }

  return windowID;
}
