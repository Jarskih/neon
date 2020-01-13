#version 330

layout(location = 0) in vec2 position;
layout(location = 1) in vec2 textcoord;

uniform mat4 projection;

out vec2 f_texcoord;

void main()
{
	gl_Position = projection * vec4(position, 0, 1);
	f_texcoord = textcoord;
}