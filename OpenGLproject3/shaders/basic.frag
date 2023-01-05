#version 410 core

in vec3 fPosition;
in vec3 fNormal;
in vec2 fTexCoords;
in vec4 fPosLightSpace;
in vec4 pos;

out vec4 fColor;

//matrices
uniform mat4 model;
uniform mat4 view;
uniform mat3 normalMatrix;
//lighting
uniform vec3 lightDir;
uniform vec3 lightColor;

uniform vec3 lightPosF1;
uniform vec3 lightPosF2;
uniform vec3 lightPosF3;
uniform vec3 lightPosF4;
uniform vec3 lightPosF5;
uniform vec3 spotlightPos1;
uniform vec3 spotlightPos2;
uniform vec3 spotlightDir1;
uniform vec3 spotlightDir2;
uniform vec3 spotlightColor;
uniform vec3 lightColorF;
// textures
uniform sampler2D diffuseTexture;
uniform sampler2D specularTexture;
uniform sampler2D shadowMap;

//components
vec3 ambient;
float ambientStrength = 0.7f;
vec3 diffuse;
vec3 specular;
float shininess = 32.0f;
float specularStrength = 1.0f;

vec3 ambient2, specular2, diffuse2;
float ambientStrength2 = 0.2f;
float specularStrength2 = 0.5f;
float shininess2 = 32.0f;

float Kc = 1.0f;
float Kl = 0.35f;
float Kq = 0.44f;

vec3 ambient3, specular3, diffuse3;
float ambientStrength3 = 0.2f;
float specularStrength3 = 0.5f;
float shininess3 = 32.0f;
float cutoff = 15.0f;

vec3 computeDirLight()
{
    //compute eye space coordinates
    vec4 fPosEye = view * model * vec4(fPosition, 1.0f);
    vec3 normalEye = normalize(normalMatrix * fNormal);

    //normalize light direction
    vec3 lightDirN = vec3(normalize(view * vec4(lightDir, 0.0f)));

    //compute view direction (in eye coordinates, the viewer is situated at the origin
    vec3 viewDir = normalize(-fPosEye.xyz);

    //compute ambient light
    ambient = ambientStrength * lightColor * vec3(texture(diffuseTexture, fTexCoords));

    //compute diffuse light
    diffuse = max(dot(normalEye, lightDirN), 0.0f) * lightColor * vec3(texture(diffuseTexture, fTexCoords));

    //compute specular light
    vec3 reflectDir = reflect(-lightDirN, normalEye);
    float specCoeff = pow(max(dot(viewDir, reflectDir), 0.0f), 32);
    specular = specularStrength * specCoeff * lightColor * vec3(texture(diffuseTexture, fTexCoords));
    return ambient + diffuse + specular;
}

vec3 computePointLight(vec3 lightpos)
{   
    //compute eye space coordinates
    vec3 normalEye = normalize(fNormal);

    //normalize light direction
    vec3 lightDirN = normalize(lightpos - pos.xyz);
    vec3 viewDir = normalize((-view * model * pos).xyz);
    //compute ambient light
    ambient2 = ambientStrength2 * lightColorF * vec3(texture(diffuseTexture, fTexCoords));

    //compute diffuse light
    float diff = max(dot(normalEye, lightDirN), 0.0f);
    diffuse2 = diff * lightColorF * vec3(texture(diffuseTexture, fTexCoords));
    //compute specular light
    vec3 reflectDir = reflect(-lightDirN, normalEye);
    float specCoeff = pow(max(dot(viewDir, reflectDir), 0.0f), shininess2);
    specular2 = specularStrength * specCoeff * lightColorF * vec3(texture(diffuseTexture, fTexCoords));
    
    float dist = length(lightpos-pos.xyz); //compute attenuation 
    float att = 1.0f / (Kc + Kl * dist + Kq * (dist * dist));

    ambient2  *= att;
    diffuse2  *= att;
    specular2 *= att;

    return ambient2 + diffuse2 + specular2;
}


float computeShadow()
{
	vec3 normalizedCoordinates = fPosLightSpace.xyz / fPosLightSpace.w;
	normalizedCoordinates *= 0.5;
	normalizedCoordinates += 0.5;
	float closestDepth = texture(shadowMap, normalizedCoordinates.xt).r;
	float currentDepth = normalizedCoordinates.z;
	float bias = 0.005f;
	float shadow = currentDepth - bias > closestDepth ? 1.0 : 0.0;
	if(currentDepth > 1.0f)
		shadow = 0.0f;
	return shadow;
}

float computeFog()
{
     vec4 fPosEye = view * model * vec4(fPosition, 1.0f);
     float fogDensity = 0.01f;
     float fragmentDistance = length(fPosEye);
     float fogFactor = exp(-pow(fragmentDistance * fogDensity, 2));

     return clamp(fogFactor, 0.0f, 1.0f);
}

void main() 
{
    float shadow = computeShadow();
    //compute final vertex color
    vec3 color = computeDirLight();
    vec3 lamp1 = computePointLight(lightPosF1);
    vec3 lamp2 = computePointLight(lightPosF2);
    vec3 lamp3 = computePointLight(lightPosF3);
    vec3 lamp4 = computePointLight(lightPosF4);
    vec3 lamp5 = computePointLight(lightPosF5);
    color += lamp1 + lamp2 + lamp3 + lamp4 + lamp5;
    float fogFactor = computeFog();
    vec4 fogColor = vec4(0.5f,0.5f,0.5f,1.0f);
    
    //vec3 color = min((ambient + (1.0f - shadow) * diffuse) * texture(diffuseTexture, fTexCoords).rgb + (1.0f - shadow) * specular * texture(specularTexture, fTexCoords).rgb, 1.0f);
    vec4 colorFromTexture = texture(diffuseTexture, fTexCoords); 
    if(colorFromTexture.a < 0.1) 
    {
        discard; 
        fColor = colorFromTexture;
    }
     else   
    //vec3 color = min((ambient + (1.0f - shadow) * diffuse) + (1.0f - shadow) * specular, 1.0f);
        fColor = mix(fogColor,vec4(color,1.0f),fogFactor);
        //fColor = vec4(color, 1.0f);
    
    
}
