#version 330 core

uniform sampler2D texture_diffuse0;
uniform sampler2D texture_diffuse1;
uniform sampler2D texture_diffuse2;
uniform sampler2D texture_specular0;
uniform sampler2D texture_specular1;
uniform sampler2D texture_specular2;
uniform sampler2D shadowMap;

in vec4 FragPosLightSpace;
in vec3 FragPos;
in vec2 TextCoord;
in vec3 FragNormal;

// ��Դ���Խṹ��
struct LightAttr
{
	vec3 position;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;

	float constant;	// ˥������
	float linear;   // ˥��һ��ϵ��
	float quadratic; // ˥������ϵ��
};
uniform LightAttr light;
uniform vec3 viewPos;

out vec4 color;
float ShadowCalculation(vec4 fragPosLightSpace)
{
    //�Ƚ���͸�ӳ���
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    //������ӳ�䵽[0,1]֮��
    projCoords = projCoords * 0.5 + 0.5;
    //�õ����������������һ�������Ȼ�����Ϣ
    float closestDepth = texture(shadowMap, projCoords.xy).r; 
    //�õ���ǰ���ڹ��ӽ��µ������Ϣ
    float currentDepth = projCoords.z;
    //���õ��Ƿ�����Ӱ��
	vec3	lightDir = normalize(light.position - FragPos);
	//�������
	float bias = max(0.05 * (1.0 - dot(FragNormal, lightDir)), 0.006);

	float shadow = 0.0;
	//���textureSize����һ�����������������0��mipmap��vec2���͵Ŀ�͸ߡ�
	//��1����������һ�������������صĴ�С���������Զ������������ƫ�ƣ�ȷ��ÿ�������������Բ�ͬ�����ֵ��
	vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
	for(int x = -1; x <= 1; ++x){
		for(int y = -1; y <= 1; ++y){
        float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r; 
        shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;        
		}    
	}
	shadow /= 9.0;

    return shadow;
}
vec3 culLightColor(){
	// ������ɷ�
	vec3	ambient = light.ambient * vec3(texture(texture_diffuse0, TextCoord));

	// �������ɷ� ��ʱ��Ҫ���߷���Ϊָ���Դ
	vec3	lightDir = normalize(light.position - FragPos);
	vec3	normal = normalize(FragNormal);
	float	diffFactor = max(dot(lightDir, normal), 0.0);
	vec3	diffuse = diffFactor * light.diffuse * vec3(texture(texture_diffuse0, TextCoord));

	// ���淴��ɷ� ��ʱ��Ҫ���߷���Ϊ�ɹ�Դָ��
	float	specularStrength = 0.5f;
	vec3	reflectDir = normalize(reflect(-lightDir, normal));
	vec3	viewDir = normalize(viewPos - FragPos);
	float	specFactor = pow(max(dot(reflectDir, viewDir), 0.0), 10.0f);
	vec3	specular = specFactor * light.specular * vec3(texture(texture_specular0, TextCoord));

	// ����˥������
	float distance = length(light.position - FragPos); // ����������ϵ�м������
	float attenuation = 1.0f / (light.constant 
			+ light.linear * distance
			+ light.quadratic * distance * distance);
	//������Ӱӳ��
	float shadow = ShadowCalculation(FragPosLightSpace); 
	vec3	result = (ambient + (1.0-shadow)*(diffuse + specular)) * attenuation;
	return result;
}
void main()
{
	color = vec4(culLightColor(),1.0);
}

