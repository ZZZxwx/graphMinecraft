#ifndef _MESH_H_
#define _MESH_H_
#include "Angel.h"
#include <vector>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <string>   
#include <iostream>
using namespace std;
///
/// 模型MODEL的每一部分的绘制类
/// 
/// 

// 表示一个顶点属性
struct Vertex{
    vec3 position;  // 顶点位置
    vec2 texCoords; // 纹理坐标
    vec3 normal;  // 法向量
};

// 表示obj文件中一个顶点的位置、纹理坐标和法向量 索引
struct VertexCombineIndex{
	GLuint posIndex;
	GLuint textCoordIndex;
	GLuint normIndex;
};

// 表示一个Texture
struct Texture{
	GLuint id;
	aiTextureType type;
	std::string path;
};

class Mesh{
public:
	void draw(const GLuint programID)const;// 绘制Mesh
    Mesh(const std::vector<Vertex>& vertData,
        GLint textureId); // 构造一个Mesh
	Mesh();
	Mesh(const std::vector<Vertex>& vertData,
		const std::vector<Texture>& textures,
		const std::vector<GLuint>& indices);
	void setData(const std::vector<Vertex>& vertData,
		const std::vector<Texture>& textures,
		const std::vector<GLuint>& indices);
	void final() const;

private:
    std::vector<Vertex> vertData;// 顶点数据
	std::vector<GLuint> indices;
	std::vector<Texture> textures;
    GLuint VAOId, VBOId , EBOId; // 缓存对象
    GLint textureId; // 纹理对象id
    void setupMesh();  // 建立VAO,VBO等缓冲区

};

void Mesh::setupMesh()  // 建立VAO,VBO等缓冲区
{
	glGenVertexArrays(1, &this->VAOId);
	glGenBuffers(1, &this->VBOId);
	glGenBuffers(1, &this->EBOId);

	glBindVertexArray(this->VAOId);
	glBindBuffer(GL_ARRAY_BUFFER, this->VBOId);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * this->vertData.size(),
		&this->vertData[0], GL_STATIC_DRAW);
	// 顶点位置属性
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,
		sizeof(Vertex), (GLvoid*)0);
	glEnableVertexAttribArray(0);
	// 顶点纹理坐标
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE,
		sizeof(Vertex), (GLvoid*)(3 * sizeof(GL_FLOAT)));
	glEnableVertexAttribArray(1);
	// 顶点法向量属性
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE,
		sizeof(Vertex), (GLvoid*)(5 * sizeof(GL_FLOAT)));
	glEnableVertexAttribArray(2);
	// 索引数据
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->EBOId);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * this->indices.size(),
		&this->indices[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void Mesh::draw(const GLuint programID) const// 绘制Mesh
{
	if (VAOId == 0
		|| VBOId == 0
		|| EBOId == 0)
	{
		return;
	}
	glUseProgram(programID);
	glBindVertexArray(this->VAOId);
	int diffuseCnt = 0, specularCnt = 0, texUnitCnt = 0;
	//读入纹理
	for (std::vector<Texture>::const_iterator it = this->textures.begin();
		this->textures.end() != it; ++it)
	{
		switch (it->type)
		{
		case aiTextureType_DIFFUSE:
		{
			glActiveTexture(GL_TEXTURE0 + texUnitCnt);
			glBindTexture(GL_TEXTURE_2D, it->id);
			std::stringstream samplerNameStr;
			samplerNameStr << "texture_diffuse" << diffuseCnt++;
			glUniform1i(glGetUniformLocation(programID,
				samplerNameStr.str().c_str()), texUnitCnt++);
		}
		break;
		case aiTextureType_SPECULAR:
		{
			glActiveTexture(GL_TEXTURE0 + texUnitCnt);
			glBindTexture(GL_TEXTURE_2D, it->id);
			std::stringstream samplerNameStr;
			samplerNameStr << "texture_specular" << specularCnt++;
			glUniform1i(glGetUniformLocation(programID,
				samplerNameStr.str().c_str()), texUnitCnt++);
		}
		break;
		default:
			std::cerr << "Warning::Mesh::draw, texture type" << it->type
				<< " current not supported." << std::endl;
			break;
		}
	}

	//绘制
	glDrawElements(GL_TRIANGLES, this->indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
	glUseProgram(0);
}
//----------------------------------------------------------------------------------------
//mesh的各种构造重载
//mesh构造
Mesh::Mesh(const std::vector<Vertex>& vertData, GLint textureId) // 构造一个Mesh
{
    this->vertData = vertData;
    this->textureId = textureId;
    this->setupMesh();

}
Mesh::Mesh() :VAOId(0), VBOId(0), EBOId(0) {}
Mesh::Mesh(const std::vector<Vertex>& vertData,
	const std::vector<Texture>& textures,
	const std::vector<GLuint>& indices) :VAOId(0), VBOId(0), EBOId(0) // 构造一个Mesh
{
	setData(vertData, textures, indices);
}
void Mesh::setData(const std::vector<Vertex>& vertData,
	const std::vector<Texture>& textures,
	const std::vector<GLuint>& indices)
{
	this->vertData = vertData;
	this->indices = indices;
	this->textures = textures;
	if (!vertData.empty() && !indices.empty())
	{
		this->setupMesh();
	}
}

//回收空间
void Mesh::final() const
{
	glDeleteVertexArrays(1, &this->VAOId);
	glDeleteBuffers(1, &this->VBOId);
	glDeleteBuffers(1, &this->EBOId);
}

//加载obj文件用类
class ObjLoader
{
public:
	static bool loadFromFile(const char* path,
		std::vector<Vertex>& vertData)
	{

		std::vector<VertexCombineIndex> vertComIndices;
		std::vector<vec3> temp_vertices;
		std::vector<vec2> temp_textCoords;
		std::vector<vec3> temp_normals;

		std::ifstream file(path);
		if (!file)
		{
			std::cerr << "Error::ObjLoader, could not open obj file:"
				<< path << " for reading." << std::endl;
			return false;
		}
		std::string line;
		while (getline(file, line))
		{
			if (line.substr(0, 2) == "vt") // 顶点纹理坐标数据
			{
				std::istringstream s(line.substr(2));
				vec2 v;
				s >> v.x;
				s >> v.y;
				v.y = -v.y;  // 注意这里加载的dds纹理 要对y进行反转
				temp_textCoords.push_back(v);
			}
			else if (line.substr(0, 2) == "vn") // 顶点法向量数据
			{
				std::istringstream s(line.substr(2));
				vec3 v;
				s >> v.x; s >> v.y; s >> v.z;
				temp_normals.push_back(v);
			}
			else if (line.substr(0, 1) == "v") // 顶点位置数据
			{
				std::istringstream s(line.substr(2));
				vec3 v;
				s >> v.x; s >> v.y; s >> v.z;
				temp_vertices.push_back(v);
			}
			else if (line.substr(0, 1) == "f") // 面数据
			{
				std::istringstream vtns(line.substr(2));
				std::string vtn;
				while (vtns >> vtn) // 处理一行中多个顶点属性
				{
					VertexCombineIndex vertComIndex;
					std::replace(vtn.begin(), vtn.end(), '/', ' ');
					std::istringstream ivtn(vtn);
					if (vtn.find("  ") != std::string::npos) // 没有纹理数据
					{
						std::cerr << "Error:ObjLoader, no texture data found within file:"
							<< path << std::endl;
						return false;
					}
					ivtn >> vertComIndex.posIndex
						>> vertComIndex.textCoordIndex
						>> vertComIndex.normIndex;

					vertComIndex.posIndex--;
					vertComIndex.textCoordIndex--;
					vertComIndex.normIndex--;
					vertComIndices.push_back(vertComIndex);
				}
			}
			else if (line[0] == '#'){ // 注释忽略
			
			}
		}
		//根据面的数据设置顶点vector
		for (int i = 0; i < vertComIndices.size(); ++i)
		{
			Vertex vert;
			VertexCombineIndex comIndex = vertComIndices[i];

			vert.position = temp_vertices[comIndex.posIndex];
			vert.texCoords = temp_textCoords[comIndex.textCoordIndex];
			vert.normal = temp_normals[comIndex.normIndex];

			vertData.push_back(vert);
		}

		return true;
	}
};
#endif