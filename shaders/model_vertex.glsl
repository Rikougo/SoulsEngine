#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

out vec2 TexCoords;
out vec3 fragNormal;
out vec4 worldSpaceCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    TexCoords = aTexCoords;

    fragNormal = normalize(mat3(transpose(inverse(model))) * aNormal);
    worldSpaceCoord = model * vec4(aPos, 1.0);

    gl_Position = projection * view * model * vec4(aPos, 1.0);
}