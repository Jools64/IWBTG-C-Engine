#version 140

in vec2 texturePos;
uniform sampler2D uTextureSampler;

out vec4 outColor;

void main()                                   
{         
    outColor = texture2D(uTextureSampler, texturePos) * vec4(1.0, 1.0, 1.0, 1.0);
    //outColor = vec4(texturePos.x, texturePos.y, 1.0, 1.0);
}