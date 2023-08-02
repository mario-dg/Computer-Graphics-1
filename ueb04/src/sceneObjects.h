

/**
 * liefert, ob die Normalen angezeigt werden sollen
 * @return true, wenn die Normalen angezeigt werden sollen, sonst false
 */
GLboolean getShowNormal(void);

/**
 * Setzt das Anzeigen der Normalen
 * @param normal true, wenn die Normalen angezeigt werden sollen, sonst false
 */ 
void setShowNormal(GLboolean normal);

/**
 * liefert die Hoehe der Insel in der Mitte
 * @return Hoehe der Insel
 */
GLfloat getIslandHeight(void);

/**
 * Zeichnet eine Kugel in der uebergebenen Farbe
 * @param color Farbe der Kugel
 */
void drawSphere(const CGColor3f color);

/**
 * Berechnet die Punkte auf einem Kreis und speichert sie im circlePoints Array
 */
void calcCylinderPoints(void);

/**
 * Zeichnet ein Zylinder mit der uebergebenen Farbe
 * @param color Farbe des Zylinder
 * @param lowerRadius Radius des unteren Kreises
 * @param upperRadius Radius des oberen Kreises
 * @param heigth Hoehe des Zylinder
 * @param texturing ob der Zylinder texturiert werden soll
 */
void drawCylinder(const CGColor3f color, GLfloat lowerRadius, GLfloat upperRadius, GLfloat height, GLboolean texturing);

/**
 * Zeichnet die Insel in der Mitte
 */
void drawIsland(void);

void drawBoats(void);