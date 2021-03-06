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

void main(void)
{
	mat4 model=Model*Model2;
	Color=color;
	Normal = normalize(vec3(View*model*vec4(normal,0.0)));
    TexCoord = vec2(texCoords);
    VPos = vec4(View*model*vec4(position,1.0)).xyz;
    gl_Position = Projection*vec4(VPos,1.0);
}
