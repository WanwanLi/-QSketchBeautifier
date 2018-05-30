precision highp float;
varying vec3 vColor;
varying vec3 vNormal;
varying vec3 vPosition;
varying vec2 vTexcoord;
uniform float shininess;
uniform vec3 lightColor;
uniform vec3 eyePosition;
uniform vec3 lightPosition;
uniform vec3 decayVector;
uniform sampler2D texture;

float lightDecay(float distance)
{
	float  a=decayVector.x, b=decayVector.y, c=decayVector.z;
	return 1.0/(a+b*distance+c*distance*distance);
}
vec3 lightIntensity()
{
	vec3 N=-normalize(vNormal);
	vec3 V=eyePosition-vPosition;
	vec3 L=lightPosition-vPosition;
	if(dot(N, L)<=0.0)N=-N;
	float decay=lightDecay(length(V));
	decay*=lightDecay(length(L));
	V=normalize(V); L=normalize(L);
	vec3 R=normalize(reflect(-L, N));
	float specular=max(0.0, dot(V, R));
	float diffuse=max(0.0, dot(N, L));
	specular=pow(specular, shininess);
	specular=diffuse==0.0?0.0:specular;
	return vec3(diffuse, specular, decay);
}
void main()
{
	vec3 specularColor=lightColor, light=lightIntensity();
	vec3 color=vColor*light.x+specularColor*light.y;
	vec3 fColor=min(vColor+vec3(0.2), vec3(1.0));
	gl_FragColor=vec4(0.6*fColor+0.4*color, 1.0);
}
