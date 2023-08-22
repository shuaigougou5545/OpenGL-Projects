#version 330 core

#define MaxLights 16

#ifndef NUM_DIR_LIGHTS
    #define NUM_DIR_LIGHTS 3
#endif

#ifndef NUM_POINT_LIGHTS
    #define NUM_POINT_LIGHTS 0
#endif

#ifndef NUM_SPOT_LIGHTS
    #define NUM_SPOT_LIGHTS 0
#endif


struct Light{
    vec3 Strength;
    float FalloffStart; // point/spot light only
    vec3 Position; // point light only
    float FalloffEnd; // point/spot light only
    vec3 Direction; // directional/spot light only
    float SpotPower; // spot light only
};

struct Material{
    sampler2D DiffuseTexture;
    vec4 DiffuseAlbedo;
    vec3 FresnelR0;
    float Roughness;
};

out vec4 FragColor;

in vec3 PosW;
in vec3 NormalW;
in vec2 TexCoords;

uniform vec4 gAmbientLight;
uniform Light gLights[MaxLights];
uniform Material gMat;
uniform vec3 gViewPos;


const float pi = 3.1415926;


float CalcAttenuation(float d, float falloffStart, float falloffEnd)
{
    // linear
    return clamp((falloffEnd - d) / (falloffEnd - falloffStart), 0.0, 1.0);
}

vec3 SchlickFresnel(vec3 R0, vec3 normal, vec3 lightVec)
{
    float cosIncidentAngle = max(dot(normal, lightVec), 0.0);
    float f0 = 1 - cosIncidentAngle;
    return R0 + (1.0 - R0) * (f0*f0*f0*f0*f0);
}

vec3 BlinnPhong(vec3 lightStrength, vec3 lightVec, vec3 normal, vec3 toEye, Material mat)
{
    // calculate diffuse albedo & specular albedo
    float m = mat.Roughness * 256.0f;
    vec3 halfVec = normalize(toEye + lightVec);

    float roughnessFactor = (m + 8.0f)*pow(max(dot(halfVec, normal), 0.0f), m) / 8.0f;
    vec3 fresnelFactor = SchlickFresnel(mat.FresnelR0, halfVec, lightVec);

    vec3 specAlbedo = fresnelFactor*roughnessFactor;

    specAlbedo = specAlbedo / (specAlbedo + 1.0f);

    return (mat.DiffuseAlbedo.xyz + specAlbedo) * lightStrength;
}

vec3 ComputeDirectionalLight(Light L, Material mat, vec3 normal, vec3 toEye)
{
   vec3 lightVec = -L.Direction;

   float ndotl = max(dot(lightVec, normal), 0.0f);
   vec3 lightStrength = L.Strength * ndotl;

   return BlinnPhong(lightStrength, lightVec, normal, toEye, mat);
}

vec3 ComputePointLight(Light L, Material mat, vec3 pos, vec3 normal, vec3 toEye)
{
   vec3 lightVec = L.Position - pos;

   float d = length(lightVec);

   if(d > L.FalloffEnd)
       return vec3(0.f);

   lightVec /= d;

   float ndotl = max(dot(lightVec, normal), 0.0f);
   vec3 lightStrength = L.Strength * ndotl;

   float att = CalcAttenuation(d, L.FalloffStart, L.FalloffEnd);
   lightStrength *= att;

   return BlinnPhong(lightStrength, lightVec, normal, toEye, mat);
}

vec3 ComputeSpotLight(Light L, Material mat, vec3 pos, vec3 normal, vec3 toEye)
{
   vec3 lightVec = L.Position - pos;

   float d = length(lightVec);

   if(d > L.FalloffEnd)
       return vec3(0.f);

   lightVec /= d;

   float ndotl = max(dot(lightVec, normal), 0.0f);
   vec3 lightStrength = L.Strength * ndotl;

   float att = CalcAttenuation(d, L.FalloffStart, L.FalloffEnd);
   lightStrength *= att;

   float spotFactor = pow(max(dot(-lightVec, L.Direction), 0.0f), L.SpotPower);
   lightStrength *= spotFactor;

   return BlinnPhong(lightStrength, lightVec, normal, toEye, mat);
}

vec4 ComputeLighting(Light lights[MaxLights], Material mat, vec3 pos, vec3 normal, vec3 toEye, vec3 shadowFactor)
{
    vec3 result = vec3(0.f);
    int i = 0;
    
#if (NUM_DIR_LIGHTS > 0)
    for(i = 0; i < NUM_DIR_LIGHTS; ++i)
    {
        result += shadowFactor[i] * ComputeDirectionalLight(lights[i], mat, normal, toEye);
    }
#endif

#if (NUM_POINT_LIGHTS > 0)
    for(i = NUM_DIR_LIGHTS; i < NUM_DIR_LIGHTS+NUM_POINT_LIGHTS; ++i)
    {
        result += ComputePointLight(lights[i], mat, pos, normal, toEye);
    }
#endif

#if (NUM_SPOT_LIGHTS > 0)
    for(i = NUM_DIR_LIGHTS + NUM_POINT_LIGHTS; i < NUM_DIR_LIGHTS + NUM_POINT_LIGHTS + NUM_SPOT_LIGHTS; ++i)
    {
        result += ComputeSpotLight(lights[i], mat, pos, normal, toEye);
    }
#endif

    return vec4(result, 0.0f);
}

void main()
{
    vec3 normalW = normalize(NormalW);
    vec3 toEyeW = normalize(gViewPos - PosW);
    
    vec4 diffuseAlbedo = gMat.DiffuseAlbedo;
    vec4 ambient = gAmbientLight * diffuseAlbedo;
    
    vec3 shadowFactor = vec3(1.f);
    vec4 directLight = ComputeLighting(gLights, gMat, PosW, NormalW, toEyeW, shadowFactor);
    
    vec4 col = ambient + directLight;
    
    FragColor = col;
}

