#version 330 core
layout(location = 0) in vec3 aPos;
out vec3 pos;
uniform vec2 offset;


void main()
{
    pos = vec3(aPos.x + offset.x, -aPos.y + offset.y, aPos.z);
    gl_Position = vec4(pos, 1.0);
}
