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

// 光源属性结构体
struct LightAttr
{
	vec3 position;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;

	float constant;	// 衰减常数
	float linear;   // 衰减一次系数
	float quadratic; // 衰减二次系数
};
uniform LightAttr light;
uniform vec3 viewPos;

out vec4 color;
float ShadowCalculation(vec4 fragPosLightSpace)
{
    //先进行透视除法
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    //将坐标映射到[0,1]之中
    projCoords = projCoords * 0.5 + 0.5;
    //得到离光的射线中最近的一个点的深度缓存信息
    float closestDepth = texture(shadowMap, projCoords.xy).r; 
    //得到当前点在光视角下的深度信息
    float currentDepth = projCoords.z;
    //检查该点是否在阴影中
	vec3	lightDir = normalize(light.position - FragPos);
	//允许误差
	float bias = max(0.05 * (1.0 - dot(FragNormal, lightDir)), 0.006);

	float shadow = 0.0;
	//这个textureSize返回一个给定采样器纹理的0级mipmap的vec2类型的宽和高。
	//用1除以它返回一个单独纹理像素的大小，我们用以对纹理坐标进行偏移，确保每个新样本，来自不同的深度值。
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
	// 环境光成分
	vec3	ambient = light.ambient * vec3(texture(texture_diffuse0, TextCoord));

	// 漫反射光成分 此时需要光线方向为指向光源
	vec3	lightDir = normalize(light.position - FragPos);
	vec3	normal = normalize(FragNormal);
	float	diffFactor = max(dot(lightDir, normal), 0.0);
	vec3	diffuse = diffFactor * light.diffuse * vec3(texture(texture_diffuse0, TextCoord));

	// 镜面反射成分 此时需要光线方向为由光源指出
	float	specularStrength = 0.5f;
	vec3	reflectDir = normalize(reflect(-lightDir, normal));
	vec3	viewDir = normalize(viewPos - FragPos);
	float	specFactor = pow(max(dot(reflectDir, viewDir), 0.0), 10.0f);
	vec3	specular = specFactor * light.specular * vec3(texture(texture_specular0, TextCoord));

	// 计算衰减因子
	float distance = length(light.position - FragPos); // 在世界坐标系中计算距离
	float attenuation = 1.0f / (light.constant 
			+ light.linear * distance
			+ light.quadratic * distance * distance);
	//计算阴影映射
	float shadow = ShadowCalculation(FragPosLightSpace); 
	vec3	result = (ambient + (1.0-shadow)*(diffuse + specular)) * attenuation;
	return result;
}
void main()
{
	color = vec4(culLightColor(),1.0);
}

