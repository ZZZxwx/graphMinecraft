
#ifndef _CUBEDRAW_H_
#define _CUBEDRAW_H_
#include "model.h"
#include <fstream>
#include <iostream>
#include <algorithm>
#include <vector>
///
/// �����ͼ�����࣬���Ի������еķ���
/// 
using namespace std;
// ��ͬ����objģ�͵�·��
static vector<string>modelFile{
	"./resource/object/cube/cube.obj",
	"./resource/object/leave/cube.obj",
	"./resource/object/rstone/cube.obj",
	"./resource/object/wood/wood.obj",
	"./resource/object/SwordMinecraft/SwordMinecraft.obj"
};
//cube number type
//		1		grass
//		2		leaves
//		3		stone
//		4		wood

class cubedraw
{
public:
	Model objModel[10];		//ģ�ͼ���
	int a[10][10][10];		//��ͼ��¼
	int b[10][10][10];		//�α��¼
	mat4 rotatechange;		//�α�任����
	/// 
	/// ��ʼ��������ģ�ͺ͵�ͼ
	/// 
	int init() {
		rotatechange = RotateY(45) * Scale(0.7, 1, 0.7);
		for (int i = 0; i < modelFile.size(); i++) {
			if (!objModel[i+1].loadModel(modelFile[i])) {
				cout << "model Load Error" << endl;
				return 0;
			}
		}
		ifstream mapfile("map/map.txt");
		for (int k = 0; k < 10; k++) {
			for (int i = 0; i < 10; i++) {
				for (int j = 0; j < 10; j++) {
					mapfile >> a[i][j][k];
				}
			}
		}
		return 1;
	}

	//�˳���ʱ�򱣴��ͼ
	void savemap() {
		ofstream mapfile("map/map.txt");
		for (int k = 0; k < 10; k++) {
			for (int i = 0; i < 10; i++) {
				for (int j = 0; j < 10; j++) {
					mapfile << a[i][j][k]<<" ";
				}
				mapfile << "\n";
			}
			mapfile << "\n";
		}
	}

	//��ͼ����
	//��(i,j,k)����ķ�ΧΪ(i~i+1,j~j+1,k~k+1);
	void draw(GLint program) {
		int modelLocation = glGetUniformLocation(program, "model");
		for (int k = 0; k < 10; k++) {
			for (int i = 0; i < 10; i++) {
				for (int j = 0; j < 10; j++) {
					if (a[i][j][k]) {
						mat4 d = Translate(i+0.5, -0.5+k, j+0.5);
						//���α��λ�û���Ҫ�����α����
						if (b[i][j][k]) {
							d = d* rotatechange;
						}
						glUseProgram(program);
						glUniformMatrix4fv(modelLocation, 1, true, d);
						objModel[a[i][j][k]].draw(program);
					}
				}
			}
		}
	}

	//���û��߲�������α�(x,y,z)���ķ���
	void set(int x,int y,int z,int type,int rotate = 0) {
		if (a[x][z][y]&&type)return ;
		//cout << x <<" "<< y <<" "<< z << endl;
		if (rotate) {
			b[x][z][y] = !b[x][z][y];
		}
		else {
			a[x][z][y] = type;
			if (type == 0)b[x][z][y] = 0;
		}
	}

	//��鵱ǰ�����Ƿ���һ����������
	int check(vec3 &pos) {
		if (pos.x < 0 || pos.y < 0 || pos.z < 0 || pos.x >= 10 || pos.y >= 10 || pos.z >= 10) {
			return -1;
		}
		return a[(int)pos.x][(int)pos.z][(int)(pos.y + 0.02)];
	}

	//���û��߲�������α����ָ��ķ���
	//ͨ��pos����front������һ�����ߣ���Ϊɾ�����α���ɾ�����α��һ�����������ķ���
	//��Ϊ�����������ߵ�һ�������ķ���֮ǰ���÷���
	void set(vec3 pos, vec3 front, int type,int rotate = 0) {
		
		pos.y += 0.9;						//����ƫ����
		vec3 tmp = pos + front;			
		while (tmp.x < 0 || tmp.z < 0) {	//��ֹ����Ƿ�
			tmp += front;
		}
		while (tmp.x >= 10 || tmp.z >= 10) {//��ֹ����Ƿ�
			tmp -= front;
		}
		while(1) {						
			int v = check(tmp);
			if (v == -1) {					//-1��ʾ���곬����Χ���Ҳ�������
				return;					
			}
			else if (v) {					//��������ʾ�ҵ�����
				if(type)tmp -= front;		//��Ϊ�����ڷ���֮ǰ���÷���
				break;						//��Ϊɾ�����α���ɾ�����α䵱ǰ����
			}
			else {						
				tmp += front;				//���߼�����ǰ̽��
			}
		}
		if(rotate)							//�ж��Ƿ����α�
			set(tmp.x, tmp.y, tmp.z, type,1);
		else 
			set(tmp.x, tmp.y, tmp.z,type);
	}
};

#endif