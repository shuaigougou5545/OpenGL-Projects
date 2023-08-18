#version 330 core
out vec4 FragColor;

in vec3 PosW;
in vec3 NormalW;

uniform vec3 lightPos;
uniform vec3 lightColor;
uniform vec3 objectAlbedo;


void main()
{
    float ambientStrength = 0.3;
    vec3 ambient = ambientStrength * lightColor;
    
    vec3 norm = normalize(NormalW);
    vec3 lightDir = normalize(lightPos - PosW);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;
    
    vec3 col = (ambient + diffuse) * objectAlbedo;
    
    FragColor = vec4(col, 1.0);
}
