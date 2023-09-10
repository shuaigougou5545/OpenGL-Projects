#version 330 core
layout(location = 0) in vec3 aPos;

out vec3 texture_dir;
out float depth;

uniform mat4 view;
uniform mat4 projection;


void main()
{
    texture_dir = aPos;
    gl_Position = projection * view * vec4(aPos, 1.0);
    depth = gl_Position.z / gl_Position.w;
}
