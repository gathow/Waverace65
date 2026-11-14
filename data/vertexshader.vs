#version 300 es
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;

out vec2 TexCoord;

uniform mat4 model;
uniform mat4 viewmatrix;
uniform mat4 perspectiveprojection;

void main()
{
    gl_Position = perspectiveprojection * viewmatrix * model * vec4(aPos, 1.0);
    TexCoord = aTexCoord;
}

