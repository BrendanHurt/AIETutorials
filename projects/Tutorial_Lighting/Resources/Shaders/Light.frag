#version 330
in vec3 position;
in vec3 normal;

uniform vec3 LightAmbient;

uniform vec3 LightDirection;
uniform vec3 LightColor;
uniform vec3 LightSpecular;

uniform vec3 cameraPosition;

void main()
{
	vec3 N = normalize(normal);
	vec3 L = normalize(-LightDirection);
	
	vec3 R = reflect(-L, N);
	vec3 E = normalize(cameraPosition - position);
	
	//diffuse
	float d = max(0, dot(N, L));
	vec3 diffuse = LightColor * d;
	
	vec3 ambient = LightAmbient;
	
	float S = pow(max(0, dot(E, R)), 128);
	vec3 specular = LightSpecular * S;
	
	gl_FragColor = vec4(ambient + diffuse + specular, 1);
}