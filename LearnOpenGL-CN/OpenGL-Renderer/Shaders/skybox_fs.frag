#version 330 core

out vec4 FragColor;

in vec3 texture_dir;

uniform samplerCube cubemap;

void main()
{
    FragColor = texture(cubemap, texture_dir);
}
