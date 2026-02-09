#version 100

varying mediump vec2 texCoord;

uniform mediump sampler2D tex;

void main()
{
    gl_FragColor = texture2D(tex, texCoord);
}