#version 330

layout(location = 0) in vec3 position;
layout(location = 1) in vec2 uv;
layout(location = 2) in vec3 normal;

uniform mat4 view;
uniform mat4 world;
uniform mat4 projection;
uniform vec3 camera_pos;
//uniform vec3 light_pos;

out vec2 f_texcoord;
out vec3 f_normal;
out vec3 f_view_pos;
out vec3 f_position;
out vec3 f_light_pos;

void main()
{
	gl_Position = projection * view * world * vec4(position, 1);
	f_position = position * mat3(world);

	f_texcoord = uv;

	f_view_pos = camera_pos * mat3(world);

	vec3 transformed_normal = mat3(world) * normal;
	f_normal = normalize(transformed_normal);

	//f_light_pos = light_pos * mat3(world);
}