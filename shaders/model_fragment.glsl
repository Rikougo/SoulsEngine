#version 460 core
out vec4 FragColor;

in vec4 worldSpaceCoord;
in vec2 TexCoords;

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_diffuse2;
uniform sampler2D texture_diffuse3;

uniform bool useHeightmap;
uniform sampler2D heightmap;

void main()
{
    if (!useHeightmap) {
        FragColor = texture(texture_diffuse1, TexCoords);
    } else {
        float heightValue = texture(heightmap, TexCoords).r;

        float thresold = 0.7;
        float highCoef = 1.0 / (1.0 - thresold);
        float lowCoef = 1.0 / thresold;

        vec2 tCoord = vec2(worldSpaceCoord.x / 100.f, worldSpaceCoord.z / 100.f);

        if (heightValue > thresold) {
            vec4 fMix = mix(texture(texture_diffuse2, tCoord), texture(texture_diffuse1, tCoord), (heightValue - thresold) * highCoef);
            FragColor = fMix;
        } else {
            vec4 sMix = mix(texture(texture_diffuse3, tCoord), texture(texture_diffuse2, tCoord), (heightValue * lowCoef) * (heightValue * lowCoef) );
            FragColor = sMix;
        }
/*        float heightValue = texture(heightmap, TexCoords).r;
        if (heightValue > 0.8) {
            FragColor = texture(texture_diffuse1, TexCoords);
        } else if (heightValue > 0.5) {
            FragColor = texture(texture_diffuse2, TexCoords);
        } else {
            FragColor = texture(texture_diffuse3, TexCoords);
        }*/
    }
}