/**
 * @file
 * Ein-/Ausgabe-Modul.
 * Das Modul kapselt die Ein- und Ausgabe-Funktionalitaet (insbesondere die GLUT-
 * Callbacks) des Programms.
 *
 * Bestandteil eines Beispielprogramms fuer Picking mit OpenGL & GLUT.
 *
 * @author copyright (C) Fachhochschule Wedel 1999-2011. All rights reserved.
 */

/* ---- System Header einbinden ---- */
#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#include "utility.h"

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/freeglut.h>
#endif

/* ---- Eigene Header einbinden ---- */
#include "io.h"
#include "types.h"
#include "scene.h"
#include "debugGL.h"

/* ---- Konstanten ---- */
/** Anzahl der Aufrufe der Timer-Funktion pro Sekunde */
#define TIMER_CALLS_PS 60
#define ELEVATION_VALUE (0.01f)

//Statusvariablen
static GLboolean isFullscreen = GL_FALSE;
static GLboolean isPaused = GL_FALSE;

/**
 * Gibt die Hilfe auf der Standardausgabe aus
 * 
*/
static void printHelp(void)
{
  printf("Bedienungsanleitung 'Shader':\n");
  printf("'q'/'Q'/ESC: Programm schließen.\n");
  printf("'p'/'P': Pausiert das Programm.\n");
  printf("'h'/'H': Gibt die Hilfeausgabe auf stdout aus.\n");
  printf("'l'/'L': Phong-Beleuchtung an-/ausschaltern.\n");
  printf("'t'/'T': Textur an-/ausschaltern.\n");
  printf("'c'/'C': Cartoon-Shading an-/ausschaltern.\n");
  printf("'s'/'S': Sepia-Shading an-/ausschaltern.\n");
  printf("'g'/'G': Gray-Shading an-/ausschaltern.\n");
  printf("'+'/'-': Staerke der Verschiebung durch die Heightmap anpassen.\n");
  printf("F1: Schaltet zwischen WireFrame und Flaechendarstellung um.\n");
  printf("F2: Normalen Anzeige an-/aussschalten.\n");
  printf("F3: Heightmap Anzeige an-/aussschalten.\n");
  printf("F12: Vollbildmodus an-/ausschaltern.\n");
}

int initOpenGL(void)
{
  /* DEBUG-Ausgabe */
  INFO(("Initialisiere OpenGL...\n"));

  /* Beim Aufruf von glClear werden alle Pixel des Framebuffers auf
   * diesen Wert gesetzt. */
  glClearColor(0, 0, 0, 0);

  /* Beim Aufrus von glClear werden alle Pixel des Depthbuffer auf
   * diesen Wert gesetzt */
  glClearDepth(1);

  /* Aktivieren des Tiefentests */
  glEnable(GL_DEPTH_TEST);

  /* DEBUG-Ausgabe */
  INFO(("...fertig.\n\n"));

  /* Alles in Ordnung? */
  return 1;
}

/**
 * Timer-Callback.
 * Initiiert Berechnung der aktuellen Position und Farben und anschliessendes
 * Neuzeichnen, setzt sich selbst erneut als Timer-Callback.
 * @param lastCallTime Zeitpunkt, zu dem die Funktion als Timer-Funktion
 *   registriert wurde (In).
 */
static void
cbTimer(int lastCallTime)
{
  (void)lastCallTime;
  /* Seit dem Programmstart vergangene Zeit in Millisekunden */
  int thisCallTime = glutGet(GLUT_ELAPSED_TIME);

  double interval = (thisCallTime - lastCallTime) / 1000.0f;

  if (isPaused)
  {
    interval = 0;
  }

  setTimer(interval);

  /* Wieder als Timer-Funktion registrieren */
  glutTimerFunc(1000 / TIMER_CALLS_PS, cbTimer, thisCallTime);

  /* Neuzeichnen anstossen */
  glutPostRedisplay();
}

/**
 * Zeichen-Callback.
 * Loescht die Buffer, ruft das Zeichnen der Szene auf und erzwingt die
 * Darstellung durch OpenGL.
 */
static void
cbDisplay(void)
{
  /* Buffer zuruecksetzen */
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  /* Szene zeichnen */
  drawScene();

  /* Objekt anzeigen */
  glutSwapBuffers();
}

/**
 * Debug-Callback.
 * @param type Art der Debug-Information. Einer der folgenden Werte:
 *     DEBUG_TYPE_ERROR_ARB
 *     DEBUG_TYPE_DEPRECATED_BEHAVIOR_ARB
 *     DEBUG_TYPE_UNDEFINED_BEHAVIOR_ARB
 *     DEBUG_TYPE_PORTABILITY_ARB
 *     DEBUG_TYPE_PERFORMANCE_ARB
 *     DEBUG_TYPE_OTHER_ARB
 * @param message Debug-Nachricht.
 */
#ifdef GLEW_CORRECT
static void
cbDebugOutput(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar *message, const void *userParam)
{
  if (type == GL_DEBUG_TYPE_ERROR_ARB)
  {
    printf("ARB_debug_output: %s\n", message);
    exit(1);
  }
}
#endif

/**
 * Callback fuer Tastendruck.
 * Ruft Ereignisbehandlung fuer Tastaturereignis auf.
 * @param key betroffene Taste (In).
 * @param x x-Position der Maus zur Zeit des Tastendrucks (In).
 * @param y y-Position der Maus zur Zeit des Tastendrucks (In).
 */
static void
handleKeyboardEvent(unsigned char key, int status, GLboolean isSpecialKey, int x, int y)
{
  (void)x;
  (void)y;
/** Keycode der ESC-Taste */
#define ESC 27
  /* Taste gedrueckt */
  if (status == GLUT_DOWN)
  {
    debugState state = no_debug;
    /* Spezialtaste gedrueckt */
    if (isSpecialKey)
    {
      switch (key)
      {
      case GLUT_KEY_F1:
        toggleWireframeMode();
        glutPostRedisplay();
        break;
      case GLUT_KEY_F2:
        if (getDebugState() != normals)
        {
          state = normals;
        }
        setDebugState(state);
        glutPostRedisplay();
        break;
      case GLUT_KEY_F3:
        if (getDebugState() != heightmap)
        {
          state = heightmap;
        }
        setDebugState(state);
        glutPostRedisplay();
        break;
        // Togglen des Vollbildmodus
      case GLUT_KEY_F12:
        isFullscreen = !isFullscreen;
        if (isFullscreen)
        {
          glutFullScreen();
        }
        else
        {
          glutReshapeWindow(DEFAULT_WINDOW_SIZE, DEFAULT_WINDOW_SIZE);
        }
        break;
      }
    }

    /* normale Taste gedrueckt */
    else
    {
      effectState state = lights_on;
      switch (key)
      {
      case 'h':
      case 'H':
        printHelp();
        break;
      case 'p':
      case 'P':
        isPaused = !isPaused;
        break;
      case 't':
      case 'T':
        toggleTextures();
        break;
      case 'l':
      case 'L':
        if (getEffectState() == lights_on)
        {
          state = lights_out;
        }
        setEffectState(state);
        break;
      case 'c':
      case 'C':
        if (getEffectState() != cartoon)
        {
          state = cartoon;
        }
        setEffectState(state);
        break;
      case 's':
      case 'S':
        if (getEffectState() != sepia)
        {
          state = sepia;
        }
        setEffectState(state);
        break;
      case 'g':
      case 'G':
        if (getEffectState() != gray)
        {
          state = gray;
        }
        setEffectState(state);
        break;
      /* Programm beenden */
      case 'q':
      case 'Q':
      case ESC:
        exit(0);
        break;
      /* Anhebung erhöhen */
      case '+':
        setElevation(ELEVATION_VALUE);
        break;
      /* Anhebung verringern */
      case '-':
        setElevation(-ELEVATION_VALUE);
        break;
      }
    }
  }
}

/**
 * Callback fuer Tastendruck.
 * Ruft Ereignisbehandlung fuer Tastaturereignis auf.
 * @param key betroffene Taste (In).
 * @param x x-Position der Maus zur Zeit des Tastendrucks (In).
 * @param y y-Position der Maus zur Zeit des Tastendrucks (In).
 */
static void
cbKeyboard(unsigned char key, int x, int y)
{
  handleKeyboardEvent(key, GLUT_DOWN, GL_FALSE, x, y);
}

/**
 * Callback fuer Druck auf Spezialtasten.
 * Ruft Ereignisbehandlung fuer Tastaturereignis auf.
 * @param key betroffene Taste (In).
 * @param x x-Position der Maus zur Zeit des Tastendrucks (In).
 * @param y y-Position der Maus zur Zeit des Tastendrucks (In).
 */
static void
cbSpecial(int key, int x, int y)
{
  handleKeyboardEvent(key, GLUT_DOWN, GL_TRUE, x, y);
}

void registerCallbacks(void)
{
  /* DEBUG-Ausgabe */
  INFO(("Registriere Callbacks...\n"));

  /* Ob die extension ARB_debug_output bereit steht hängt vom Treiber
   * ab. Deswegen muss zur Laufzeit geprüft werden, ob das Callback
   * registriert werden kann. */
  if (GLEW_ARB_debug_output)
  {
    /* Verhindert, dass die OpenGL Implementierung das Callback 
       * asynchron aufruft. Damit ist sichergestellt, dass der Callstack
       * im Falle eines Fehlers bereit steht.  */
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);

    /* In den Rechenzentren ist im Sommersemester 2016 eine veraltete
       * GLEW-Version installiert. Die dort erwartete Funktions-
       * signatur des Callback-Handlers stimmt nicht mit der hier
       * implementierten überein. Wenn ihr (Studenten) auf euren
       * Laptops arbeitet, ist es jedoch durchaus möglich, dass dieser
       * Code fehlerfrei kompiliert. Definiert einfach mal das Makro
       * und testet das. */
#ifdef GLEW_CORRECT
    /* Wenn dieses Callback gesetzt ist, wird die übergebene Funktion
       * immer dann aufgerufen, wenn Fehler auftreten oder weitere
       * nennenswerte Informationen ausgegeben werden müssen. Ihr
       * müsst dann nicht immer glGetError aufrufen und bekommt
       * zusätzlich auch noch einige Zusatzinformationen, die das
       * Debuggen vereinfachen. */
    glDebugMessageCallback(cbDebugOutput, NULL);
#endif

    /* Aktivieren der Extension */
    glEnable(GL_DEBUG_OUTPUT);
  }

  /* Display-Callback (wird ausgefuehrt, wenn neu gezeichnet wird
   * z.B. nach Erzeugen oder Groessenaenderungen des Fensters) */
  glutDisplayFunc(cbDisplay);

  /* Tasten-Druck-Callback - wird ausgefuehrt, wenn eine Taste gedrueckt wird */
  glutKeyboardFunc(cbKeyboard);

  /* Spezialtasten-Druck-Callback - wird ausgefuehrt, wenn Spezialtaste
   * (F1 - F12, Links, Rechts, Oben, Unten, Bild-Auf, Bild-Ab, Pos1, Ende oder
   * Einfuegen) gedrueckt wird */
  glutSpecialFunc(cbSpecial);

  /* Timer-Callback - wird einmalig nach msescs Millisekunden ausgefuehrt */
  glutTimerFunc(1000 / TIMER_CALLS_PS,       /* msecs - bis Aufruf von func */
                cbTimer,                     /* func  - wird aufgerufen    */
                glutGet(GLUT_ELAPSED_TIME)); /* value - Parameter, mit dem
                                                   func aufgerufen wird */

  /* DEBUG-Ausgabe */
  INFO(("...fertig.\n\n"));
}

int createWindow(char *title, int width, int height)
{
  int windowID = 0;

  /* DEBUG-Ausgabe */
  INFO(("Erzeuge Fenster...\n"));

  /* Initialisieren des Fensters */
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
  glutInitWindowSize(width, height);
  glutInitWindowPosition(0, 0);

  /* Es wird explizit ein Context mit der Version 3.3 und dem Kern-
   * Profil angefordert. */
  glutInitContextVersion(3, 3);
  glutInitContextProfile(GLUT_CORE_PROFILE);

  windowID = glutCreateWindow(title);

  /* DEBUG-Ausgabe */
  INFO(("...fertig..\n\n"));

  return windowID;
}