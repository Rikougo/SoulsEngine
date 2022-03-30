#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

out vec2 TexCoords;
out vec3 fragNormal;
out vec4 worldSpaceCoord;

uniform bool uUseNormalMap;
uniform sampler2D uNormalMap;

uniform mat4 uModel;
uniform mat4 uView;
uniform mat4 uProjection;

void main()
{
    TexCoords = aTexCoords;

    if (uUseNormalMap)
        fragNormal = normalize(mat3(transpose(inverse(uModel))) * texture2D(uNormalMap, aTexCoords).rgb);
    else
        fragNormal = normalize(mat3(transpose(inverse(uModel))) * aNormal);

    worldSpaceCoord = uModel * vec4(aPos, 1.0);
    gl_Position = uProjection * uView * uModel * vec4(aPos, 1.0);
}