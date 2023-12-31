#version 330 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;

out vec3 PosW;
out vec3 NormalW;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;


void main()
{
    PosW = vec3(model * vec4(aPos, 1.0));
    NormalW = mat3(transpose(inverse(model))) * aNormal;
    
    gl_Position = projection * view * model * vec4(aPos, 1.0);
}
