#version 460 core
out vec4 FragColor;

in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoords;

struct Light {
    vec3 position;
  
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

uniform Light light; 

uniform sampler2D texture_diffuse1;
uniform vec3 u_CameraPos;

void main()
{
    vec3 colorCold = vec3(0.0, 0.0, 1.0);
    vec3 colorWarm = vec3(1.0, 1.0, 0.0);

    // Get distance from fragment to light source
    float d = distance(light.position, FragPos);

    // Cool to warm shading
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(light.position - FragPos);
    float kw = (1 + dot(norm, lightDir))/2;
    // Adjust warm color strength based on distance to light
    kw = (kw * 1/d);

    vec3 texels = vec3(texture(texture_diffuse1, TexCoords));
    vec3 result = (kw*colorWarm) + (1 - kw) * colorCold;
        
    FragColor = vec4(result + texels, 1.0);
};