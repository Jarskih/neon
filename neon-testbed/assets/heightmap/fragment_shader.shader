#version 330

uniform sampler2D diffuse;
uniform vec3 light_direction;

in vec3 f_normal;
in vec2 f_texcoord;

out vec4 frag_color;

void main()
{
	vec3 N = normalize(f_normal);
	vec3 L = normalize(light_direction);
	float NdL = dot(N, -L);

	vec4 tex_color = texture(diffuse, f_texcoord);
	frag_color = vec4(NdL * tex_color.rgb, tex_color.a);
}