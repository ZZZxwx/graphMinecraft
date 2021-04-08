#ifndef _SKYBOX_H_
#define _SKYBOX_H_
#include "model.h"
#include <vector>
#include <string>
#include "camera.h"
///
/// 天空盒绘制类
/// 用以绘制天空盒
/// 
// ------------------------------------------------------------------
//天空盒的点，即一个正方体
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
//天空盒类
class skybox{
    GLuint program;                         //天空盒的着色器
    vector<std::string> textures_faces{     //天空盒不同区域图片的path
        "WaterSky/right.jpg",
        "WaterSky/left.jpg",
        "WaterSky/top.jpg",
        "WaterSky/bottom.jpg",
        "WaterSky/back.jpg",
        "WaterSky/front.jpg"
    };
    unsigned int cubemapTexture;            //天空盒捆绑的贴图id
    unsigned int skyboxVAO, skyboxVBO;      //VAO，VBO
public:
    skybox() {                              //天空盒初始化
        program=cubemapTexture=0;
        skyboxVAO=0, skyboxVBO=0;
    }

    //天空盒的绘制函数
    void skyBoxDraw(Camera& cam) {
        //设置深度函数，以绘制天空盒于所有物体后面
        glDepthFunc(GL_LEQUAL); 
        glUseProgram(program);              //使用天空盒着色器
        glUniformMatrix4fv(glGetUniformLocation(program, "projection"), 1, true, cam.getprojectMatrix());
        glUniformMatrix4fv(glGetUniformLocation(program, "view"), 1, true, cam.lookAt());
        //绘制天空盒方块
        glBindVertexArray(skyboxVAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);
        glDepthFunc(GL_LESS); // set depth function back to default
    }

    //天空盒的初始化
    void init() {
        //绑定相关数据(即正方体)
        glGenVertexArrays(1, &skyboxVAO);
        glGenBuffers(1, &skyboxVBO);
        glBindVertexArray(skyboxVAO);
        glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        cubemapTexture = loadCubemap();
    }
    
    //加载天空盒
    unsigned int loadCubemap() {
        //加载天空盒着色器
        program = InitShader("shader/vshader_skybox.glsl", "shader/fshader_skybox.glsl");
        unsigned int textureID;
        glGenTextures(1, &textureID);
        glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
        //加载天空盒每一面的纹理图片
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
        //设置天空盒的纹理属性
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
        return textureID;
    }

};

#endif