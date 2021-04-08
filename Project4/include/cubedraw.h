
#ifndef _CUBEDRAW_H_
#define _CUBEDRAW_H_
#include "model.h"
#include <fstream>
#include <iostream>
#include <algorithm>
#include <vector>
///
/// 方块地图绘制类，用以绘制所有的方块
/// 
using namespace std;
// 不同方块obj模型的路径
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
	Model objModel[10];		//模型加载
	int a[10][10][10];		//地图记录
	int b[10][10][10];		//形变记录
	mat4 rotatechange;		//形变变换矩阵
	/// 
	/// 初始化，读入模型和地图
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

	//退出的时候保存地图
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

	//地图绘制
	//第(i,j,k)方块的范围为(i~i+1,j~j+1,k~k+1);
	void draw(GLint program) {
		int modelLocation = glGetUniformLocation(program, "model");
		for (int k = 0; k < 10; k++) {
			for (int i = 0; i < 10; i++) {
				for (int j = 0; j < 10; j++) {
					if (a[i][j][k]) {
						mat4 d = Translate(i+0.5, -0.5+k, j+0.5);
						//有形变的位置还需要乘上形变矩阵
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

	//设置或者拆除或者形变(x,y,z)处的方块
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

	//检查当前坐标是否在一个方块里面
	int check(vec3 &pos) {
		if (pos.x < 0 || pos.y < 0 || pos.z < 0 || pos.x >= 10 || pos.y >= 10 || pos.z >= 10) {
			return -1;
		}
		return a[(int)pos.x][(int)pos.z][(int)(pos.y + 0.02)];
	}

	//设置或者拆除或者形变鼠标指向的方块
	//通过pos处朝front方向发射一条射线，若为删除或形变则删除或形变第一个射线碰到的方块
	//若为设置则在射线第一个碰到的方块之前设置方块
	void set(vec3 pos, vec3 front, int type,int rotate = 0) {
		
		pos.y += 0.9;						//计算偏移量
		vec3 tmp = pos + front;			
		while (tmp.x < 0 || tmp.z < 0) {	//防止坐标非法
			tmp += front;
		}
		while (tmp.x >= 10 || tmp.z >= 10) {//防止坐标非法
			tmp -= front;
		}
		while(1) {						
			int v = check(tmp);
			if (v == -1) {					//-1表示坐标超出范围都找不到方块
				return;					
			}
			else if (v) {					//正整数表示找到方块
				if(type)tmp -= front;		//若为设置在方块之前设置方块
				break;						//若为删除或形变则删除或形变当前方块
			}
			else {						
				tmp += front;				//射线继续往前探索
			}
		}
		if(rotate)							//判断是否是形变
			set(tmp.x, tmp.y, tmp.z, type,1);
		else 
			set(tmp.x, tmp.y, tmp.z,type);
	}
};

#endif