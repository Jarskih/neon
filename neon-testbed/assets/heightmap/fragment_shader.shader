//#version 330
//
//uniform sampler2D diffuse;
//uniform vec3 light_direction;
//
//in vec3 f_normal;
//in vec2 f_texcoord;
//
//out vec4 frag_color;
//
//void main()
//{
//	vec3 N = normalize(f_normal);
//	vec3 L = normalize(light_direction);
//	float NdL = dot(N, -L);
//
//	vec4 tex_color = texture(diffuse, f_texcoord);
//	frag_color = vec4(NdL * tex_color.rgb, tex_color.a);
//}

#version 330

uniform sampler2D diffuse;
uniform sampler2D shadowMap;
uniform vec4 light_color;
uniform vec3 light_direction;

in vec3 f_normal;
in vec2 f_texcoord;
in vec3 f_view_pos;
in vec3 f_position;
in vec4 f_shadow;

//in vec3 light_pos;

out vec4 frag_color;

const float shininess = 50.0f;

// https://learnopengl.com/Advanced-Lighting/Shadows/Shadow-Mapping
float ShadowCalculation(vec4 fragPosLightSpace)
{
	// perform perspective divide
	vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
	// transform to [0,1] range
	projCoords = projCoords * 0.5 + 0.5;
	// get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
	float closestDepth = texture(shadowMap, projCoords.xy).r;
	// get depth of current fragment from light's perspective
	float currentDepth = projCoords.z;
	// check whether current frag pos is in shadow
	float shadow = currentDepth > closestDepth ? 1.0 : 0.0;

	return shadow;
}

void main()
{
	vec3 N = normalize(f_normal);
	vec3 L = normalize(light_direction);
	vec3 viewDir = normalize(f_view_pos - f_position);
	vec3 halfwayDir = normalize(L + viewDir);

	vec4 tex_color = texture(diffuse, f_texcoord);

	// Ambient light
	vec3 ambient = 0.15f * tex_color.rgb;

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
	float shadow = ShadowCalculation(f_shadow);
	vec3 lighting = (ambient + (1.0 - shadow) * (vec3(diffuse) + finalSpec)) * tex_color.rgb;
	frag_color = vec4(lighting, 1);
}
