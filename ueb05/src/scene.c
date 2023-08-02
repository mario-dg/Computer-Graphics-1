/**
 * @file
 * Darstellungs-Modul.
 * Das Modul kapselt die Rendering-Funktionalitaet (insbesondere der OpenGL-
 * Aufrufe) des Programms.
 *
 * Bestandteil eines Beispielprogramms fuer Textausgabe mit OpenGL & GLUT.
 *
 * @author copyright (C) Fachhochschule Wedel 1999-2011. All rights reserved.
 */

/* ---- System Header einbinden ---- */
#ifdef WIN32
#include <windows.h>
#endif
#include "utility.h"

#include "scene.h"
#ifdef __APPLE__
#include <OpenGL/gl.h>
#include <GLUT/glut.h>
#else
#include <GL/gl.h>
#endif

/* Bibliothek um Bilddateien zu laden. Es handelt sich um eine
 * Bibliothek, die sowohl den Header als auch die Quelle in einer Datei
 * bereitstellt. Die Quelle kann durch die Definition des Makros
 * STB_IMAGE_IMPLEMENTATION eingebunden werden. Genauere Informationen
 * sind der "stb_image.h" zu entnehmen.
 * 
 * Quelle: https://github.com/nothings/stb */
// define global functions as static inline
#define STB_IMAGE_STATIC
#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h> // Bibliothek um Bilder zu laden
                           // Quelle: https://github.com/nothings/stb

/** Makro, um das Quadrat einer Zahl zu bestimmen*/
#define SQUARE(x) ((x) * (x))
/** Anzahl der Vertices pro Seite, aus denen das Mesh erzeugt wird */
#define MESH_AMOUNT_VERTICES (314)
/** Anzahl an Indizes die zum Zeichnen benoetigt werden */
#define AMOUNT_INDICES (SQUARE(MESH_AMOUNT_VERTICES - 1) * 2 * 3)

/** Buffer-Objekt, um die Vertizes zu speichern. */
static GLuint g_arrayBuffer;

/** Vertex-Array-Objekt, um die Attribut-Pointer zu speichern. */
static GLuint g_vertexArrayObject;

/** Programm-Objekt zum Rendern */
static GLuint g_program;

/** Anhebung der Vertizes */
static float g_elevation;

/** Texturen */
static GLuint g_HeightTexture;
static GLuint g_WorldTexture;

/** Location der uniform-Variable "ModelView" */
static GLint g_locationModelViewMatrix;

/** Location der uniform-Variable "Elevation" */
static GLint g_locationElevation;

/** Location der uniform-Variablen "Texturen" */
static GLuint g_locationHeightTexture;
static GLuint g_locationWorldTexture;

static GLuint g_locationEnableTexture;
static GLuint g_locationCurrEffect;
static GLuint g_locationLightPos;
static GLuint g_locationAmountVertices;
static GLuint g_locationDebugState;

// Index-Array
static GLint g_indexArray[AMOUNT_INDICES];
// Vertex-Array mit allen benoetigten Infos zu allen Vertices
static Vertex g_vertices[AMOUNT_INDICES];
// Zwischenspeicher
static Vertex helper[SQUARE(MESH_AMOUNT_VERTICES)];

//Statusvariablen
static effectState currEffect;
static debugState currDebugState;
static GLint showTextures;
static GLdouble g_timer = 0.0f;

/**
 * Fuellt das Index Array
 * 
*/
static void fillIndexArray(void)
{
    GLint i = 0;
    GLint countUpperTri = 0;
    GLint countLowerTri = 0;
    GLint rowCount = -1;
    GLboolean upperTri = GL_FALSE;
    for (i = 0; i < AMOUNT_INDICES; i += 3)
    {
        //Jede Reihe besteht aus einer Reihe normaler Dreiecke und einer Reihe die auf dem Kopf steht
        if (upperTri && (countUpperTri % (MESH_AMOUNT_VERTICES - 1) == 0))
        {
            upperTri = GL_FALSE;
        }
        else if (!upperTri && (countLowerTri % (MESH_AMOUNT_VERTICES - 1) == 0))
        {
            upperTri = GL_TRUE;
            rowCount++;
        }

        if (upperTri)
        {
            g_indexArray[i] = countUpperTri + rowCount;
            g_indexArray[i + 1] = countUpperTri + MESH_AMOUNT_VERTICES + rowCount;
            g_indexArray[i + 2] = countUpperTri + 1 + rowCount;
            countUpperTri++;
        }
        else
        {
            g_indexArray[i] = countLowerTri + 1 + rowCount;
            g_indexArray[i + 1] = countLowerTri + MESH_AMOUNT_VERTICES + rowCount;
            g_indexArray[i + 2] = countLowerTri + MESH_AMOUNT_VERTICES + 1 + rowCount;
            countLowerTri++;
        }
    }
}

/**
 * Fuellt das Zwischenstands-Array aus dem spaeter das Vertex Array zusammenggesetzt wird
 * Enthaelt jeden Punkt genau einmal
*/
static void fillHelperArray(void)
{
    GLint i = 0;
    GLfloat currX = -1.0f;
    GLfloat currZ = -1.0f - (2.0f / (MESH_AMOUNT_VERTICES - 1));
    GLfloat currTexS = 0.0f;
    GLfloat currTexT = 1.0f + 1.0f / (MESH_AMOUNT_VERTICES - 1);

    for (i = 0; i < SQUARE(MESH_AMOUNT_VERTICES); i++)
    {
        if (i % MESH_AMOUNT_VERTICES == 0)
        {
            currX = -1.0f;
            currZ += 2.0f / (MESH_AMOUNT_VERTICES - 1);
            currTexS = 0.0f;
            currTexT -= 1.0f / (MESH_AMOUNT_VERTICES - 1);
        }
        else
        {
            currX += 2.0f / (MESH_AMOUNT_VERTICES - 1);
            currTexS += 1.0f / (MESH_AMOUNT_VERTICES - 1);
        }

        helper[i].x = currX;
        helper[i].y = 0.0f;
        helper[i].z = currZ;
        helper[i].s = currTexS;
        helper[i].t = currTexT;
        helper[i].nx = 0.0f;
        helper[i].ny = 1.0f;
        helper[i].nz = 0.0f;
    }
}

/**
 * Alle benoetigten Vertices, um das Mesh zu konstruieren
 * Enthaelt Punkte auch mehrfach
 * 
*/
static void fillVerticesArray(void)
{
    GLint i = 0;
    for (i = 0; i < AMOUNT_INDICES; i++)
    {
        g_vertices[i] = helper[g_indexArray[i]];
    }
}

void initScene(void)
{
    // Default Werte
    g_elevation = 0.1f;
    showTextures = 1;
    currDebugState = no_debug;
    currEffect = lights_on;
    g_timer = 0;
    {
        /* Laden der Textur. */
        int width, height, comp;
        GLubyte *heightMap = stbi_load("../content/textures/SS20_heightmap.jpg", &width, &height, &comp, 1);
        /* Erstellen des Textur-Objekts. */
        glGenTextures(1, &g_HeightTexture);
        glBindTexture(GL_TEXTURE_2D, g_HeightTexture);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, heightMap);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

        glBindTexture(GL_TEXTURE_2D, 0);
        stbi_image_free(heightMap);
    }

    {

        /* Laden der Textur. */
        int width, height, comp;
        GLubyte *worldMap = stbi_load("../content/textures/SS20_worldmap.png", &width, &height, &comp, 4);
        /* Erstellen des Textur-Objekts. */
        glGenTextures(1, &g_WorldTexture);
        glBindTexture(GL_TEXTURE_2D, g_WorldTexture);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, worldMap);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

        glBindTexture(GL_TEXTURE_2D, 0);
        stbi_image_free(worldMap);
    }

    fillIndexArray();
    fillHelperArray();
    fillVerticesArray();

    {
        /* Erstellen eines Buffer-Objektes. 
     * In modernem OpenGL werden alle Vertex-Daten in Buffer-Objekten 
     * gespeichert. Dabei handelt es sich um Speicherbereiche die von 
     * der OpenGL-Implementierung verwaltet werden und typischerweise 
     * auf der Grafikkarte angelegt werden.
     * 
     * Mit der Funktion glGenBuffers können Namen für Buffer-Objekte
     * erzeugt werden. Mit glBindBuffer werden diese anschließend 
     * erzeugt, ohne jedoch einen Speicherbereich für die Nutzdaten 
     * anzulegen. Dies geschieht nachfolgend mit einem Aufruf der 
     * Funktion glBufferData.
     * 
     * Um mit Buffer-Objekten zu arbeiten, mussen diese an Targets
     * gebunden werden. Hier wird das Target GL_ARRAY_BUFFER verwendet.
     * 
     * Der OpenGL-Implementierung wird zusätzlich ein Hinweis mitgegeben,
     * wie die Daten eingesetzt werden. Hier wird GL_STATIC_DRAW
     * übergeben. OpenGL kann diesen Hinweis nutzen, um Optimierungen
     * vorzunehmen. */
        glGenBuffers(1, &g_arrayBuffer);
        glBindBuffer(GL_ARRAY_BUFFER, g_arrayBuffer);
        glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertices), g_vertices, GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    {
        const GLuint positionLocation = 0;
        const GLuint texCoordLocation = 1;
        const GLuint normalLocation = 2;
        /* Erstellen eines Vertex-Array-Objektes (VAO).
     * Damit die OpenGL-Implementierung weiß welche Daten der Pipeline
     * bereitgestellt werden müssen, werden Attribut-Pointer gesetzt.
     * Um mehrere Datenquellen (unterschiedliche Meshes) zu verwalten
     * können die Attribut-Pointer in VAOs gruppiert werden. 
     * 
     * Die Erzeugung von VAOs geschieht prinzipiell genauso wie bei
     * Buffern oder anderen OpenGL-Objekten. */
        glGenVertexArrays(1, &g_vertexArrayObject);
        glBindVertexArray(g_vertexArrayObject);

        /* Die Pointer werden immer in den Buffer gesetzt, der am
     * GL_ARRAY_BUFFER-Target gebunden ist! */
        glBindBuffer(GL_ARRAY_BUFFER, g_arrayBuffer);

        /* Im Vertex-Shader existieren folgende Zeilen:
     * > layout (location = 0) in vec4 vPosition;
     * > layout (location = 1) in vec2 vTexCoord;
     * 
     * Beim Aufruf einen Draw-Command, müssen diesen beiden Attributen
     * Daten bereitgestellt werden. Diese sollen aus dem oben erstellten
     * Buffer gelesen werden. Dafür müssen zwei Attribut-Pointer aktiviert
     * und eingerichtet werden. */
        glEnableVertexAttribArray(positionLocation);
        glVertexAttribPointer(
            positionLocation,             /* location (siehe Shader) */
            3,                            /* Dimensionalität */
            GL_FLOAT,                     /* Datentyp im Buffer */
            GL_FALSE,                     /* Keine Normierung notwendig */
            sizeof(Vertex),               /* Offset zum nächsten Vertex */
            (void *)offsetof(Vertex, x)); /* Offset zum ersten Vertex */

        /* Zweiter Attribut-Pointer für die Textur-Koordinate */
        glEnableVertexAttribArray(texCoordLocation);
        glVertexAttribPointer(texCoordLocation, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, s));

        /* Zweiter Attribut-Pointer für die Normalen-Koordinate */
        glEnableVertexAttribArray(normalLocation);
        glVertexAttribPointer(normalLocation, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, nx));

        /* Um Programmierfehler leichter zu finden, sollte der OpenGL-
     * Zustand wieder zurückgesetzt werden. Wird beispielweise das Binden
     * eines Vertex-Array-Objekts vergessen werden, arbeitet OpenGL
     * auf dem vorher gebundenen. Vor allem bei starker Modularisierung
     * sind diese Fehler schwer zu finden. */
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }

    {
        /* Erstellen des Programms */
        g_program = createProgram("../content/shaders/color.vert", "../content/shaders/color.frag");

        /* Abfragen der uniform-locations.
     * Dies kann auch zur Laufzeit gemacht werden, es ist jedoch 
     * performanter dies vorab zu tun, da String-Vergleiche gemacht
     * werden müssen. */
        g_locationModelViewMatrix = glGetUniformLocation(g_program, "ModelView");
        g_locationElevation = glGetUniformLocation(g_program, "Elevation");
        g_locationHeightTexture = glGetUniformLocation(g_program, "heightTexture");
        g_locationWorldTexture = glGetUniformLocation(g_program, "worldTexture");
        g_locationEnableTexture = glGetUniformLocation(g_program, "enableTexture");
        g_locationCurrEffect = glGetUniformLocation(g_program, "currEffect");
        g_locationLightPos = glGetUniformLocation(g_program, "lightPos");
        g_locationAmountVertices = glGetUniformLocation(g_program, "amountVertices");
        g_locationDebugState = glGetUniformLocation(g_program, "debugState");

        /* DEBUG-Ausgabe */
        fprintf(stderr, "ModelView hat 'location': %i\n", g_locationModelViewMatrix);
        fprintf(stderr, "Elevation hat 'location': %i\n", g_locationElevation);
        fprintf(stderr, "heightTexture hat 'location': %i\n", g_locationHeightTexture);
        fprintf(stderr, "worldTexture hat 'location': %i\n", g_locationWorldTexture);
        fprintf(stderr, "enableTexture hat 'location': %i\n", g_locationEnableTexture);
        fprintf(stderr, "currEffect hat 'location': %i\n", g_locationCurrEffect);
        fprintf(stderr, "lightPos hat 'location': %i\n", g_locationLightPos);
        fprintf(stderr, "debugState hat 'location': %i\n", g_locationDebugState);
    }

    {
        /* Erstellen der Projektions-Matrix.
     * Da die Projektions-Matrix für die Laufzeit des Programms konstant
     * ist, wird sie einmalig gesetzt. */
        float projectionMatrix[16];
        perspective(60, (float)glutGet(GLUT_WINDOW_WIDTH) / glutGet(GLUT_WINDOW_HEIGHT), 0.01f, 100.0f, projectionMatrix);
        glUseProgram(g_program);
        glUniformMatrix4fv(glGetUniformLocation(g_program, "Projection"), 1, GL_FALSE, projectionMatrix);
        glUniform1f(g_locationElevation, 1.0f);
        glUniform1i(g_locationEnableTexture, showTextures);
        glUniform1i(g_locationCurrEffect, currEffect);
        glUniform1i(g_locationAmountVertices, MESH_AMOUNT_VERTICES);
        glUniform1i(g_locationDebugState, currDebugState);
        glUseProgram(0);
    }
}

void drawScene(void)
{
    /* Aktivieren des Programms. Ab jetzt ist die Fixed-Function-Pipeline
   * inaktiv und die Shader des Programms aktiv. */
    glUseProgram(g_program);
    float viewMatrix[16];

    const float distance = 2;
    //rotierende Kamera
    lookAt(distance * sinf(g_timer), 1, distance * cosf(g_timer), 0, 0, 0, 0, 1, 0, viewMatrix);
    //rotierende Lichtquelle
    glUniform3f(g_locationLightPos, -distance * sinf(2 * g_timer), 1.5f, -distance * cosf(2 * g_timer));

    float greenValue = (sin(g_timer) / 2.0f) + 0.5f;
    //zeitabhängiger Einfarbiger Farbwechsel
    glUniform4f(glGetUniformLocation(g_program, "color"), 0.4f, greenValue, 0.0f, 1.0f);

    /* Übermitteln der View-Matrix an den Shader.
   * OpenGL arbeitet intern mit Matrizen in column-major-layout. D.h.
   * nicht die Reihen, sondern die Spalten liegen hintereinander im
   * Speicher. Die Funktionen zur Erzeugen von Matrizen in diesem 
   * Programm berücksichtigen dies. Deswegen müssen die Matrizen nicht
   * transponiert werden und es kann GL_FALSE übergeben werden. Beim
   * Schreiben eigener Matrix-Funktionen oder beim Verwenden von
   * Mathematik-Bibliotheken muss dies jedoch berücksichtigt werden. */
    glUniformMatrix4fv(g_locationModelViewMatrix, 1, GL_FALSE, viewMatrix);

    /* Übermitteln der aktuellen Anhebung */
    glUniform1f(g_locationElevation, g_elevation);

    /* Übergeben der Textur an den Shader.
   * Texturen werden nicht direkt an den Shader übergeben, sondern
   * zuerst an eine Textureinheit gebunden. Anschließend wird dem
   * Programm nur der Index der Textureinheit übergeben. */
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, g_HeightTexture);
    glUniform1i(g_locationHeightTexture, 0);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, g_WorldTexture);
    glUniform1i(g_locationWorldTexture, 1);
    /* Aktivieren des Vertex-Array-Objekts (VAO).
   * Hiermit werden alle Attribut-Pointer aktiv, die auf diesem VAO
   * gesetzt wurden. */
    glBindVertexArray(g_vertexArrayObject);

    /* Rendern der Dreiecke.
   * Ab dem ersten Dreieck im Buffer werden alle Dreiecke gerendert.
   * Dem Draw-Command wird jedoch die Anzahl der Vertizes übergeben, die
   * gezeichnet werden sollen. */
    glDrawArrays(GL_TRIANGLES, 0, AMOUNT_INDICES);

    glBindTexture(GL_TEXTURE_2D, 0);

    /* Zurücksetzen des OpenGL-Zustands, um Seiteneffekte zu verhindern */
    glBindVertexArray(0);
    glUseProgram(0);
}

void setEffectState(effectState newEffect)
{
    currEffect = newEffect;
    glUseProgram(g_program);
    glUniform1i(g_locationCurrEffect, currEffect);
    glUseProgram(0);
}

void setDebugState(debugState newState)
{
    currDebugState = newState;
    glUseProgram(g_program);
    glUniform1i(g_locationDebugState, currDebugState);
    glUseProgram(0);
}

void toggleTextures(void)
{
    showTextures = (showTextures + 1) % 2;
    glUseProgram(g_program);
    glUniform1i(g_locationEnableTexture, showTextures);
    glUseProgram(0);
}

void toggleWireframeMode(void)
{
    /* Flag: Wireframe: ja/nein */
    static GLboolean wireframe = GL_FALSE;

    /* Modus wechseln */
    wireframe = !wireframe;

    if (wireframe)
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }
    else
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }
}


effectState getEffectState(void)
{
    return currEffect;
}

debugState getDebugState(void)
{
    return currDebugState;
}

void setElevation(GLdouble elevation)
{
    if (g_elevation >= -0.000001f)
    {
        g_elevation += elevation;
        g_elevation = fmax(g_elevation, 0);
    }
}

void setTimer(double interval)
{
    g_timer += interval;
}