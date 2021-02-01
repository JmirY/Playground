#version 330 core
in vec3 FragPos;
in vec3 Normal;
out vec4 FragColor;

uniform vec3 objectColor;
uniform vec3 viewPos;

void main()
{
  // /* light setting */
  // vec3 lightColor = vec3(1.0, 1.0, 1.0);
  // vec3 lightPos = vec3(0.0, 10.0, 0.0);
  
  // /* ambient lighting */
  // float ambientStrength = 0.3;
  // vec3 ambient = ambientStrength * lightColor;

  // /* diffuse lighting */
  // vec3 norm = normalize(Normal);
  // vec3 lightDir = normalize(lightPos - FragPos);
  // float diff = max(dot(norm, lightDir), 0.0);
  // vec3 diffuse = diff * lightColor;

  // /* specualr lighting */
  // float specularStrength = 0.5;
  // int shininess = 32;
  // vec3 viewDir = normalize(viewPos - FragPos);
  // vec3 reflectDir = reflect(-lightDir, norm);
  // float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
  // vec3 specular = specularStrength * spec * lightColor;

  // vec3 result = (ambient + diffuse + specular) * objectColor;
  // FragColor = vec4(result, 1.0);
  FragColor = vec4(objectColor, 1.0);
}