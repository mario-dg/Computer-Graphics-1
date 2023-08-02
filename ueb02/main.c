/* ---- System Header einbinden ---- */
#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>

/* ---- Eigene Header einbinden ---- */
#include "io.h"
#include "types.h"

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

/**
 * Hauptprogramm.
 * Initialisiert Fenster, Anwendung und Callbacks, startet glutMainLoop.
 * @param argc Anzahl der Kommandozeilenparameter (In).
 * @param argv Kommandozeilenparameter (In).
 * @return Rueckgabewert im Fehlerfall ungleich Null.
 */
int main(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    /* Initialisierung des I/O-Sytems
     (inkl. Erzeugung des Fensters und Starten der Ereignisbehandlung). */
    if (!initAndStartIO("ueb01", DEFAULT_WINDOW_WIDTH, DEFAULT_WINDOW_HEIGHT))
    {
        fprintf(stderr, "Initialisierung fehlgeschlagen!\n");
        return 1;
    }
    else
    {
        return 0;
    }
}