#ifndef _CAMERA_H_
#define _CAMERA_H_
#include "Angel.h"
#include "cubedraw.h"
/// <summary>
/// ����һ��������࣬��ʵ�־�ͷ��ת����
/// </summary>
extern int windows_width, windows_height;
extern cubedraw cubeDraw;
class Camera
{
public:
	vec3 cameraPos = vec3(0, 0, 3);		//�������λ��eye
	vec3 cameraFront = vec3(0, 0, -1);	//���������Է���Front at-eye
	vec3 cameraUp = vec3(0, 1, 0);		//�������Ϸ���up

	float fov = 60;		//��Ұ�Ƕ�
	float zNear = 0.1;	//����Ұ
	float zFar = 100;	//Զ��Ұ(������)

	float speed = 0.08;	//��������ƶ��ٶ�

	float pitch = 0, yaw = -89;	//������ӽǷ���
	int check(vec3& pos) {
		if (pos.x < 0 || pos.y < 0 || pos.z < 0 || pos.x >= 10 || pos.y >= 10 || pos.z >= 10) {
			return 0;
		}
		return cubeDraw.a[(int)pos.x][(int)pos.z][(int)(pos.y + 0.02)];
	}
	//��ʼ��������RESET
	void init() {
		cameraPos = vec3(0, 0, 5);
		cameraFront = vec3(0, 0, -1);
		cameraUp = vec3(0, 1, 0);
		pitch = 0, yaw = -89;
	}


	/// Lookat���������View����
	mat4 lookAt(const vec3 eye, const vec3 at, const vec3 up)
	{
		// TODO �밴��ʵ������ݲ�ȫ����۲����ļ���
		vec3 n = normalize(eye - at);
		vec3 u = normalize(cross(up, n));
		vec3 v = normalize(cross(n, u));
		mat4 res = mat4(vec4(u, 0), vec4(v, 0), vec4(n, 0), vec4(0, 0, 0, 1.0));
		res = res * Translate(-eye);
		return res;
	}

	//��lookat������һ����װ����
	//����������ĵ���
	mat4 lookAt(){
		getFrontchangeCamera();
		return lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
	}

	//���ڵ�ǰ��Ŀ���������
	int first = 0;							//�����Ƿ�򿪵�һ�˳�
	int free = 0;							//�����Ƿ�������ӽ�
	mat4 lookAt(vec3 &pos){
		getFrontchangeCamera();				//��ȡǰ������
		if (!first&&!free) {						//������ǵ�һ�˳ƣ���ʹ�õ����˳�
			cameraPos = vec3(0, 2, 0) + pos;
			cameraPos -= 3 * cameraFront;	//�����˳��ڵ�һ�˳��������������ƫ��һ��
			if (check(cameraPos)) {			//��ƫ�ƺ��ڵ�������ǰƫ��һ��
				cameraPos += 2.5*cameraFront;
				cameraPos.y -= 1;
			}
		}
		else if(!free){
			//��һ�˳Ƶ������λ��������ǰ��
			cameraPos = vec3(0, 1, 0) + pos+ 2*normalize(vec3(cameraFront.x,0, cameraFront.z));
		}
		//����view����
		return lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
	}

	//����projectMatrix����ĺ���
	mat4 getprojectMatrix() {
		return Perspective(fov, windows_height / windows_width, zNear, zFar);
	}

	//���modelViewprojectMatrix
	mat4 getModelViewprojectMatrix() {
		return getprojectMatrix() * lookAt();
	}

	//���modelViewprojectMatrix
	mat4 getModelViewprojectMatrix(vec3 &pos) {
		return getprojectMatrix() * lookAt(pos);
	}

	//ģʽ��FPSģʽ ��������ƶ�����
	//���������󶨵�ʱ��Ų�������ʣŲ�������λ�á�(�Դ�ͬʱʵ��������ƶ�)
	void moveFront(vec3& pos) {	//ǰ
		vec3 speedAdd = speed * normalize(vec3(cameraFront.x,0, cameraFront.z));
		pos += speedAdd;
		//cameraPos += speed * cameraFront;
	}
	void moveBack(vec3& pos) {	//��
		//cameraPos -= speed * cameraFront;
		vec3 speedAdd = speed * normalize(vec3(cameraFront.x, 0, cameraFront.z));
		pos -= speedAdd;

	}
	void moveLeft(vec3& pos) {	//��
		//cameraPos -= speed * normalize(cross(cameraFront, cameraUp));
		vec3 speedAdd = speed * normalize(cross(vec3(cameraFront.x, 0, cameraFront.z), cameraUp));
		pos -= speedAdd;
	}
	void moveRight(vec3& pos) {	//��
		//cameraPos += speed * normalize(cross(cameraFront, cameraUp));
		vec3 speedAdd = speed * normalize(cross(vec3(cameraFront.x, 0, cameraFront.z), cameraUp));
		pos += speedAdd;
	}

	//������������ʱ������غ������Դ��ƶ����
	void moveFront() {	//ǰ
		cameraPos += speed * cameraFront;
	}
	void moveBack() {	//��
		cameraPos -= speed * cameraFront;
	}
	void moveLeft() {	//��
		cameraPos -= speed * normalize(cross(cameraFront, cameraUp));
	}
	void moveRight() {	//��
		cameraPos += speed * normalize(cross(cameraFront, cameraUp));
	}


	//ģʽ��FPSģʽ����ͷ�ӽǵ��ƶ�����
	//����Ϊ�ı�cameraFront�ĳ����������������ָ����
	//��yaw�Ǻ�pitch�����õ�Front
	void getFrontchangeCamera() {
		cameraFront.x = cos(pitch * DegreesToRadians) * cos(yaw * DegreesToRadians);
		cameraFront.y = sin(pitch * DegreesToRadians);
		cameraFront.z = cos(pitch * DegreesToRadians) * sin(yaw * DegreesToRadians);
		cameraFront = normalize(cameraFront);
	}

	//�ӽǸı�
	float sensi = 0.4;			//�ӽǸı�����ж�
	void addPitch(float v) {
		pitch += sensi * v;
		if (pitch >= 90)pitch -= sensi * v;
		else if (pitch <= -90)pitch -= sensi * v;
	}
	void addYaw(float v) {
		yaw += sensi * v;

	}

	//������ļ�����Ӧ���
	void keyboard(unsigned char key, int x, int y,vec3 &pos,float &bodyThlta){
		if(!free)
			switch (key){
				case 'w'://ǰ��
					moveFront(pos);
					bodyThlta = 90 - yaw;
					break;
				case 'a'://����
					moveLeft(pos);
					break;
				case 's'://���
					moveBack(pos);
					bodyThlta = 90 - yaw;
					break;
				case 'd'://����
					moveRight(pos);
					break;
			}
		else 
			switch (key) {
				case 'w'://ǰ��
					moveFront();
					break;
				case 'a'://����
					moveLeft();
					break;
				case 's'://���
					moveBack();
					break;
				case 'd'://����
					moveRight();
					break;
			}
		glutPostRedisplay();
	}

	//����ƶ������Ӧ����
	//�ƶ����޸�yaw�Ǻ�pitch�ǣ��޸��������
	int mousePrex = windows_width/2, mousePrey = windows_height/2;
	void MoveMouse(int x, int y) {
		addYaw(x - mousePrex);
		addPitch(mousePrey - y);
		mousePrex = x;
		mousePrey = y;
	}
};
#endif