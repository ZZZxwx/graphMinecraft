#ifndef _SKYBOX_H_
#define _SKYBOX_H_
#include "model.h"
#include <vector>
#include <string>
#include "camera.h"
///
/// ��պл�����
/// ���Ի�����պ�
/// 
// ------------------------------------------------------------------
//��պеĵ㣬��һ��������
static float skyboxVertices[] = {
    // positions          
    -1.0f,  1.0f, -1.0f,
    -1.0f, -1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,
     1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,

    -1.0f, -1.0f,  1.0f,
    -1.0f, -1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f,  1.0f,
    -1.0f, -1.0f,  1.0f,

     1.0f, -1.0f, -1.0f,
     1.0f, -1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,

    -1.0f, -1.0f,  1.0f,
    -1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f, -1.0f,  1.0f,
    -1.0f, -1.0f,  1.0f,

    -1.0f,  1.0f, -1.0f,
     1.0f,  1.0f, -1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
    -1.0f,  1.0f,  1.0f,
    -1.0f,  1.0f, -1.0f,

    -1.0f, -1.0f, -1.0f,
    -1.0f, -1.0f,  1.0f,
     1.0f, -1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,
    -1.0f, -1.0f,  1.0f,
     1.0f, -1.0f,  1.0f
};
//��պ���
class skybox{
    GLuint program;                         //��պе���ɫ��
    vector<std::string> textures_faces{     //��պв�ͬ����ͼƬ��path
        "WaterSky/right.jpg",
        "WaterSky/left.jpg",
        "WaterSky/top.jpg",
        "WaterSky/bottom.jpg",
        "WaterSky/back.jpg",
        "WaterSky/front.jpg"
    };
    unsigned int cubemapTexture;            //��պ��������ͼid
    unsigned int skyboxVAO, skyboxVBO;      //VAO��VBO
public:
    skybox() {                              //��պг�ʼ��
        program=cubemapTexture=0;
        skyboxVAO=0, skyboxVBO=0;
    }

    //��պеĻ��ƺ���
    void skyBoxDraw(Camera& cam) {
        //������Ⱥ������Ի�����պ��������������
        glDepthFunc(GL_LEQUAL); 
        glUseProgram(program);              //ʹ����պ���ɫ��
        glUniformMatrix4fv(glGetUniformLocation(program, "projection"), 1, true, cam.getprojectMatrix());
        glUniformMatrix4fv(glGetUniformLocation(program, "view"), 1, true, cam.lookAt());
        //������պз���
        glBindVertexArray(skyboxVAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);
        glDepthFunc(GL_LESS); // set depth function back to default
    }

    //��պеĳ�ʼ��
    void init() {
        //���������(��������)
        glGenVertexArrays(1, &skyboxVAO);
        glGenBuffers(1, &skyboxVBO);
        glBindVertexArray(skyboxVAO);
        glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        cubemapTexture = loadCubemap();
    }
    
    //������պ�
    unsigned int loadCubemap() {
        //������պ���ɫ��
        program = InitShader("shader/vshader_skybox.glsl", "shader/fshader_skybox.glsl");
        unsigned int textureID;
        glGenTextures(1, &textureID);
        glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
        //������պ�ÿһ�������ͼƬ
        int width, height, nrChannels;
        for (unsigned int i = 0; i < textures_faces.size(); i++) {
            unsigned char* data = stbi_load(textures_faces[i].c_str(), &width, &height, &nrChannels, 0);
            if (data){
                glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                    0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data
                );
                stbi_image_free(data);
            }
            else {
                std::cout << "Cubemap texture failed to load at path: " << textures_faces[i] << std::endl;
                stbi_image_free(data);
            }
        }
        //������պе���������
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
        return textureID;
    }

};

#endif