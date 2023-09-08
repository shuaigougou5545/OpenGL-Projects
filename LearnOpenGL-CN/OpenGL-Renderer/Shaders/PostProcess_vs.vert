#version 330 core
layout(location = 0) in vec2 aPos;

out vec2 uv_interp;


void main()
{
    uv_interp = 0.5 * aPos + 0.5;
    gl_Position = vec4(aPos.xy, 0.0, 1.0);
}
