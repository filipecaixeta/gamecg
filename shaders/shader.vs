#version 120

attribute vec3 position;

void main(void)
{
	vec3 temp=position;
	temp=temp*0.1;
    temp.y-=0.5;
    gl_Position = vec4(temp,1.0);
}
