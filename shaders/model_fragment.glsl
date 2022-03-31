#version 330 core
out vec4 glFragColor;

struct Material {
    vec3 Ambient;
    vec3 Diffuse;
    vec3 Specular;
};

in vec3 vFragNormal;
in vec4 vWorldSpaceCoord;
in vec2 vTexCoords;

uniform vec3 uLightPosition;

uniform bool uShadingOn;
uniform Material uMaterial;
uniform vec4 uColor;
uniform bool uHasTexture;
uniform sampler2D uTexture0;

void main()
{
    if (!uShadingOn) {
        glFragColor = vec4(uColor.rgb, 1.0);
        return;
    }

    vec3 lightColor = vec3(1.0f, 1.0f, 1.0f);
    vec3 lightDirection = normalize(uLightPosition - vWorldSpaceCoord.xyz);

    // diffuse
    float diffuseCoef = max(dot(vFragNormal, lightDirection), 0.0);
    vec3 diffuse = diffuseCoef * uMaterial.Diffuse * lightColor;

    // ambient
    vec3 ambient = uMaterial.Ambient * lightColor;

    vec3 totalLight = diffuse + ambient;

    vec4 rawColor;
    if(uHasTexture) rawColor = texture2D(uTexture0, vTexCoords); else rawColor = uColor;

    // out
    glFragColor = vec4(totalLight * rawColor.rgb, rawColor.a);
}