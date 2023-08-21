#version 330 core
out vec4 FragColor;

in vec3 PosW;
in vec3 NormalW;
in vec2 TexCoords;


struct Light{
    vec3 position;
    vec3 color;
};

struct Material{
    sampler2D diffuse;
    vec3 diffuseAlbedo;
    vec3 fresnelR0;
    float roughness;
};

uniform vec3 ambientLight;
uniform Light light0;
uniform Material mat0;
uniform vec3 viewPos;


const float pi = 3.1415926;


vec3 SchlickFresnel(vec3 R0, vec3 normal, vec3 lightVec)
{
    float cosIncidentAngle = max(dot(normal, lightVec), 0.0);
    float f0 = 1 - cosIncidentAngle;
    return R0 + (1.0 - R0) * (f0*f0*f0*f0*f0);
}

vec3 BlinnPhong(Light light, vec3 lightVec, vec3 normal, vec3 toEye, Material mat)
{
    float m = (1.0 - mat.roughness) * 255.0 + 1.0; // m >= 1
    vec3 halfDir = normalize(lightVec + toEye);
    float roughnessFactor = (m+8.0)/8.0 * pow(max(dot(lightVec, halfDir), 0.0), m);
    vec3 fresnelFactor = SchlickFresnel(mat.fresnelR0, normal, lightVec);
   
    vec3 specularAlbedo = fresnelFactor * roughnessFactor;
    return specularAlbedo;
}

void main()
{
    vec3 normalW = normalize(NormalW);
    vec3 lightDir = normalize(light0.position - PosW);
    vec3 viewDir = normalize(viewPos - PosW);
    float lambert_cos = max(dot(lightDir, normalW), 0.0) / pi;
    
    // ambient
//    vec3 diffuseAlbedo = mat0.diffuseAlbedo;
    vec3 diffuseAlbedo = texture(mat0.diffuse, TexCoords).xyz;
    vec3 ambient = ambientLight * diffuseAlbedo;
    
    // diffuse
    vec3 diffuse = lambert_cos * light0.color * diffuseAlbedo;
    
    // specular
    vec3 specularAlbedo = BlinnPhong(light0, lightDir, normalW, viewDir, mat0);
    vec3 specular = lambert_cos * light0.color * specularAlbedo;
    
    vec3 col = ambient + diffuse + specular;
    
    FragColor = vec4(col, 1.0);
}

