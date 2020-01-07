#version 330

uniform sampler2D depthMap;

out vec4 FragColor;

in vec2 TexCoords;

void main()
{
   float depthValue = texture(depthMap, TexCoords).r;
   FragColor = vec4(vec3(depthValue), 1.0);
}