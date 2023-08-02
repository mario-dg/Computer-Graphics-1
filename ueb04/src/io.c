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

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

/* ---- Eigene Header einbinden ---- */
#include "io.h"
#include "scene.h"
#include "debugGL.h"
#include "stringOutput.h"
#include "logic.h"
#include "texture.h"
#include <math.h>



/* ---- Konstanten ---- */
#define MAX_AMOUNT_VERTICES (100)
#define MIN_AMOUNT_VERTICES (2)

/** Anzahl der Aufrufe der Timer-Funktion pro Sekunde */
#define TIMER_CALLS_PS 360

//Makros fuer die Kamerabewegung
#define CAMERA_MOVEMENT_STEPS 2.0f
#define CAMERA_RADIUS_STEPS 5.0f
#define MIN_THETA 0.001f
#define MAX_THETA M_PI_2
#define MIN_RADIUS 1.0f
#define MAX_RADIUS 75.0f

//Makros fuer die Projektion
#define NEAR_CLIPPING_PLANE 0.1f
#define FAR_CLIPPING_PLANE 500.0f

/** Anzahl Simulationsschritte pro Sekunde*/
#define SIM_STEPS_PS (800)

/* ---- Globale Daten ---- */
double interval = 0.0f;
double lastIdleCallTime = 0.0f;

//Status fuer die rotierende Lichtquelle des Leuchtturms
int last1State = rotating;
//Welches Boot derzeit angeleuchtet werden soll
int lastClickedBoat = boat1;

GLboolean isFullscreen = GL_FALSE;
GLboolean isPaused = GL_FALSE;
GLboolean pickSpheres = GL_TRUE;
/* ---- Funktionen ---- */

/**
 * Berechnung der Frames pro Sekunde.
 * @return aktuelle Framerate.
 */
static float
frameRate(void)
{
  /* Zeitpunkt der letzten Neuberechnung */
  static int timebase = 0;
  /* Anzahl der Aufrufe seit letzter Neuberechnung */
  static int frameCount = 0;
  /* Zuletzt berechneter FPS-Wert */
  static float fps = 0.0;
  /* aktuelle "Zeit" */
  static int time = 0;

  /* Diesen Aufruf hinzuzaehlen */
  frameCount++;

  /* seit dem Start von GLUT vergangene Zeit ermitteln */
  time = glutGet(GLUT_ELAPSED_TIME);

  /* Eine Sekunde ist vergangen */
  if (time - timebase > 1000)
  {
    /* FPS-Wert neu berechnen */
    fps = frameCount * 1000.0f / (time - timebase);

    /* Zureuecksetzen */
    timebase = time;
    frameCount = 0;
  }

  /* Aktuellen FPS-Wert zurueckgeben */
  return fps;
}

/**
 * bestimmt die Anzahl an Stellen einer Zahl
 * @param num Zahl, dessen Laenge bestimmt wird
 * @return Anzahl Stellen der Zahl
 */
GLint calcNumLength(GLint num)
{
  GLint len = num == 0 ? 1 : 0; //Wenn num 0 ist Länge gleich 1
  while (num)
  {
    len++;
    num /= 10; //Zehnerstellen ermitteln
  }
  return len;
}

/**
 * Verarbeitung der Picking-Ergebnisse.
 * Findet den Treffer, der dem Betrachter am naechsten liegt und gibt die
 * Namen dieses Treffers aus.
 * @param numHits Anzahl der getroffenen Objekte (In).
 * @param buffer Buffer, in dem die Treffer gespeichert sind (In).
 */
static void
processHits(GLint numHits, GLuint buffer[], mouseButtons click)
{
  /* Anzahl der Namen des Treffers, der am naechsten zum Betrachter ist */
  GLint numOfClosestNames = 0;
  /* Anzahl der Namen des Treffers, der am naechsten zum Betrachter ist */
  GLuint *ptrClosestNames = NULL;

  /* Laufvariable - Nummer des akt. verarbeiteten Treffers */
  GLint i;
  /* Laufvariable - Zeiger auf den Beginn des Hit-Records */
  GLuint *ptr = (GLuint *)buffer;

  /* DepthBuffer - Wert */
  GLuint minZ = 0xffffffff;

  if (numHits > 0)
  {
    /* Alle Treffer verarbeiten */
    for (i = 0; i < numHits; i++)
    {
      /* Ist der Treffer naeher dran, als der bisher naechste? */
      if (*(ptr + 1) < minZ)
      {
        /* Anzahl der Namen merken */
        numOfClosestNames = *ptr;
        /* Zeiger auf den ersten Namen merken */
        ptrClosestNames = ptr + 3;
        /* als neuen minimalen Tiefenwert merken */
        minZ = *(ptr + 1);
      }
      /* zum Beginn des naechsten Hit-Records springen */
      /* Schrittweite: Anzahl-Feld + minZ-Feld + maxZ-Feld + Namen-Felder */
      ptr += 3 + *ptr;
    }

    /* Alle Namen des Treffers ausgeben, der am naechsten zum Betrachter liegt */
    for (i = 0; i < numOfClosestNames; i++, ptrClosestNames++)
    {
      if (pickSpheres)
      {
        pickedVertex(*ptrClosestNames, click);
      }
      else
      {
        lastClickedBoat = *ptrClosestNames;
        setLight1State(lastClickedBoat);
      }
    }
  }
}

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
  /* perspektivische Projektion */
  gluPerspective(70.0,                /* Oeffnungswinkel */
                 aspect,              /* Seitenverhaeltnis */
                 NEAR_CLIPPING_PLANE, /* nahe Clipping-Ebene */
                 FAR_CLIPPING_PLANE /* ferne Clipping-Ebene */);
}

/**
 * Setzen der Kameraposition.
 */
static void
setCamera(void)
{
  GLfloat cameraX = getCameraRadius() * sin(getCameraTheta()) * cos(getCameraPhi());
  GLfloat cameraY = getCameraRadius() * cos(getCameraTheta());
  GLfloat cameraZ = getCameraRadius() * sin(getCameraTheta()) * sin(getCameraPhi());
  /* Kameraposition */
  gluLookAt(cameraX, cameraY, cameraZ, /* Augpunkt */
            0.0, 0.0, 0.0,             /* Zentrum */
            0.0, 1.0, 0.0);            /* Up-Vektor */
}

/**
 * Picking. Auswahl von Szenenobjekten durch Klicken mit der Maus.
 * @param click mit welcher Maustaste gepickt wurde
 */
static void
pick(int x, int y, mouseButtons click)
{
  /** Groesse des Buffers fuer Picking Ergebnisse */
#define SELECTBUFSIZE 512

  /* Viewport (Darstellungsbereich des Fensters) */
  GLint viewport[4];

  /* Puffer fuer Picking-Ergebnis */
  GLuint buffer[SELECTBUFSIZE];

  /* Anzahl der getroffenen Namen beim Picking */
  GLint numHits;

  /* aktuellen Viewport ermitteln */
  glGetIntegerv(GL_VIEWPORT, viewport);

  /* Puffer festlegen, Name-Stack leeren und Rendermode wechseln */
  glSelectBuffer(SELECTBUFSIZE, buffer);
  glInitNames();
  glRenderMode(GL_SELECT);

  /* Nachfolgende Operationen beeinflussen Projektionsmatrix */
  glMatrixMode(GL_PROJECTION);
  /* Matrix zuruecksetzen - Einheitsmatrix laden */
  glLoadIdentity();

  gluPickMatrix(x, glutGet(GLUT_WINDOW_HEIGHT) - y, 5, 5, viewport);

  setProjection((double)glutGet(GLUT_WINDOW_WIDTH) /
                (double)glutGet(GLUT_WINDOW_HEIGHT));

  /* Nachfolgende Operationen beeinflussen Modelviewmatrix */
  glMatrixMode(GL_MODELVIEW);
  /* Matrix zuruecksetzen - Einheitsmatrix laden */
  glLoadIdentity();

  setCamera();

  /* Zeichnen nur der pickbaren Objekte */
  drawScenePickable(pickSpheres);

  /* Zeichnen beenden und auswerten */
  glFlush();

  /* Nachfolgende Operationen beeinflussen Projektionsmatrix */
  glMatrixMode(GL_PROJECTION);
  /* Matrix zuruecksetzen - Einheitsmatrix laden */
  glLoadIdentity();

  setProjection((double)glutGet(GLUT_WINDOW_WIDTH) /
                (double)glutGet(GLUT_WINDOW_HEIGHT));

  numHits = glRenderMode(GL_RENDER);

  /*   GLGETERROR; */

  //Verarbeiten der Treffer
  processHits(numHits, buffer, click);
}

/**
 * Verarbeitung eines Tasturereignisses.
 * ESC-Taste und q, Q beenden das Programm.
 * @param key Taste, die das Ereignis ausgeloest hat. (ASCII-Wert oder WERT des
 *        GLUT_KEY_<SPECIAL>.
 * @param status Status der Taste, GL_TRUE=gedrueckt, GL_FALSE=losgelassen.
 * @param isSpecialKey ist die Taste eine Spezialtaste?
 * @param x x-Position des Mauszeigers zum Zeitpunkt der Ereignisausloesung.
 * @param y y-Position des Mauszeigers zum Zeitpunkt der Ereignisausloesung.
 */
static void
handleKeyboardEvent(int key, int status, GLboolean isSpecialKey, int x,
                    int y)
{
  (void)x;
  (void)y;

  /* temporaere Variablen fuer Zustaende */
  int state = 0;
  GLboolean boolState = GL_TRUE;

  /** Keycode der ESC-Taste */
#define ESC 27

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
        /* Anzeigen der Normalen umschalten */
      case GLUT_KEY_F2:
        toggleNormal();
        glutPostRedisplay();
        break;
        /* Aktivierung der Lichtberechnung umschalten */
      case GLUT_KEY_F3:
        boolState = !getLightingState();
        setLightingState(boolState);
        glutPostRedisplay();
        break;
        /* Punktlichtquelle umschalten */
      case GLUT_KEY_F4:
        boolState = !getLight0State();
        setLight0State(boolState);
        glutPostRedisplay();
        break;
        /* (rotierendes) Spotlight (De-)Aktivieren */
      case GLUT_KEY_F5:
        state = getLight1State();
        if (state == off)
        {
          state = last1State;
        }
        else
        {
          state = off;
        }
        setLight1State(state);
        glutPostRedisplay();
        break;
        /* Umschalten zwischen Picken der Kugeln und Picken der Boote */
      case GLUT_KEY_F6:
        pickSpheres = !pickSpheres;
        if (last1State == rotating)
        {
          last1State = lastClickedBoat;
        }
        else
        {
          last1State = rotating;
        }
        setLight1State(last1State);
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
          glutReshapeWindow(DEFAULT_WINDOW_WIDTH, DEFAULT_WINDOW_HEIGHT);
        }
        break;
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
        freeAllocatedMem();
        freeAllocatedMemLogic();
        exit(0);
        break;
        /* naeher ranzoomen */
      case 'u':
      case 'U':
        if (getCameraRadius() > MIN_RADIUS)
        {
          setCameraRadius(getCameraRadius() - (CAMERA_RADIUS_STEPS * interval));
          if (getCameraRadius() < MIN_RADIUS)
          {
            setCameraRadius(MIN_RADIUS);
          }
        }
        break;
        /* weiter rauszoomen */
      case 'o':
      case 'O':
        if (getCameraRadius() < MAX_RADIUS)
        {
          setCameraRadius(getCameraRadius() + (CAMERA_RADIUS_STEPS * interval));
          if (getCameraRadius() > MAX_RADIUS)
          {
            setCameraRadius(MAX_RADIUS);
          }
        }
        break;
        /* Kamera bewegen nach oben*/
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
        /* Kamera bewegen nach links */
      case 'j':
      case 'J':
        setCameraPhi(getCameraPhi() + (CAMERA_MOVEMENT_STEPS * interval));
        break;
        /* Kamera bwegen nach unten */
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
        /* Kamera bewegen nach rechts */
      case 'l':
      case 'L':
        setCameraPhi(getCameraPhi() - (CAMERA_MOVEMENT_STEPS * interval));
        break;
        /* Punte hinzufügen */
      case '+':
        if (getAmountVertices() < MAX_AMOUNT_VERTICES)
        {
          updateLogic(expand);
          updateVertexArray(expand);
        }
        break;
        /* Punkte entfernen */
      case '-':
        if (getAmountVertices() > MIN_AMOUNT_VERTICES)
        {
          updateLogic(shrink);
          updateVertexArray(shrink);
        }
        break;
        /* Anzeigen der Texturen umschalten */
      case 't':
      case 'T':
        state = !getTexturingStatus();
        setTexturingStatus(state);
        glutPostRedisplay();
        break;
        /* Anzeigen der Kugeln umschalten */
      case 's':
      case 'S':
        toggleSpheres();
        break;
        /* Simulation pausieren*/
      case 'p':
      case 'P':
        isPaused = !isPaused;
        break;

      case 'h':
      case 'H':
        toggleShowHelp();
        break;
      }
    }
  }
}

/**
 * Verarbeitung eines Mausereignisses.
 * Durch Bewegung der Maus bei gedrueckter Maustaste kann die aktuelle
 * Zeichenfarbe beeinflusst werden.
 * Falls Debugging aktiviert ist, wird jedes Mausereignis auf stdout
 * ausgegeben.
 * @param x x-Position des Mauszeigers zum Zeitpunkt der Ereignisausloesung.
 * @param y y-Position des Mauszeigers zum Zeitpunkt der Ereignisausloesung.
 * @param eventType Typ des Ereignisses.
 * @param button ausloesende Maustaste (nur bei Ereignissen vom Typ mouseButton).
 * @param buttonState Status der Maustaste (nur bei Ereignissen vom Typ mouseButton).
 */
static void
handleMouseEvent(int x, int y, CGMouseEventType eventType, int button,
                 int buttonState)
{
  switch (eventType)
  {
  case mouseButton:
    switch (button)
    {
    case GLUT_LEFT_BUTTON:
      if (buttonState == GLUT_UP)
      {
        pick(x, y, leftClick);
      }
      break;
    case GLUT_RIGHT_BUTTON:
      if (buttonState == GLUT_UP)
      {
        pick(x, y, rightClick);
      }
      break;
    default:
      break;
    }
    break;
  default:
    break;
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

/**
 * Mouse-Button-Callback.
 * @param button Taste, die den Callback ausgeloest hat.
 * @param state Status der Taste, die den Callback ausgeloest hat.
 * @param x X-Position des Mauszeigers beim Ausloesen des Callbacks.
 * @param y Y-Position des Mauszeigers beim Ausloesen des Callbacks.
 */
static void
cbMouseButton(int button, int state, int x, int y)
{
  handleMouseEvent(x, y, mouseButton, button, state);
}

/**
 * Callback fuer Aenderungen der Fenstergroesse.
 * Initiiert Anpassung der Projektionsmatrix an veränderte Fenstergroesse.
 * @param w Fensterbreite (In).
 * @param h Fensterhoehe (In).
 */
static void
cbReshape(int w, int h)
{
  /* Das ganze Fenster ist GL-Anzeigebereich */
  glViewport(0, 0, (GLsizei)w, (GLsizei)h);

  /* Nachfolgende Operationen beeinflussen Projektionsmatrix */
  glMatrixMode(GL_PROJECTION);
  /* Matrix zuruecksetzen - Einheitsmatrix laden */
  glLoadIdentity();

  /* Anpassen der Projektionsmatrix an das Seitenverhältnis des Fensters */
  setProjection((GLdouble)w / (GLdouble)h);
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
set3DViewport(GLint x, GLint y, GLint width, GLint height)
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
  gluPerspective(70,                  /* oeffnungswinkel */
                 aspect,              /* Seitenverhaeltnis */
                 NEAR_CLIPPING_PLANE, /* nahe Clipping-Ebene */
                 FAR_CLIPPING_PLANE); /* ferne Clipping-Ebene */

  /* Folge Operationen beeinflussen die Modelviewmatrix */
  glMatrixMode(GL_MODELVIEW);

  /* Einheitsmatrix laden */
  glLoadIdentity();

  setCamera();

  /* Szene zeichnen lassen */
  drawScene();
}

/**
 * Zeichen-Callback.
 * Loescht die Buffer, ruft das Zeichnen der Szene auf und tauscht den Front-
 * und Backbuffer.
 */
static void
cbDisplay(void)
{
  /* Frames pro Sekunde */
  static GLfloat fps = 0.0f;

  // Wir gehen davon aus, dass die FPS nie groesser als 99999.99 werden
  char fpsOutputBuffer[13];
  /* Fensterdimensionen auslesen */
  int width = glutGet(GLUT_WINDOW_WIDTH);
  int height = glutGet(GLUT_WINDOW_HEIGHT);

  /* Framewbuffer und z-Buffer zuruecksetzen */
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  /* 3D-Viewports setzen */
  set3DViewport(0, 0, /* x, y */
                width, height);

  glutSwapBuffers(); /* fuer DoubleBuffering */

  if (sprintf(fpsOutputBuffer, "FPS: %.2f", fps) < 0)
  {
    fpsOutputBuffer[0] = 'F';
    fpsOutputBuffer[1] = 'E';
    fpsOutputBuffer[2] = 'H';
    fpsOutputBuffer[3] = 'L';
    fpsOutputBuffer[4] = 'E';
    fpsOutputBuffer[5] = 'R';
    fpsOutputBuffer[6] = '\0';
  }
  /* Framerate ausgegeben */
  glutSetWindowTitle(fpsOutputBuffer);
  /* Framerate berechnen */
  fps = frameRate();
}

/**
 * Controller fuer die Simulation des Wassers kontrolliert den Aufruf der Logik
 * @param interval Zeitintervall
 */
void simulationController(double interval)
{
  /* der accumulator sammelt die Zeitintervalle, bis diese insgesamt groesser sind als eine Simulationszeit
     (die durch das Makro SIM_STEPS_PS festgelegt wird). Die Simulation erfolgt dann immer in gelich grossen Zeitstuecken*/
  static double accumulator;
  accumulator += interval;
  while (accumulator >= (1.0f / SIM_STEPS_PS))
  {

    simulateWater(1.0f / SIM_STEPS_PS);

    accumulator -= (1.0f / SIM_STEPS_PS);
  }
}

/**
 * Idle-Callback.
 * Stoesst Neuzeichnen an.
 */
static void
cbIdle(void)
{

  /* Seit dem Programmstart vergangene Zeit in Millisekunden */
  int thisCallTime = glutGet(GLUT_ELAPSED_TIME);

  /* Seit dem letzten Funktionsaufruf vergangene Zeit in Sekunden */
  double IdleInterval = (double)(thisCallTime - lastIdleCallTime) / 1000.0f;
  if (!isPaused)
  {
    simulationController(IdleInterval);
  }
  lastIdleCallTime = thisCallTime;
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
  if (!isPaused)
  {
    simulationController(interval);
  }

  updateNormals();

  calcLight1Rotation(interval);

  /* Wieder als Timer-Funktion registrieren */
  glutTimerFunc(1000 / TIMER_CALLS_PS, cbTimer, thisCallTime);

  /* Neuzeichnen anstossen */
  glutPostRedisplay();
}

/**
 * Registrierung der GLUT-Callback-Routinen.
 */
static void
registerCallbacks(void)
{
  /* Mouse-Button-Callback (wird ausgefuehrt, wenn eine Maustaste
   * gedrueckt oder losgelassen wird) */
  glutMouseFunc(cbMouseButton);

  /* Idle-Callback (wird ausgefuehrt, wenn das System nichts zu tun hat
   * (es ist idle) - ACHTUNG: Aufruffrequenz variiert!! ) */
  glutIdleFunc(cbIdle);

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

  /* DEBUG-Ausgabe */
  INFO(("Erzeuge Fenster...\n"));

  /* Initialisieren des Fensters */
  /* RGB-Framewbuffer, Double-Buffering und z-Buffer anfordern */
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
  glutInitWindowSize(width, height);
  glutInitWindowPosition(0, 0);

  /* Fenster erzeugen */
  windowID = glutCreateWindow(title);

  if (windowID)
  {

    /* DEBUG-Ausgabe */
    INFO(("...fertig.\n\n"));

    /* DEBUG-Ausgabe */
    INFO(("Initialisiere Szene...\n"));

    initLogic();
    if (initScene())
    {
      /* DEBUG-Ausgabe */
      INFO(("...fertig.\n\n"));

      if (initTextures())
      {
        /* DEBUG-Ausgabe */
        INFO(("Registriere Callbacks...\n"));

        registerCallbacks();

        /* DEBUG-Ausgabe */
        INFO(("...fertig.\n\n"));

        /* DEBUG-Ausgabe */
        INFO(("Trete in Schleife der Ereignisbehandlung ein...\n"));

        glutMainLoop();
      }
      else
      {
        /* DEBUG-Ausgabe */
        INFO(("...fehlgeschlagen.\n\n"));

        glutDestroyWindow(windowID);
        windowID = 0;
      }

      /* DEBUG-Ausgabe */
      INFO(("Trete in Schleife der Ereignisbehandlung ein...\n"));

      glutMainLoop();
    }
    else
    {
      /* DEBUG-Ausgabe */
      INFO(("...fehlgeschlagen.\n\n"));

      glutDestroyWindow(windowID);
      windowID = 0;
    }
  }
  else
  {
    /* DEBUG-Ausgabe */
    INFO(("...fehlgeschlagen.\n\n"));
  }

  return windowID;
}
