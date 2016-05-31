#version 140

in vec2 inVertexPos;
in vec2 inTexturePos;

uniform float time;

out vec2 texturePos;

void main()
{
    texturePos = inTexturePos;
    gl_Position = vec4(inVertexPos.x, inVertexPos.y, 0, 1);
}