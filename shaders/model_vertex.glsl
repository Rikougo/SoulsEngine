#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

out vec4 worldSpaceCoord;
out vec2 TexCoords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform bool useHeightmap;
uniform sampler2D heightmap;

void main()
{
    if (!useHeightmap) {
        TexCoords = aTexCoords;

        worldSpaceCoord = model * vec4(aPos, 1.0);
        gl_Position = projection * view * model * vec4(aPos, 1.0);
    } else {
        TexCoords = aTexCoords;

        vec3 heightedPosition = vec3(aPos.x, texture2D(heightmap, aTexCoords).r * 0.5f, aPos.z);

        worldSpaceCoord = model * vec4(heightedPosition, 1.0);
        gl_Position = projection * view * model * vec4(heightedPosition, 1.0);
    }
}