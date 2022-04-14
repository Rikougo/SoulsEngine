#version 330 core

layout (location = 0) out vec4 glFragColor;

uniform vec4 uOutlineColor;

void main() {
    glFragColor = vec4(uOutlineColor);
}