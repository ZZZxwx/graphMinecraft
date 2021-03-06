#version 330 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec2 textCoord;
layout(location = 2) in vec3 normal;

out vec3 FragPos;
out vec2 TextCoord;
out vec3 FragNormal;
out vec4 FragPosLightSpace;
uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;
uniform mat4 lightSpaceMatrix;
void main()
{
	gl_Position = projection * view * model * vec4(position, 1.0);
	FragPos = vec3(model * vec4(position, 1.0)); // 在世界坐标系中指定片元位置
	TextCoord = textCoord;
	mat3 normalMatrix = mat3(transpose(inverse(model)));
	FragNormal = normalMatrix * normal; // 计算法向量经过模型变换后值
	FragPosLightSpace = lightSpaceMatrix * vec4(FragPos, 1.0);
}