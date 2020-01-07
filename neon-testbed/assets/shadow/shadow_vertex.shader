#version 330

layout(location = 0) in vec2 aPos;
layout(location = 1) in vec2 aTexCoords;

uniform vec3 light_direction;
uniform mat4 view;
uniform mat4 world;
uniform mat4 projection;

out vec2 TexCoords;

void main() {
	gl_Position = projection * view * world * vec4(light_direction, 0.0);
	TexCoords = aTexCoords;
}