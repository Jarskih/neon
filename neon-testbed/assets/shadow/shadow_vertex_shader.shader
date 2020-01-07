#version 330

uniform vec3 light_direction;
uniform mat4 view;
uniform mat4 world;
uniform mat4 projection;

void main() {
	gl_Position = projection * view * world * vec4(light_direction, 0.0);
}