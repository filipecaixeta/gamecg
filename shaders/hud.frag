#version 120

uniform mat4 Model;
varying vec2 TexCoord;

uniform sampler2D texture1;
uniform int useTexture;
uniform vec3 color;


void main()
{
    vec4 coloout;
    vec2 tc=TexCoord;
    tc=vec2(vec2(1.0,1.0)+TexCoord)/2;
    tc.y=-tc.y;
    if (useTexture==1)
        coloout=texture2D(texture1,tc);
    else
        coloout=vec4(color,1.0);
    if (coloout.a<0.5)
        discard;
    gl_FragColor = coloout;
        
}
