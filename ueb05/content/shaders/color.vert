#version 330 core
/** 
 * Position des Vertex. 
 * Wird von der Host-Anwendung über den Attribut-Pointer 0 
 * bereitgestellt. 
 */
layout (location = 0) in vec4 vPosition;
/**
 * Textur-Koordinate des Vertex. 
 * Wird von der Host-Anwendung über den Attribut-Pointer 1
 * bereitgestellt. 
 */
layout (location = 1) in vec2 vTexCoord;

layout (location = 2) in vec3 normal;

/** 
 * Ausgabe des Vertex-Shaders. 
 * Die Textur-Koordinate wird interpoliert vom Fragment-Shader 
 * entgegengenommen.
 */
out vec2 fTexCoord;


/** 
 * Projektions-Matrix.
 * Wird dem Shader mit einem Aufruf von glUniformMatrix4fv übergeben.
 */
uniform mat4 Projection;
/**
 * View-Matrix. 
 * Wird dem Shader mit einem Aufruf von glUniformMatrix4fv übergeben.
 */
uniform mat4 ModelView;

/**
 * Anhebung der Vertizes.
 * Wird dem Shader mit einem Aufruf von glUniform1f übergeben.
 */
uniform float Elevation;

uniform int amountVertices;

/**
 * Texture-Sampler, um auf die Textur zuzugreifen. Die Einstellungen
 * des Samplers (Interpolation und Randbehandlung) werden beim Erstellen
 * der Textur gesetzt.
 */
uniform sampler2D heightTexture;

out vec4 heightMapColor;

uniform vec4 color;

out vec4 elemColor;

//PHONG
// ------------------- //
uniform vec3 lightPos;

out vec3 vertexNormal;

out vec3 eyeDir;

out vec3 lightDir;
// ------------------- //

vec3 calcSphereCoords(vec4 coord)
{
    const float RADIUS = 0.75f;
    vec3 U = vec3(1.0f, 0.0f, 0.0f);
    vec3 V = vec3(0.0f, 0.0f, 1.0f);
    float u = coord.x * U.x + coord.y * U.y + coord.z * U.z;
    float v = coord.x * V.x + coord.y * V.y + coord.z * V.z;
    float longitude =  6.2831853 * u / 2.0f;
    float latitude = 3.1415926 * v / 2.0f;
    float r = RADIUS + coord.y;

    return vec3(r * cos(latitude) * sin(longitude), r * sin(latitude), r * cos(latitude) * cos(longitude));
}

vec3 updateNormal(void) {
    float vertexDistance = 2.0f / (amountVertices - 1);
    float texDistance = 1.0f / (amountVertices - 1);
    vec3 upperNeighbour = vec3(vPosition.x, 0.0f, vPosition.z - vertexDistance);
    vec3 lowerNeighbour = vec3(vPosition.x, 0.0f, vPosition.z + vertexDistance); 
    vec3 leftNeighbour = vec3(vPosition.x - vertexDistance, 0.0f, vPosition.z);
    vec3 rightNeighbour = vec3(vPosition.x + vertexDistance, 0.0f, vPosition.z);

    //Randbehandlung -> Punkt selbst nehmen
    upperNeighbour.y = Elevation * texture(heightTexture, vec2(vTexCoord.x, clamp(vTexCoord.y + texDistance, 0.0, 1.0))).r;
    lowerNeighbour.y = Elevation * texture(heightTexture, vec2(vTexCoord.x, clamp(vTexCoord.y - texDistance, 0.0, 1.0))).r;
    leftNeighbour.y = Elevation * texture(heightTexture, vec2(clamp(vTexCoord.x - texDistance, 0.0, 1.0), vTexCoord.y)).r;
    rightNeighbour.y = Elevation * texture(heightTexture, vec2(clamp(vTexCoord.x + texDistance, 0.0, 1.0), vTexCoord.y)).r;

    upperNeighbour = calcSphereCoords(vec4(upperNeighbour, 1.0));
    lowerNeighbour = calcSphereCoords(vec4(lowerNeighbour, 1.0));
    leftNeighbour = calcSphereCoords(vec4(leftNeighbour, 1.0));
    rightNeighbour = calcSphereCoords(vec4(rightNeighbour, 1.0));

    vec3 NS = upperNeighbour - lowerNeighbour;
    vec3 EW = leftNeighbour - rightNeighbour;
    return normalize(cross(EW, NS));
}

/**
 * Hauptprogramm des Vertex-Shaders.
 * Diese Funktion wird für jeden Vertex ausgeführt!
 */
void main(void)
{
    /* Neue Vertex-Position berechnen auf Grundlage der übergebenen
     * Anhebung. vPosition ist die Vertex-Position, die von der Host-
     * Anwendung im Buffer gespeichert wurde. Die w-Komponente wird
     * von der OpenGL-Implementierung automatisch auf 1 gesetzt. */
    vec4 elevatedPosition = vec4(vPosition.x, Elevation * texture(heightTexture, vTexCoord).r, vPosition.z, 1);
    elevatedPosition = vec4(calcSphereCoords(elevatedPosition), 1);

    /* Die Textur-Koordinate wird untransformiert an den Fragment-
     * Shader weitergereicht. Bei der Rasterization wird dieser Wert
     * jedoch interpoliert. */
    fTexCoord = vTexCoord;

    elemColor = color;

    heightMapColor = vec4(texture(heightTexture, vTexCoord).r, texture(heightTexture, vTexCoord).r, texture(heightTexture, vTexCoord).r, 1.0);

    //PHONG
    // ------------------- //
    mat4 normalMatrix = transpose(inverse(ModelView));
    vertexNormal = (normalMatrix * vec4(updateNormal(), 0.0)).xyz;
    vertexNormal = normalize(vertexNormal);

    vec3 vertexInCamSpace = (ModelView * elevatedPosition).xyz;
    eyeDir = -vertexInCamSpace.xyz;

    vec3 lightInCamSpace = vec4(ModelView * vec4(lightPos, 1.0)).xyz;
    lightDir = vec3(lightInCamSpace - vertexInCamSpace).xyz;
    // ------------------- //

    /* Setzen der Vertex-Position im Device-Koordinatensystem.
     * Nachfolgend findet das Clipping und die Rasterization statt. */
    gl_Position = Projection * vec4(vertexInCamSpace, 1.0);
}
