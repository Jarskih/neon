#version 330

uniform samplerCube cubemap;

in vec3 f_texcoord;

out vec4 frag_color;

void main()
{
	frag_color = texture(cubemap, f_texcoord);
}