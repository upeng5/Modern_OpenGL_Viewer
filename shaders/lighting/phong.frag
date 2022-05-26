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
uniform float u_Shininess;

uniform sampler2D texture_diffuse1;
uniform vec3 u_CameraPos;

void main()
{
    // ambient
    vec3 ambient  = light.ambient;
  	
    // diffuse 
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(light.position - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    //vec3 diffuse  = light.diffuse * (diff * material.diffuse);
    vec3 diffuse  = light.diffuse * (diff);

    // specular
    float specularStrength = 0.5;
    vec3 viewDir = normalize(u_CameraPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), u_Shininess);
    //vec3 specular = light.specular * (spec * material.specular);
     vec3 specular = light.specular * (spec);

    // Sample texture
    vec3 texels = vec3(texture(texture_diffuse1, TexCoords));
    vec3 result = texels * (ambient + diffuse + specular);
    FragColor = vec4(result, 1.0);
};