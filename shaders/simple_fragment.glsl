#version 330 core

uniform vec3 uLineColor;

out vec4 glFragColor;

void main() {
    glFragColor = vec4(uLineColor, 1.0f);
}
