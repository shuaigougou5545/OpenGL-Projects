#version 330 core
out vec4 FragColor;

in vec2 uv;

void main()
{
    FragColor = vec4(vec3(uv, 0.0), 1.0);
}
