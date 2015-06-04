#version 120

attribute vec3 position;
attribute vec3 normal;
attribute vec3 texCoords;
attribute vec3 tangent;
attribute vec3 color;

uniform mat4 Projection;
uniform mat4 View;
uniform mat4 Model;
uniform mat4 Model2;

varying vec3 Normal;
varying vec2 TexCoord;
varying vec3 Color;
varying vec3 VPos;
varying vec4 vEyeSpacePos;

void main(void)
{
	Color=color;
	Normal = normalize(vec3(View*Model*vec4(normal,0.0)));
    TexCoord = vec2(texCoords);
    vec4 vEyeSpacePosVertex = View*Model*vec4(position,1.0);
    vEyeSpacePos = vEyeSpacePosVertex;
    gl_Position = Projection*vEyeSpacePosVertex;
    VPos = gl_Position.xyz;
}
