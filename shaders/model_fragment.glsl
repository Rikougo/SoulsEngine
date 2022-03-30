#version 330 core
out vec4 glFragColor;

in vec3 fragNormal;
in vec4 worldSpaceCoord;
in vec2 TexCoords;

uniform vec3 uLightPosition;
uniform vec3 uAmbient;
uniform bool uHasTexture;
uniform vec3 uColor;

uniform bool uShadingOn;
uniform sampler2D uTexture0;

void main()
{
    if (!uShadingOn) {
        glFragColor = vec4(uColor.rgb, 1.0);
        return;
    }

    vec3 lightColor = vec3(1.0f, 1.0f, 1.0f);
    vec3 lightDir = normalize(uLightPosition - worldSpaceCoord.xyz);

    // diffuse
    float diff = max(dot(fragNormal, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;

    vec3 ambient = uAmbient * lightColor;

    vec4 color;
    if(uHasTexture) color = texture2D(uTexture0, TexCoords); else color = vec4(uColor.xyz, 1.0f);

    // out
    glFragColor = vec4((diffuse + ambient) * color.rgb, 1.0);
}