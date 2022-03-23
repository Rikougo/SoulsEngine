#version 330 core
out vec4 glFragColor;

in vec3 fragNormal;
in vec4 worldSpaceCoord;
in vec2 TexCoords;


const float offset = 1.0 / 2.0;
uniform vec3 lightPosition;
uniform vec3 uAmbient;
uniform bool hasTexture;
uniform vec3 uColor;
uniform sampler2D texture0;

void main()
{
    vec3 lightColor = vec3(1.0f, 1.0f, 1.0f);
    vec3 lightDir = normalize(lightPosition - worldSpaceCoord.xyz);
    float diff = max(dot(fragNormal, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;

    vec3 ambient = uAmbient * lightColor;

    vec4 color;
    if(hasTexture) color = texture2D(texture0, TexCoords); else color = vec4(uColor.xyz, 1.0f);
    glFragColor = vec4((diffuse + ambient) * color.rgb, 1.0);
}