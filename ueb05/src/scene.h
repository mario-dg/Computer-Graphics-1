
#include "types.h"

/**
 * (In-)/Dekrementiert die Elevation
 * 
 * @param elevation; Value
*/
void setElevation(GLdouble elevation);

/**
 * Erhoeht den Timer um den uebergebenen Wert
 * 
 * @param interval; Value
*/
void setTimer(double interval);

/**
 * Bereitet die Szene vor.
 */
void initScene(void);

/**
 * Zeichen-Funktion.
 * Stellt die Szene dar.
 * Ausgabe eines Rechtecks.
 */
void drawScene(void);

/**
 * (De-)aktiviert den Wireframe-Modus.
 */
void toggleWireframeMode(void);

/**
 * Setzt den Debug-Status
 * 
 * @param newState; Zu setzender Debug Status
*/
void setDebugState(debugState newState);

/**
 * Setzt den Effekt-Status
 * 
 * @param newEffect; Zu setzender Effekt
*/
void setEffectState(effectState newEffect);

/**
 * (De-)/Aktiviert die Texturierung
 * 
*/
void toggleTextures(void);

/**
 * Liefert den Debug-State
 * 
 * @return debugState; Debug-Status
*/
debugState getDebugState(void);

/**
 * Liefert den Effekt-State
 * 
 * @return effectState; Effekt
*/
effectState getEffectState(void);
