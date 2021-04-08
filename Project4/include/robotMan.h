/// ʷ�ٷ�Ļ�����غ�������ҪΪ4��Ĳ㼶ģ�ͣ�������Կ�ʵ�鱨��
/// �ṹΪ 
/// body(
///     HEAD,
///     RightUpperArm(
///         RightLowerArm(
///             HandKeep)),
///     LeftUpperArm(
///         LeftLowerArm),
///     RightUpperLeg(
///         RightLowerLeg),
///     LeftUpperLeg(
///         LeftLowerLeg),
///     funSword)
/// 
#ifndef _ROBOTMAN_H_
#define _ROBOTMAN_H_
#include <vector>
#include "Angel.h"
#include "model.h"
#include <math.h>
#include "cubedraw.h"
#include <algorithm>
using namespace std;
//��ͬ��λ��ģ��λ��
static vector<string>path{
    "resource/people/arm/cube.obj",
    "resource/people/body/cube.obj",
    "resource/people/head/cube.obj",
    "resource/people/leg/cube.obj"
};
enum {
    arm,
    body,
    head,
    leg
};
extern cubedraw cubeDraw;
//----------------------------------------------------------------------------
//ʷ�ٷ����Ϊ1.8�����Ϊ1
//hand 12/4/4; leg 12/4/4 ; head 8/8/8 ;body 12/8/4
//total height 32 total width 4&8
const static float TORSO_HEIGHT = 0.6;
const static float TORSO_WIDTH = 0.4;
const static float TORSO_WIDTH2 = 0.2;
const static float UPPER_ARM_HEIGHT = 0.3;
const static float UPPER_ARM_WIDTH = 0.2;
const static float LOWER_ARM_HEIGHT = 0.4;
const static float LOWER_ARM_WIDTH = 0.2;
const static float UPPER_LEG_WIDTH = 0.2;
const static float UPPER_LEG_HEIGHT = 0.4;
const static float LOWER_LEG_HEIGHT = 0.4;
const static float LOWER_LEG_WIDTH = 0.2;
const static float HEAD_HEIGHT = 0.4;
const static float HEAD_WIDTH = 0.4;
const static float HANDKEEP_SIZE = 0.2;
const static float FUN_SIZE = 0.02;
static double pi = acos(-1);
//��ͬ��λ����ת�Ƕȱ���
static enum {     
    bodyThlta,          //������ת
    bodyoffset,         //���������Ų��
    funthlta,           //����������ת
    dz,                 //���Ķ���
    thetaHeadY,         //ͷ��������ת
    thetaHeadX,         //ͷ��������ת
    handkeepThlta,      //���ֳ����������ת
    leftHandThlta,      //���ֵ�ǰ��ڶ�
    leftUpperArmThltaZ, //���ֵ����Ұڶ�
    leftlowerArmThlta,  //�������ǰ��ڶ�
    rightHandThlta,     //���ֵ�ǰ��ڶ�
    rightUpperArmThltaZ,//���ֵ����Ұڶ�
    rightlowerArmThlta, //�������ǰ��ڶ�
    rightUpperLegthltaZ,//�ҽŵ����Ұڶ�
    rightLoweLegthlta,  //�ҽ�������Ұڶ�
    leftUpperLegthltaZ, //��ŵ����Ұڶ�
    leftLoweLegthlta,   //���������Ұڶ�
    LegThlta,           //�ŵ�ǰ��ڶ�
    
};
static int enumpartNum = 18;

class robotMan
{
    //���������˵�ģ��
    Model rbman[4];
public:
    vec3 pos = vec3(1, 0, 2);                           //��������λ��
    float thlta[30];                                    //��ͬ���ֵ���ת��
    GLuint program;                                     //��ɫ��

    /// <summary>
    /// ��ʼ����������ʼ��ģ�Ͳ������������ƫ����
    /// </summary>
    void init() {                                       
        for (int i = 0; i < 4; i++) {
            if (!rbman[i].loadModel(path[i])) {
                cout << "error" << endl;
            }
        }
        thlta[bodyoffset] = 0.7;
    }

    float headlimit = 100;           // ͷ������ת���ƣ���ֹͷ��180�ȴ�ת��
    //---------------------------------------------------------------------
    //���ƺ�������ʼ����
    void draw(GLuint programID,float yaw,float pitch) {
        program = programID;
        thlta[thetaHeadX] = -pitch;
        thlta[thetaHeadY] = 90-yaw- thlta[bodyThlta];
        if (thlta[thetaHeadY] > headlimit) {
            thlta[thetaHeadY] = headlimit;
        }
        else if (thlta[thetaHeadY] <  -headlimit) {
            thlta[thetaHeadY] =  -headlimit;
        }
        if (thlta[thetaHeadX] >headlimit/2) {
            thlta[thetaHeadX] = headlimit / 2;
        }
        else if (thlta[thetaHeadX]< -headlimit / 2) {
            thlta[thetaHeadX] = -headlimit / 2;
        }
        mat4 d = Translate(pos.x, pos.y+ thlta[bodyoffset], pos.z);
        mat4 model_view = d;//���ɱ任����
        torso(model_view);//���ɻ���
    }
    float armOffset = 0;
    //��������
    int isfun = 0;
    void torso(mat4 model_view)
    {
        model_view = model_view * RotateY(thlta[bodyThlta]);
        mat4 instance = (Translate(0.0, 0.5 * TORSO_HEIGHT, 0.0) *
            Scale(TORSO_WIDTH, TORSO_HEIGHT, TORSO_WIDTH2));//���ڵ�ֲ��任����
        glUseProgram(program);
        glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, true, model_view*instance);
        rbman[body].draw(program);

        headdraw(model_view * Translate(0.0, TORSO_HEIGHT, 0.0));
        left_upper_arm(model_view * Translate(+0.5 * (TORSO_WIDTH + UPPER_ARM_WIDTH - armOffset), TORSO_HEIGHT, 0));
        right_upper_arm(model_view * Translate(-0.5 * (TORSO_WIDTH + UPPER_ARM_WIDTH - armOffset), TORSO_HEIGHT, 0));
        left_upper_leg(model_view * Translate(-0.25 * TORSO_WIDTH, 0.1, 0));
        right_upper_leg(model_view * Translate(0.25 * TORSO_WIDTH, 0.1, 0));
        if(isfun)fun(model_view * Translate(0.25 * TORSO_WIDTH, 1, 0));
    }
    float tx=0, ty=0, tz=0;
    void fun(mat4 model_view) {
        model_view = model_view * RotateX(thlta[funthlta]) * RotateZ(90+thlta[dz]) *Translate(min(1.0f,max(-1.0f,thlta[dz])),0,0.75) * RotateY(-45);// ;//* RotateX(thlta[funthlta])
        mat4 instance = (Scale(FUN_SIZE, FUN_SIZE, FUN_SIZE));//*;//���ڵ�ֲ��任����
        glUseProgram(program);
        glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_TRUE, model_view * instance);//���ڵ����*���ڵ�ֲ��任����
        cubeDraw.objModel[5].draw(program);
    }
    void headdraw(mat4 model_view)
    {
        model_view = model_view * RotateY(thlta[thetaHeadY]) * RotateX(thlta[thetaHeadX]);
        mat4 instance = (Translate(0.0, 0.5 * HEAD_HEIGHT, 0.0) *
            Scale(HEAD_WIDTH, HEAD_HEIGHT, HEAD_WIDTH));//���ڵ�ֲ��任����
        glUseProgram(program);
        glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_TRUE, model_view * instance);//���ڵ����*���ڵ�ֲ��任����
        rbman[head].draw(program);
    }
    void left_upper_arm(mat4 model_view) {
        model_view = model_view * RotateX(180+ thlta[leftHandThlta]) * RotateZ(thlta[leftUpperArmThltaZ]);
        mat4 instance = (Translate(0.0, 0.5 * UPPER_ARM_HEIGHT, 0.0) *
            Scale(UPPER_ARM_WIDTH, UPPER_ARM_HEIGHT, UPPER_ARM_WIDTH)*RotateZ(180));//���ڵ�ֲ��任����
        glUseProgram(program);
        glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_TRUE, model_view * instance);//���ڵ����*���ڵ�ֲ��任����
        rbman[arm].draw(program);
        left_lower_arm(model_view * Translate(0.0, UPPER_ARM_HEIGHT, 0.0));
    }

    void left_lower_arm(mat4 model_view) {
        model_view = model_view * RotateZ(thlta[leftlowerArmThlta]);
        mat4 instance = (Translate(0.0, 0.5 * LOWER_ARM_HEIGHT, 0.0) *
            Scale(LOWER_ARM_WIDTH, LOWER_ARM_HEIGHT, LOWER_ARM_WIDTH));//���ڵ�ֲ��任����
        glUseProgram(program);
        glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_TRUE, model_view * instance);//���ڵ����*���ڵ�ֲ��任����
        rbman[arm].draw(program);
    }
    void right_upper_arm(mat4 model_view){
        model_view = model_view * RotateX(180 + thlta[rightHandThlta])*RotateZ(thlta[rightUpperArmThltaZ]);
        mat4 instance = (Translate(0.0, 0.5 * UPPER_ARM_HEIGHT, 0.0) *
            Scale(UPPER_ARM_WIDTH, UPPER_ARM_HEIGHT, UPPER_ARM_WIDTH) * RotateZ(180));//���ڵ�ֲ��任����
        glUseProgram(program);
        glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_TRUE, model_view * instance);//���ڵ����*���ڵ�ֲ��任����
        rbman[arm].draw(program);
        right_lower_arm(model_view * Translate(0.0, UPPER_ARM_HEIGHT, 0.0));
    }

    void right_lower_arm(mat4 model_view){
        model_view = model_view * RotateZ(thlta[rightlowerArmThlta]);
        mat4 instance = (Translate(0.0, 0.5 * LOWER_ARM_HEIGHT, 0.0) *
            Scale(LOWER_ARM_WIDTH, LOWER_ARM_HEIGHT, LOWER_ARM_WIDTH));//���ڵ�ֲ��任����
        glUseProgram(program);
        glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_TRUE, model_view * instance);//���ڵ����*���ڵ�ֲ��任����
        rbman[arm].draw(program);
        if (handkeepNo != 0) {
            hand_keep(model_view * Translate(0.0, LOWER_ARM_HEIGHT- 0.5*HANDKEEP_SIZE, -1.2*HANDKEEP_SIZE));    
        }
    }
    int handkeepNo = 1;
    void
        hand_keep(mat4 model_view)
    {
        //model_view = model_view * RotateX(theta[RightLowerArm]);
        mat4 instance = (Translate(0.0, 0.5 * HANDKEEP_SIZE, 0.0) *
            Scale(0.8*HANDKEEP_SIZE, 0.8 * HANDKEEP_SIZE, 0.8 * HANDKEEP_SIZE)*RotateZ(180)*RotateY(thlta[handkeepThlta])*RotateZ(thlta[handkeepThlta]));//���ڵ�ֲ��任����
        glUseProgram(program);
        glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_TRUE, model_view * instance);//���ڵ����*���ڵ�ֲ��任����
        cubeDraw.objModel[handkeepNo].draw(program);
    }
    void left_upper_leg(mat4 model_view){
        model_view = model_view * RotateX(180 - thlta[LegThlta])*RotateZ(thlta[leftUpperLegthltaZ]);
        mat4 instance = (Translate(0.0, 0.5 * UPPER_LEG_HEIGHT, 0.0) *
            Scale(UPPER_LEG_WIDTH, UPPER_LEG_HEIGHT, UPPER_LEG_WIDTH) * RotateZ(180));//���ڵ�ֲ��任����
        glUseProgram(program);
        glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_TRUE, model_view * instance);//���ڵ����*���ڵ�ֲ��任����
        rbman[leg].draw(program);
        left_lower_leg(model_view * Translate(0.0, UPPER_LEG_HEIGHT, 0.0));
    }

    void left_lower_leg(mat4 model_view){
        model_view = model_view * RotateZ(thlta[leftLoweLegthlta]);
        mat4 instance = (Translate(0.0, 0.5 * LOWER_LEG_HEIGHT, 0.0) *
            Scale(LOWER_LEG_WIDTH, LOWER_LEG_HEIGHT, LOWER_LEG_WIDTH) );//���ڵ�ֲ��任����
        glUseProgram(program);
        glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_TRUE, model_view * instance);//���ڵ����*���ڵ�ֲ��任����
        rbman[leg].draw(program);
    }
    void
        right_upper_leg(mat4 model_view)
    {
        model_view = model_view * RotateX(180 + thlta[LegThlta])*RotateZ(thlta[rightUpperLegthltaZ]);
        mat4 instance = (Translate(0.0, 0.5 * UPPER_LEG_HEIGHT, 0.0) *
            Scale(UPPER_LEG_WIDTH, UPPER_LEG_HEIGHT, UPPER_LEG_WIDTH) * RotateZ(180));//���ڵ�ֲ��任����
        glUseProgram(program);
        glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_TRUE, model_view * instance);//���ڵ����*���ڵ�ֲ��任����
        rbman[leg].draw(program);
        right_lower_leg(model_view * Translate(0.0, UPPER_LEG_HEIGHT, 0.0));
    }

    void right_lower_leg(mat4 model_view){
        model_view = model_view * RotateZ(thlta[rightLoweLegthlta]);
        mat4 instance = (Translate(0.0, 0.5 * LOWER_LEG_HEIGHT, 0.0) *
            Scale(LOWER_LEG_WIDTH, LOWER_LEG_HEIGHT, LOWER_LEG_WIDTH) );//���ڵ�ֲ��任����
        glUseProgram(program);
        glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_TRUE, model_view * instance);//���ڵ����*���ڵ�ֲ��任����
        rbman[leg].draw(program);
    }
    float hspeed = 5;
    void shand() {
        thlta[leftHandThlta] += hspeed;
        thlta[rightHandThlta] -= hspeed;
        thlta[LegThlta] += hspeed/2;
        if (thlta[leftHandThlta] >= 60 || thlta[leftHandThlta]<= -60){
            hspeed = -hspeed;
        }
        
    }

    void jump() {
        pos.y += 0.2;
        thlta[LegThlta] += hspeed ;
        thlta[leftHandThlta] += hspeed;
        thlta[rightHandThlta] -= hspeed;
        if (thlta[LegThlta] >= 60 || thlta[LegThlta] <= -60) {
            hspeed = -hspeed;
        }
    }

    float toZeroSpeed = 5;
    void tozero(float &v) {
        if (v>0) {
            v -= min(v, toZeroSpeed);
        }
        else if (v < 0) {
            v += min(-v, toZeroSpeed);
        }
    }

    int reshap() {
        int flag = 0;
        thlta[dz] = 0;
        for (int i = 7; i < enumpartNum; i++) {
            if (fabs(thlta[i] - 0) > 2e-2) {
                tozero(thlta[i]);
                flag = 1;
            }
        }
        thlta[bodyoffset] = 0.7;
        armOffset = 0;
        return flag;

    }
    void ohyeah() {
        thlta[rightUpperLegthltaZ] = -60, thlta[rightLoweLegthlta] = 60;
        thlta[leftUpperLegthltaZ] = 60, thlta[leftLoweLegthlta] = -60;
        thlta[leftUpperArmThltaZ] = -100; thlta[leftlowerArmThlta] = -80;
        thlta[rightUpperArmThltaZ] = 100, thlta[rightlowerArmThlta] = 80;
        armOffset = 0.2;
        thlta[bodyoffset] = 0.45;
    }


};
#endif