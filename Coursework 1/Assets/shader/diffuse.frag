#version 430

struct Material {
    sampler2D diffuse;
    sampler2D specular;
    float     shininess;
};  

struct Light {
    vec3 position;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

in vec3 vertPos;  
in vec3 vertNormal;  
in vec2 texCoords;
  
out vec4 FragColour;
  
uniform vec3 viewPos;
uniform Material material;
uniform Light light;

void main()
{
	//Calculate the light direction vector
	vec3 L = normalize(light.position - vertPos);  
    
	//calculate Diffuse Light Intensity making sure it is not negative 
	//and is clamped 0 to 1  
	vec4 Id = vec4(light.diffuse,1.0) * max(dot(vertNormal,L), 0.0);
	Id = clamp(Id, 0.0, 1.0);     
	float n = 15; //Specular reflection exponent 

	//Calculate ambient, diffuse and specular light
	vec4 Ambient = vec4(light.ambient * vec3(texture(material.diffuse, texCoords)), 1.0f);
	vec4 Diffuse = Id * vec4(texture(material.diffuse, texCoords));
	vec4 Specular = vec4(light.specular,1.0) * vec4(texture(material.specular, texCoords)) * pow(max(dot(normalize(reflect(L, normalize(vertNormal))),normalize(vertPos)), 0.0), material.shininess);

	//Calculate the attenuation
	float Dist = length(light.position - vertPos); //Calculate the distance between the light and the vertex
	float Fatt = 1 - pow((Dist/50), 1);
	Fatt = clamp(Fatt, 0.0, 1.0);   

	//Multiply the Reflectivity by the Diffuse intensity
	FragColour = Ambient + Fatt * (Diffuse + Specular);
} 
