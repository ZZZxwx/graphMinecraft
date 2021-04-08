
#ifndef _DEPMANSHADOW_H_
#define _DEPMANSHADOW_H_
#include "Angel.h"
#include "camera.h"
/// 
/// 阴影映射类，用于绘制阴影映射
/// 
/// 
extern int windows_width, windows_height;
class depmapShadow
{
    Camera cam;                 
    unsigned int depthMapFBO;
public:
    GLuint programID;               //阴影映射用着色器
    vec3 lightPos;                  //光源位置
    mat4 lightProjection, lightView;//在光源处摄像的投影矩阵和view矩阵
    mat4 lightSpaceMatrix;          //将世界坐标变换到光源坐标
    GLuint debugid;                 //阴影debug用着色器
    //光源处射线的投影矩阵的参数
    float near_plane = 1.0f, far_plane = 20.0f;
    //深度贴图的id
    unsigned int depthMap;
    //深度贴图的解析度
    const unsigned int SHADOW_WIDTH =2* windows_width, SHADOW_HEIGHT = 2* windows_height;

    //初始化
    void init() {
        //读入阴影着色器
        programID = InitShader("shader/vshader_shadow.glsl", "shader/fshader_shadow.glsl");
        glGenFramebuffers(1, &depthMapFBO);                 //生成深度缓存的id
                                                            
        glGenTextures(1, &depthMap);                        //创造深度贴图并且设置相关参数
        glBindTexture(GL_TEXTURE_2D, depthMap);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
            
        glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);     //绑定深度贴图
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
        glDrawBuffer(GL_NONE);
        glReadBuffer(GL_NONE);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        lightPos = vec3(-5,10,-5);                          //设置光源相关参数
        lightProjection = Ortho(-20.0f, 20.0f, -20.0f, 20.0f, near_plane, far_plane);
        lightView = cam.lookAt(lightPos, vec3(0.0f), vec3(0.0, 1.0, 0.0));
        lightSpaceMatrix = lightProjection * lightView;
                                                            //阴影debug用着色器
        debugid = InitShader("shader/vdebugshadow.glsl", "shader/fdebugshadow.glsl");
    }

    //阴影的绘制准备
    void draw(){
        glUseProgram(programID);                            //使用阴影绘制的着色器并传入相关矩阵
        glUniformMatrix4fv(glGetUniformLocation(programID, "lightSpaceMatrix"), 1, true, lightSpaceMatrix);
        glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
        glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
        glClear(GL_DEPTH_BUFFER_BIT);
       
	}
    
    //——————————————————————————————————————————————DEBUG————————————————————————————————————————————————————————
    //debug用
    void debug() {
        glUseProgram(debugid);
        glUniform1f(glGetUniformLocation(debugid,"near_plane"), near_plane);
        glUniform1f(glGetUniformLocation(debugid, "far_plane"), far_plane);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, depthMap);
        renderQuad();
    }

    //debug用绘制深度贴图
    unsigned int quadVAO = 0;
    unsigned int quadVBO;
    void renderQuad()
    {
        if (quadVAO == 0)
        {
            float quadVertices[] = {
                // positions        // texture Coords
                -1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
                -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
                 1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
                 1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
            };
            // setup plane VAO
            glGenVertexArrays(1, &quadVAO);
            glGenBuffers(1, &quadVBO);
            glBindVertexArray(quadVAO);
            glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
            glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
            glEnableVertexAttribArray(0);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
            glEnableVertexAttribArray(1);
            glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
        }
        glBindVertexArray(quadVAO);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        glBindVertexArray(0);
    }
};

#endif