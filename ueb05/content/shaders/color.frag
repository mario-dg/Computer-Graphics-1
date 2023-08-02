#version 330 core

/**
 * Interpolierte Textur-Koordinate des Fragments.
 */
in vec2 fTexCoord;

/**
 * Farbe des Fragments, die auf den Framebuffer geschrieben wird.
 */
out vec4 FragColor;

in vec4 elemColor;

uniform int enableTexture;
uniform int currEffect;
uniform int debugState;

/**
 * Texture-Sampler, um auf die Textur zuzugreifen. Die Einstellungen
 * des Samplers (Interpolation und Randbehandlung) werden beim Erstellen
 * der Textur gesetzt.
 */
uniform sampler2D worldTexture;
in vec4 heightMapColor;

struct LightSource {
    vec3 ambient_color;
    vec3 diffuse_color;
    vec3 specular_color;
};

struct Material {
    vec3 ambient_color;
    vec3 diffuse_color;
    vec3 specular_color;
    float shininess;
};

in vec3 vertexNormal;
in vec3 eyeDir;
in vec3 lightDir;

vec3 calcPhong(void) {
    LightSource light;
    light.ambient_color = vec3(0.5, 0.5, 0.5f);
    light.diffuse_color = vec3(0.5f, 0.5, 0.5f);
    light.specular_color = vec3(1.0f);
    Material material;
    material.ambient_color = vec3(1.0f);
    material.diffuse_color = vec3(1.0f);
    material.specular_color = vec3(0.5f);
    material.shininess = 5.0f;

    vec3 E = normalize(eyeDir);
    vec3 N = normalize(vertexNormal);
    vec3 L = normalize(lightDir);

    vec3 reflectDir = reflect(-L, N);
    reflectDir = normalize(reflectDir);
    float specularAngle = max(dot(reflectDir, E), 0.0);

    vec3 ambient = light.ambient_color;
    vec3 diffuse = light.diffuse_color * max(dot(L,N), 0);
    vec3 specular = light.specular_color * pow(specularAngle, material.shininess / 4.0f);

    ambient *= material.ambient_color;
    diffuse *= material.diffuse_color;
    specular *= material.specular_color;

    return vec3(ambient + diffuse + specular);
}

vec4 cartoon(vec4 color)
{
    const vec4 lowerValue = vec4(0.1f);
    const vec4 upperValue = vec4(0.9f);
    float intensity = max(0, dot(normalize(lightDir), normalize(vertexNormal)));
    color = mix(lowerValue, upperValue, step(0.5, intensity)) * color; 
    return color;
}

vec4 sepia(vec4 color)
{
    color.x = .393 * color.x + .769 * color.y + .189 * color.z;
    color.y = .343 * color.x + .786 * color.y + .168 * color.z;
    color.z = .272 * color.x + .534 * color.y + .131 * color.z;
    return color; 
}

vec4 gray(vec4 color)
{
    //Standard Grayscale Konversion -> Menschliches Auge nimmt
    //unterschiedliche Farben anders stark war
    //CCIR601 Standard (1982)
    color.x = .3 * color.x + .59 * color.y + .11 * color.z;
    color.y = .3 * color.x + .59 * color.y + .11 * color.z;
    color.z = .3 * color.x + .59 * color.y + .11 * color.z;
    return color;
}

/**
 * Hauptprogramm des Fragment-Shaders.
 * Diese Funktion wird für jedes Fragment ausgeführt!
 */
void main(void)
{
    vec4 tempColor = mix(elemColor, texture(worldTexture, fTexCoord), step(0.5, enableTexture * 1.0f));  

    vec4 temp;

    if(debugState == 0){
        temp = mix(tempColor, tempColor * vec4(calcPhong(), 1.0), step(0.5, currEffect * 1.0f));

        temp = mix(temp, cartoon(tempColor), step(1.5, currEffect * 1.0f));

        temp = mix(temp, sepia(tempColor), step(2.5, currEffect * 1.0f));

        temp = mix(temp, gray(tempColor), step(3.5, currEffect * 1.0f));
    
        FragColor = temp;
    } 
    else if(debugState == 1)
    {   
        FragColor = vec4(vertexNormal, 1.0);
    } 
    else if(debugState == 2)
    {
        FragColor = heightMapColor;
    }
}
