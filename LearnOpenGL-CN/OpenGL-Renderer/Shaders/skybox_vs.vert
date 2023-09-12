#version 330 core
layout(location = 0) in vec3 aPos;

out VS_OUT{
    vec3 texture_dir;
} vs_out;


uniform mat4 view;
uniform mat4 projection;

void main()
{
    vs_out.texture_dir = aPos;
    gl_Position = projection * view * vec4(aPos, 1.0);
    gl_Position = gl_Position.xyww;
}
