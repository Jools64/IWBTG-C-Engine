#version 140

in vec2 inVertexPos;
in vec2 inTexturePos;

uniform float time;

out vec2 texturePos;

const float pi = 3.141592;

void main()
{
    // TODO: Upload this to the shader from game

    float l = 0.0, r = 960.0, t = 0.0, b = 540.0, // Left, right, top and bottom of view
          angle = 0, scaleX = 1.0, scaleY = 1.0, offsetX = 0, offsetY = 0; // Camera positioning
    
    mat4 orthographicMatrix = mat4(
       2 / (r - l)       , 0.0                , 0.0, 0.0,
       0.0               , 2 / (t - b)        , 0.0, 0.0,
       0.0               , 0.0                , 1.0, 0.0,
       -(r + l) / (r - l), -(t + b) / (t - b) , 0.0, 1.0
    );
    
    mat4 cameraMatrix = mat4(
        cos(angle) * scaleX, -sin(angle) * scaleY, 0.0, 0.0,
        sin(angle) * scaleY, cos(angle) * scaleX, 0.0, 0.0,
        0.0, 0.0, 1.0, 0.0,
        -offsetX, -offsetY, 0.0, 1.0
    );

    texturePos = inTexturePos;
    vec4 position = orthographicMatrix * cameraMatrix * vec4(inVertexPos, 0.0, 1.0);
    
    gl_Position = position;
}