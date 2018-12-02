#version 330 core
layout (location = 0) in vec3 inVertPos;
layout (location = 1) in vec2 inTexPos;

out vec2 texPos;

uniform mat4 modelMat;          // From local to world
uniform mat4 viewMat;           // From world to camera
uniform mat4 projectionMat;     // From camera to screen

void main()
{
    gl_Position = projectionMat * viewMat * modelMat * vec4(inVertPos, 1.0);
    texPos = inTexPos;
}