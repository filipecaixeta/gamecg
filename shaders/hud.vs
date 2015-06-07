#version 120

attribute vec3 position;
uniform mat4 Model;
varying vec2 TexCoord;

void main(void)
{
	vec4 pos = vec4(position,1.0);
	TexCoord = pos.xy;
	// pos.z=-pos.z;
	pos=Model*pos;
	// pos.xy*=0.25;
	pos.x+=0.8;
	pos.y-=0.7;
    gl_Position = pos;
}
