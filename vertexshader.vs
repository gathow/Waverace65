#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;

out vec2 TexCoord;

uniform mat4 model;
uniform mat4 viewmatrix;
uniform mat4 perspectiveprojection;
uniform float time;
uniform bool animateWater;

const float PI = 3.14159265359;

void addWave(
    inout vec3 p,
    vec2 direction,
    float amplitude,
    float wavelength,
    float speed,
    float steepness)
{
    direction = normalize(direction);

    float k = 2.0 * PI / wavelength;
    float phase = k * dot(direction, aPos.xz) - speed * time;

    p.x += direction.x * steepness * amplitude * cos(phase);
    p.z += direction.y * steepness * amplitude * cos(phase);
    p.y += amplitude * sin(phase);
}

void main()
{
vec3 p = aPos;
if (animateWater) {
    addWave(p, vec2( 1.0,  0.25), 0.22, 7.0, 1.4, 0.55);
    addWave(p, vec2(-0.45, 1.0),  0.12, 4.0, 1.8, 0.45);
    addWave(p, vec2( 0.7, -1.0),  0.06, 2.5, 2.2, 0.35);
}
gl_Position = perspectiveprojection * viewmatrix * model * vec4(p, 1.0);


    gl_Position = perspectiveprojection * viewmatrix * model * vec4(p, 1.0);
    TexCoord = aTexCoord;
}

