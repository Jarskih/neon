#version 330

layout(location = 0) in vec3 position;
layout(location = 1) in vec2 uv;
layout(location = 2) in vec3 normal;

uniform mat4 view;
uniform mat4 projection;
uniform mat4 world;

out vec2 f_texcoord;

void main()
{
	gl_Position = projection * view * world * vec4(position, 1.0);
	f_texcoord = uv;
}