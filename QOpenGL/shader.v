attribute vec3 color;
attribute vec3 normal;
attribute vec3 position;
attribute vec2 texcoord;
varying vec3 vColor;
varying vec3 vNormal;
varying vec3 vPosition;
varying vec2 vTexcoord;
uniform mat4 viewMatrix;
uniform mat4 modelMatrix;
uniform mat3 normalMatrix;
uniform mat4 projectionMatrix;
 
void main()
{
	vNormal=normalMatrix*normal; vTexcoord=texcoord; 
	vPosition=(modelMatrix*vec4(position, 1)).xyz; vColor=color;
	gl_Position=projectionMatrix*viewMatrix*vec4(vPosition, 1.0);
}
