#version 330 core

out vec4 FragColor;

in float depth;
in vec3 texture_dir;

uniform samplerCube cubemap;


void main()
{
    FragColor = texture(cubemap, texture_dir);
    FragColor = vec4(vec3(depth), 1.0);
}

