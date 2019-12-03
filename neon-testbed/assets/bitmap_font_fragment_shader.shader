#version 330

uniform sampler2D diffuse;

in vec2 f_texcoord;
out vec4 frag_color;

void main()
{
	frag_color = texture(diffuse, f_texcoord)
}
