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

GLboolean isFullscreen = GL_FALSE;

/**
 * Setzen der Projektionsmatrix.
 * Setzt die Projektionsmatrix unter Beruecksichtigung des Seitenverhaeltnisses
 * des Anzeigefensters, sodass das Seitenverhaeltnisse der Szene unveraendert
 * bleibt und gleichzeitig entweder in x- oder y-Richtung der Bereich von -1
 * bis +1 zu sehen ist.
 * @param aspect Seitenverhaeltnis des Anzeigefensters (In).
 */
static void
setProjection(GLdouble aspect)
{
  /* Nachfolgende Operationen beeinflussen Projektionsmatrix */
  glMatrixMode(GL_PROJECTION);
  /* Matrix zuruecksetzen - Einheitsmatrix laden */
  glLoadIdentity();

  /* Koordinatensystem bleibt quadratisch */
  if (aspect <= 1)
  {
    gluOrtho2D(-1.0, 1.0,                    /* links, rechts */
               -1.0 / aspect, 1.0 / aspect); /* unten, oben */
  }
  else
  {
    gluOrtho2D(-1.0 * aspect, 1.0 * aspect, /* links, rechts */
               -1.0, 1.0);                  /* unten, oben */
  }
}

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
 * @param isSpecialKey ist die Taste eine Spezialtaste?
 */
static void
handleKeyboardEvent(int key, int status, GLboolean isSpecialKey)
{
  /** Keycode der ESC-Taste */
#define ESC 27
  GLint i = 0;
  GLint k = 0;

  /* Taste gedrueckt */
  if (status == GLUT_DOWN)
  {
    /* Spezialtaste gedrueckt */
    if (isSpecialKey)
    {
      switch (key)
      {
        /* (De-)Aktivieren des Wireframemode */
      case GLUT_KEY_F1:
        toggleWireframeMode();
        glutPostRedisplay();
        break;
        // Togglen des Vollbildmodus
      case GLUT_KEY_F2:
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
        /* Bewegung des Rechtecks in entsprechende Richtung starten */
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
        setShowHelp(!getShowHelp());
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
        // Gewonnen oder Verloren Ausgabe mit einer beliebigen
        // noch nicht belegten nicht special Taste beenden
      default:
        if (g_gameState == won)
        {
          // Bei gewonnenem Spiel, nächstes Level starten
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
  double interval = (double)(thisCallTime - lastCallTime) / 1000.0f;
  if (getIsPaused())
  {
    interval = 0;
  }

  // Berechnung der Skalierung des Radius
  calcRadiusPortal(interval);

  // Berechnung des Countdowns
  calcTimer(interval);

  /* Wieder als Timer-Funktion registrieren */
  glutTimerFunc(1000 / TIMER_CALLS_PS, cbTimer, thisCallTime);

  /* Neuzeichnen anstossen */
  glutPostRedisplay();
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
  /* Das ganze Fenster ist GL-Anzeigebereich */
  glViewport(0, 0, (GLsizei)w, (GLsizei)h);

  /* Anpassen der Projektionsmatrix an das Seitenverhaeltnis des Fensters */
  setProjection((GLdouble)w / (GLdouble)h);
}

/**
 * Zeichen-Callback.
 * Loescht die Buffer, ruft das Zeichnen der Szene auf und tauscht den Front-
 * und Backbuffer.
 */
static void
cbDisplay(void)
{
  /* Buffer zuruecksetzen */
  glClear(GL_COLOR_BUFFER_BIT);

  /* Nachfolgende Operationen beeinflussen Modelviewmatrix */
  glMatrixMode(GL_MODELVIEW);
  /* Matrix zuruecksetzen - Einheitsmatrix laden */
  glLoadIdentity();

  /* Szene zeichnen */
  drawScene();

  /* Objekt anzeigen */
  /* glFlush (); */  /* fuer SingleBuffering */
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
  glutIgnoreKeyRepeat(1);

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
  /* glutInitDisplayMode (GLUT_SINGLE | GLUT_RGB); */ /* fuer SingleBuffering */
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);        /* fuer DoubleBuffering */
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
  else
  {
  }

  return windowID;
}
