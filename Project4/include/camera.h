#ifndef _CAMERA_H_
#define _CAMERA_H_
#include "Angel.h"
#include "cubedraw.h"
/// <summary>
/// 声明一个摄像机类，以实现镜头的转化。
/// </summary>
extern int windows_width, windows_height;
extern cubedraw cubeDraw;
class Camera
{
public:
	vec3 cameraPos = vec3(0, 0, 3);		//定义相机位置eye
	vec3 cameraFront = vec3(0, 0, -1);	//定义相机面对方向Front at-eye
	vec3 cameraUp = vec3(0, 1, 0);		//定义正上方向up

	float fov = 60;		//视野角度
	float zNear = 0.1;	//近视野
	float zFar = 100;	//远视野(即迷雾)

	float speed = 0.08;	//摄像机的移动速度

	float pitch = 0, yaw = -89;	//摄像机视角方向
	int check(vec3& pos) {
		if (pos.x < 0 || pos.y < 0 || pos.z < 0 || pos.x >= 10 || pos.y >= 10 || pos.z >= 10) {
			return 0;
		}
		return cubeDraw.a[(int)pos.x][(int)pos.z][(int)(pos.y + 0.02)];
	}
	//初始化或者是RESET
	void init() {
		cameraPos = vec3(0, 0, 5);
		cameraFront = vec3(0, 0, -1);
		cameraUp = vec3(0, 1, 0);
		pitch = 0, yaw = -89;
	}


	/// Lookat函数，获得View矩阵
	mat4 lookAt(const vec3 eye, const vec3 at, const vec3 up)
	{
		// TODO 请按照实验课内容补全相机观察矩阵的计算
		vec3 n = normalize(eye - at);
		vec3 u = normalize(cross(up, n));
		vec3 v = normalize(cross(n, u));
		mat4 res = mat4(vec4(u, 0), vec4(v, 0), vec4(n, 0), vec4(0, 0, 0, 1.0));
		res = res * Translate(-eye);
		return res;
	}

	//对lookat函数的一个封装调用
	//正常摄像机的调用
	mat4 lookAt(){
		getFrontchangeCamera();
		return lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
	}

	//对于当前项目的特殊调用
	int first = 0;							//决定是否打开第一人称
	int free = 0;							//决定是否打开自由视角
	mat4 lookAt(vec3 &pos){
		getFrontchangeCamera();				//获取前向向量
		if (!first&&!free) {						//如果不是第一人称，则使用第三人称
			cameraPos = vec3(0, 2, 0) + pos;
			cameraPos -= 3 * cameraFront;	//第三人称于第一人称往上升，并向后偏移一段
			if (check(cameraPos)) {			//若偏移后被遮挡，则往前偏移一段
				cameraPos += 2.5*cameraFront;
				cameraPos.y -= 1;
			}
		}
		else if(!free){
			//第一人称的摄像机位置在人正前方
			cameraPos = vec3(0, 1, 0) + pos+ 2*normalize(vec3(cameraFront.x,0, cameraFront.z));
		}
		//返回view矩阵
		return lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
	}

	//计算projectMatrix矩阵的函数
	mat4 getprojectMatrix() {
		return Perspective(fov, windows_height / windows_width, zNear, zFar);
	}

	//获得modelViewprojectMatrix
	mat4 getModelViewprojectMatrix() {
		return getprojectMatrix() * lookAt();
	}

	//获得modelViewprojectMatrix
	mat4 getModelViewprojectMatrix(vec3 &pos) {
		return getprojectMatrix() * lookAt(pos);
	}

	//模式以FPS模式 摄像机的移动函数
	//相机与人物绑定的时候，挪动相机就剩挪动人物的位置。(以此同时实现人物的移动)
	void moveFront(vec3& pos) {	//前
		vec3 speedAdd = speed * normalize(vec3(cameraFront.x,0, cameraFront.z));
		pos += speedAdd;
		//cameraPos += speed * cameraFront;
	}
	void moveBack(vec3& pos) {	//后
		//cameraPos -= speed * cameraFront;
		vec3 speedAdd = speed * normalize(vec3(cameraFront.x, 0, cameraFront.z));
		pos -= speedAdd;

	}
	void moveLeft(vec3& pos) {	//左
		//cameraPos -= speed * normalize(cross(cameraFront, cameraUp));
		vec3 speedAdd = speed * normalize(cross(vec3(cameraFront.x, 0, cameraFront.z), cameraUp));
		pos -= speedAdd;
	}
	void moveRight(vec3& pos) {	//右
		//cameraPos += speed * normalize(cross(cameraFront, cameraUp));
		vec3 speedAdd = speed * normalize(cross(vec3(cameraFront.x, 0, cameraFront.z), cameraUp));
		pos += speedAdd;
	}

	//相机不与人物绑定时候的重载函数，以此移动相机
	void moveFront() {	//前
		cameraPos += speed * cameraFront;
	}
	void moveBack() {	//后
		cameraPos -= speed * cameraFront;
	}
	void moveLeft() {	//左
		cameraPos -= speed * normalize(cross(cameraFront, cameraUp));
	}
	void moveRight() {	//右
		cameraPos += speed * normalize(cross(cameraFront, cameraUp));
	}


	//模式以FPS模式摄像头视角的移动函数
	//具体为改变cameraFront的朝向来控制摄像机的指向方向
	//从yaw角和pitch角来得到Front
	void getFrontchangeCamera() {
		cameraFront.x = cos(pitch * DegreesToRadians) * cos(yaw * DegreesToRadians);
		cameraFront.y = sin(pitch * DegreesToRadians);
		cameraFront.z = cos(pitch * DegreesToRadians) * sin(yaw * DegreesToRadians);
		cameraFront = normalize(cameraFront);
	}

	//视角改变
	float sensi = 0.4;			//视角改变的敏感度
	void addPitch(float v) {
		pitch += sensi * v;
		if (pitch >= 90)pitch -= sensi * v;
		else if (pitch <= -90)pitch -= sensi * v;
	}
	void addYaw(float v) {
		yaw += sensi * v;

	}

	//摄像机的键盘响应相关
	void keyboard(unsigned char key, int x, int y,vec3 &pos,float &bodyThlta){
		if(!free)
			switch (key){
				case 'w'://前进
					moveFront(pos);
					bodyThlta = 90 - yaw;
					break;
				case 'a'://向左
					moveLeft(pos);
					break;
				case 's'://向后
					moveBack(pos);
					bodyThlta = 90 - yaw;
					break;
				case 'd'://向右
					moveRight(pos);
					break;
			}
		else 
			switch (key) {
				case 'w'://前进
					moveFront();
					break;
				case 'a'://向左
					moveLeft();
					break;
				case 's'://向后
					moveBack();
					break;
				case 'd'://向右
					moveRight();
					break;
			}
		glutPostRedisplay();
	}

	//鼠标移动相关响应函数
	//移动后修改yaw角和pitch角，修改相机朝向
	int mousePrex = windows_width/2, mousePrey = windows_height/2;
	void MoveMouse(int x, int y) {
		addYaw(x - mousePrex);
		addPitch(mousePrey - y);
		mousePrex = x;
		mousePrey = y;
	}
};
#endif