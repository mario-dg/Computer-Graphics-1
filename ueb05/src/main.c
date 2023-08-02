/**
 * @file
 * Einfaches Beispielprogramm fuer OpenGL, GLUT & GLEW
 * sowie einen Shader in GLSL.
 * Vier texturierte Quadrate werden gezeigt.
 * @author copyright (C) Fachhochschule Wedel 1999-2018. All rights reserved.
 */

/* ---- System Header einbinden ---- */
#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>

/* ---- Eigene Header einbinden ---- */
#include "utility.h"
#include "debugGL.h"
#include "scene.h"
#include "types.h"
#include "io.h"

/**
 * Hauptprogramm.
 * Initialisiert Fenster, Anwendung und Callbacks, startet glutMainLoop.
 * @param argc Anzahl der Kommandozeilenparameter (In).
 * @param argv Kommandozeilenparameter (In).
 * @return Rueckgabewert im Fehlerfall ungleich Null.
 */
int main(int argc, char **argv)
{
  GLenum error;

  /* Glut initialisieren */
  glutInit(&argc, argv);

  /* Erzeugen des Fensters */
  if (!createWindow("Shader", DEFAULT_WINDOW_SIZE, DEFAULT_WINDOW_SIZE))
  {
    fprintf(stderr, "Initialisierung des Fensters fehlgeschlagen!");
    exit(1);
  }

  /* Initialisieren von GLEW.
   * Wenn glewExperimental auf GL_TRUE gesetzt wird, läd GLEW
   * zusätzliche Extensions. In diesem konkreten Programm wird dies
   * beispielsweise benötigt, um glGenVertexArrays() aufzurufen. (Ohne
   * glewExperimental auf GL_TRUE zu setzen tritt ein Speicherzugriffs-
   * fehler auf.) */
  glewExperimental = GL_TRUE;
  error = glewInit();
  if (error != GLEW_OK)
  {
    fprintf(stderr, "Initialisierung von GLEW fehlgeschlagen!");
    exit(1);
  }

  /* Initialen OpenGL-Zustand setzen */
  if (!initOpenGL())
  {
    fprintf(stderr, "Initialisierung von OpenGL fehlgeschlagen!");
    exit(1);
  }

  /* Initialisiere Szene */
  initScene();

  /* registriere Callbacks */
  registerCallbacks();

  /* Hauptschleife der Ereignisbehandlung starten */
  glutMainLoop();

  return 0;
}
