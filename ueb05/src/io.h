// Konstante fuer die Standard Fenstergroesse
#define DEFAULT_WINDOW_SIZE 750

/**
 * Erzeugt ein Fenster.
 * @param title Beschriftung des Fensters
 * @param width Breite des Fensters
 * @param height Hoehe des Fensters
 * @return ID des erzeugten Fensters, 0 im Fehlerfall
 */
int createWindow(char *title, int width, int height);

/**
 * Registrierung der GLUT-Callback-Routinen.
 */
void registerCallbacks(void);

/**
 * Initialisierung des OpenGL-Systems.
 * Setzt Shading, Hintergrund- und Zeichenfarbe.
 * @return Rueckgabewert: im Fehlerfall 0, sonst 1.
 */
int initOpenGL(void);