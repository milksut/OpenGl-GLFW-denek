#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;
layout (location = 2) in vec2 aTexCoord;

uniform mat4 model;

out vec3 ourColor;
out vec2 TexCoord;
out vec3 pos;

void main()
{
    gl_Position = model * vec4(aPos.x,aPos.y,-aPos.z, 1.0); 
    ourColor = aColor;
    TexCoord = aTexCoord;
    pos = vec3(gl_Position.x, gl_Position.y, gl_Position.z);
}