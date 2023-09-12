#version 330 core

out vec4 FragColor;

in VS_OUT{
    vec3 texture_dir;
} fs_in;


uniform samplerCube cubemap;

void main()
{
    FragColor = texture(cubemap, fs_in.texture_dir);
}
