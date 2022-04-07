#version 330 core

#define MAX_LIGHTS 10
const float PI =  3.14159265359;

struct Material {
    float metallic;
    float roughness;
    vec4 albedo;
    bool hasTexture;
};

struct Light {
    vec3 position;
    vec3 color;
};

layout (location = 0) out vec4 glFragColor;
layout (location = 3) out int outEntity;

in vec3 vFragNormal;
in vec4 vWorldSpaceCoord;
in vec2 vTexCoords;

uniform int uEntity;

uniform Material uMaterial;
uniform sampler2D uTexture0;

uniform vec3 uViewPos;
uniform bool uLightsOn;
uniform Light uLights[10];
uniform int uLightsAmount = 0;

vec3 FresnelSchlick(float cosTheta, vec3 F0) {
    return F0 + (1.0f - F0) * pow(clamp(1.0f - cosTheta, 0.0f, 1.0f), 5.0);
}

float DistributionGGX(vec3 N, vec3 H, float roughness) {
    float a = roughness * roughness;
    float a2 = a * a;
    float NdotH = max(dot(N, H), 0.0f);
    float NdotH2 = NdotH * NdotH;

    float num = a2;
    float denom = (NdotH2 * (a2 - 1.0f) + 1.0f);
    denom = PI * denom * denom;

    return num / denom;
}

float GeometrySchlickGGX(float NdotV, float roughness) {
    float r = roughness + 1.0f;
    float k = (r * r) / 8.0f;

    float num = NdotV;
    float denom = NdotV * (1.0f - k) + k;

    return num / denom;
}

float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2  = GeometrySchlickGGX(NdotV, roughness);
    float ggx1  = GeometrySchlickGGX(NdotL, roughness);

    return ggx1 * ggx2;
}

void main()
{
    vec4 rawColor = vec4(1.0f);

    if (uMaterial.hasTexture) {
        rawColor = texture2D(uTexture0, vTexCoords);
    }

    rawColor = rawColor * uMaterial.albedo;

    // no shading here
    if (uLightsAmount == 0 || !uLightsOn) {
        glFragColor = rawColor;

        return;
    }

    // PBR SHADING
    vec3 N = normalize(vFragNormal);
    vec3 V = normalize(uViewPos - vWorldSpaceCoord.xyz);

    vec3 F0 = vec3(0.04f);
    F0 = mix(F0, rawColor.xyz, uMaterial.metallic);

    vec3 Lo = vec3(0.0f);
    for(int i = 0; i < uLightsAmount; i++) {
        Light l = uLights[i];
        vec3 L = normalize(l.position - vWorldSpaceCoord.xyz);
        vec3 H = normalize(V + L);
        float distance    = length(l.position - vWorldSpaceCoord.xyz);
        float attenuation = 1.0f / (distance * distance);
        vec3 radiance     = l.color * attenuation;

        // cook-torrance brdf
        float NDF = DistributionGGX(N, H, uMaterial.roughness);
        float G   = GeometrySmith(N, V, L, uMaterial.roughness);
        vec3 F    = FresnelSchlick(clamp(dot(H, V), 0.0f, 1.0f), F0);

        vec3 kS = F;
        vec3 kD = vec3(1.0) - kS;
        kD *= 1.0 - uMaterial.metallic;

        vec3 numerator    = NDF * G * F;
        float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.0001;
        vec3 specular     = numerator / denominator;

        // add to outgoing radiance Lo
        float NdotL = max(dot(N, L), 0.0);
        Lo += (kD * rawColor.xyz / PI + specular) * radiance * NdotL;
    }

    vec3 ambient = vec3(0.03) * uMaterial.albedo.xyz;
    vec3 color = ambient + Lo;

    color = color / (color + vec3(1.0f)); // HDR Tonemapping
    // color = pow(color, vec3(1.0f / 2.2f)); // gamma correct

    glFragColor = vec4(color, 1.0f);
    outEntity = 0;
}