#version 100
attribute highp vec3 aPos;
attribute highp vec2 aTex;

varying highp vec2 texCoord;

void main()
{
    texCoord = aTex;
    gl_Position = vec4(aPos, 1.0);
}