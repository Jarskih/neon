#version 330

in vec3 in_position;

uniform mat4 mvpMatrix;

void main() {
	gl_position = mvpMatrix * vec4(in_position, 1.0);
}