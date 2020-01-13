#version 330

uniform sampler2D diffuse;
uniform vec4 light_color;
uniform vec3 light_direction;

in vec3 f_normal;
in vec2 f_texcoord;
in vec3 f_view_pos;
in vec3 f_position;
//in vec3 light_pos;

out vec4 frag_color;

const float shininess = 16.0;

void main()
{
	vec3 N = normalize(f_normal);
	vec3 L = normalize(light_direction);
	vec3 viewDir = normalize(f_view_pos - f_position);
	vec3 halfwayDir = normalize(L + viewDir);

	vec4 tex_color = texture(diffuse, f_texcoord);

	// Ambient light
	vec3 ambient = vec3(0.1f, 0.1f, 0.1f);

	// Diffuse light
	float NdL = clamp(dot(N, -L), 0, 1);
	vec4 diffuse = vec4(NdL * tex_color.rgb * vec3(light_color), tex_color.a);

	// Specular light (spec is calculated by taking dot product of view direction and reflect direction. Reflect() function gives us reflection vector using the normal)
	vec3 finalSpec = vec3(0);
	if (NdL > 0) {
		vec3 reflectDir = normalize(reflect(-L, N));
		float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
		finalSpec = vec3(light_color) * spec;
	}

	// output = Phong shading https://en.wikipedia.org/wiki/Phong_shading
	frag_color = tex_color * vec4(ambient, 1) + diffuse + vec4(finalSpec,1);
}
