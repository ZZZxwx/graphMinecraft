
#include "Angel.h"
#include <string>
#include "cubeDraw.h"
#include "skybox.h"
#include "camera.h"
#include "depmapShadow.h"
#include "robotMan.h"
using namespace std;
///
/// create by 许伟鑫 2021/1/2
/// 图形学大作业 我的世界简单demo
/// 

//----------------------------------------------------------------------------
GLuint program;			//普通的着色器的ID
Camera cam;				//摄像机
cubedraw cubeDraw;		//方块的加载
Model crosshair;		//屏幕中间的选择点
skybox skyBox;			//天空盒
robotMan rb,rb2;		//史蒂夫一号与史蒂夫二号
int npc=0;				//表示是否调出史蒂夫二号npc
depmapShadow shadow;	//阴影映射类

//初始化的屏幕大小x
int windows_width=1000, windows_height=1000;


//display 函数，用于反复调用绘制主体
void display(void){
	//clearcolor设为白色，且每次绘制都清除缓存
	glClearColor(1.0, 1.0, 1.0,1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//---------------------获取阴影-----------------------------------
	shadow.draw();				//一些阴影映射初始化
	mat4 model = mat4(1.0f);	//
	glUniformMatrix4fv(glGetUniformLocation(shadow.programID, "model"), 1, true, model);
	cubeDraw.draw(shadow.programID);				//方块绘制(使用阴影着色器)
	rb.draw(shadow.programID,cam.yaw,cam.pitch);	//史蒂夫一号绘制(主要控制)
	if(npc)rb2.draw(shadow.programID, cam.yaw, cam.pitch);	//史蒂夫二号绘制

	//阴影绘制完成，恢复初始状态
	glBindFramebuffer(GL_FRAMEBUFFER, 0);			
	glViewport(0, 0, 1000, 1000);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//shadow.debug();		//shadow的debug函数，可以显示出阴影映射

	//-----------------绘制主体---------------------------------------
	glUseProgram(program);	//使用主体的着色器
	// 设置观察者位置
	GLint viewPosLoc = glGetUniformLocation(program, "viewPos");
	glUniform3f(viewPosLoc, cam.cameraPos.x, cam.cameraPos.y, cam.cameraPos.z);

	//加载正交投影矩阵与view矩阵
	glUniformMatrix4fv(glGetUniformLocation(program,"projection"), 1, true, cam.getprojectMatrix());
	glUniformMatrix4fv(glGetUniformLocation(program,"view"), 1, true, cam.lookAt(rb.pos));

	//加载阴影贴图
	glActiveTexture(GL_TEXTURE6);
	glBindTexture(GL_TEXTURE_2D, shadow.depthMap);
	glUniform1i(glGetUniformLocation(program,"shadowMap"), 6);

	cubeDraw.draw(program);							//方块绘制(使用阴影着色器)
	rb.draw(program, cam.yaw, cam.pitch);			//史蒂夫一号绘制(主要控制)
	if(npc)rb2.draw(program, 90, 0);						//史蒂夫二号绘制

	
	vec3 tmp = cam.cameraPos + cam.cameraFront;		//绘制中心导航点
	mat4 d = Translate(tmp.x,tmp.y,tmp.z)*Scale(0.01,0.01,0.01);
	glUseProgram(program);
	glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, true, d);
	crosshair.draw(program);

	skyBox.skyBoxDraw(cam);							//绘制天空盒
	glutSwapBuffers();								
}

//----------------------------------------------------------------------------
//debug用，可显示着色器的报错信息
void debugCallback(GLenum src, GLenum type, GLuint id, GLenum severity, GLsizei legth, const GLchar* message, const void* param) {
	printf("(%d): %s\n", id, message);
}

//----------------------------------------------------------------------------
//初始化
int init(void){	
	cubeDraw.init();											//方块地图初始化
	program = InitShader("shader/vshader81.glsl", "shader/fshader81.glsl");	//加载普通着色器
	rb.init();													//史蒂夫一号初始化
	rb2.init();													//史蒂夫二号初始化
	skyBox.init();												//天空盒初始化
	shadow.init();												//阴影映射初始化
																//中心导航点初始化加载
	crosshair.loadModel("./resource/object/crosshair/crosshair.obj");

	glEnable(GL_DEPTH_TEST);									//打开深度测试
	glEnable(GL_DEBUG_OUTPUT);									//打开debug，以及debug相关参数修改
	glDebugMessageCallback(debugCallback, 0);
	glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, 0, GL_TRUE);

	//设置光源属性
	glUseProgram(program);
	GLint lightAmbientLoc = glGetUniformLocation(program, "light.ambient");
	GLint lightDiffuseLoc = glGetUniformLocation(program, "light.diffuse");
	GLint lightSpecularLoc = glGetUniformLocation(program, "light.specular");
	GLint lightPosLoc = glGetUniformLocation(program, "light.position");
	GLint attConstant = glGetUniformLocation(program, "light.constant");
	GLint attLinear = glGetUniformLocation(program, "light.linear");
	GLint attQuadratic = glGetUniformLocation(program, "light.quadratic");
	glUniform3f(lightAmbientLoc, 0.3f, 0.3f, 0.3f);		//环境反射初始系数
	glUniform3f(lightDiffuseLoc, 0.7f, 0.7f, 0.7f);		//漫反射初始系数
	glUniform3f(lightSpecularLoc, 1.0f, 1.0f, 1.0f);	//镜面反射初始系数
	glUniform3f(lightPosLoc, shadow.lightPos.x, shadow.lightPos.y, shadow.lightPos.z);
	// 设置衰减系数
	glUniform1f(attConstant, 1.0f);
	glUniform1f(attLinear, 0.009f);
	glUniform1f(attQuadratic, 0.00032f);
	glUniformMatrix4fv(glGetUniformLocation(program, "lightSpaceMatrix"), 1, true, shadow.lightSpaceMatrix);
	return 1;
}

//----------------------------------------------------------------------------
//鼠标点击回调函数 与 鼠标移动回调函数
//左键删除方块，右键添加方块，方块为当前手上的方块
void mousefunc(int button,int state,int x,int y) {
	if (GLUT_LEFT_BUTTON == button&& state == GLUT_DOWN) {
		cubeDraw.set(cam.cameraPos,0.1*cam.cameraFront,0);
	}
	else if (GLUT_RIGHT_BUTTON == button && state ==GLUT_DOWN) {
		cubeDraw.set(cam.cameraPos,0.1*cam.cameraFront,rb.handkeepNo);
	}
	else if (GLUT_MIDDLE_BUTTON == button && state == GLUT_DOWN) {
		cubeDraw.set(cam.cameraPos, 0.1 * cam.cameraFront, 0,1);
	}
}
//鼠标移动控制摄像机视角
void mouse( int x, int y)
{
	cam.MoveMouse(x, y);
	glutPostRedisplay();
}


//------------------------------------------------------------------------------
//史蒂夫与方块之间的碰撞检测相关，由于都是方块，所以可以通过映射到map矩阵实现
//判断当前位置是否存在方块
int quad(int _x, int _y, int _z) {
	if (_x < 0 || _y < 0 || _z < 0)return 1;
	return cubeDraw.a[_x][_z][_y];
}
//垂直判断方块
int checkdep(float x, float y, float z) {
	if (fabs(y - int(y)) > 0.01) {
		return 0;
	}
	int x0 = x, y0 = y, z0 = z;
	int x1 = x + 0.2, z1 = z + 0.2;
	float y1 = y0 + 1, y2 = y0 + 2;
	if (quad(x0, y0, z0) || quad(x1, y0, z0) || quad(x0, y0, z1) || quad(x1, y0, z1)) {
		return 1;
	}
	return 0;
}
//水平判断方块
int check(float x, float y, float z) {
	if (x < 0 || y < 0 || z < 0 || x>10 || z>10) {
		return 1;
	}
	int x0 = x, y0 = y, z0 = z;
	int x1 = x + 0.2, z1 = z + 0.2;
	float y1 = y0 + 1, y2 = y0 + 2;
	if (quad(x0, y1, z0) || quad(x1, y1, z0) || quad(x0, y1, z1) || quad(x1, y1, z1) ||
		quad(x0, y2, z0) || quad(x1, y2, z0) || quad(x0, y2, z1) || quad(x1, y2, z1)) {
		return 1;
	}
	return 0;
}

//------------------------------------------------------------------------------
//史蒂夫一号控制与动作的相关回调函数，主要由时间回调函数完成
//reshap人物复原的时间回调函数，触发后直到复原为止，将一直调用。
void reshap(int value = 0) {
	if (rb.reshap()) {
		rb2.reshap();
		glutTimerFunc(10, reshap, 1);
	}
	glutPostRedisplay();
}
//人物跳跃的时间回调函数，触发后将会上升一段时间。
int jumpadd = 0;
void idlejump(int value = 0) {
	if (!check(rb.pos.x, rb.pos.y, rb.pos.z)) {
		rb.jump();			//若上方由障碍物将不再上升，若无则调用史蒂夫一号的跳跃函数升高。
	}
	else {
		jumpadd = 0;
		glutTimerFunc(10, reshap, 1);//升高后会调用reshap函数复原史蒂夫的挥手状态。
		return;
	}
	jumpadd++;
	if (jumpadd != 12) {
		glutTimerFunc(10, idlejump, 1);
	}
	else { 
		jumpadd = 0; 
		glutTimerFunc(10, reshap, 1);//升高后会调用reshap函数复原史蒂夫的挥手状态。
	}
	glutPostRedisplay();
}

//特殊动作的时间回调函数
float jijian = 1;			//设置击剑速度
void idelfun(int value=0) {
	if (rb.isfun) {									//若特殊动作按钮打开
		rb2.thlta[funthlta] += -jijian;
		rb2.thlta[rightUpperLegthltaZ] += -jijian / 10;
		rb2.thlta[bodyoffset] += -jijian / 1000;
		rb2.thlta[leftUpperLegthltaZ] += jijian / 10;
		rb.thlta[funthlta] += jijian;
		rb.thlta[rightUpperLegthltaZ] += jijian / 10;
		rb.thlta[bodyoffset] += jijian / 1000;
		rb.thlta[leftUpperLegthltaZ] -= jijian / 10;
		if (rb.thlta[funthlta] > 50 || rb.thlta[funthlta] < -50) {
			jijian = -jijian;
		}
		rb.thlta[dz] = rand();
		rb2.thlta[dz] = rand();
		glutTimerFunc(10, idelfun, 1);
	}
	else {
		reshap();
	}
}

//----------------------------------------------------------------------------
//键盘控制相关
void keyboard(unsigned char key, int x, int y){
	if (!cam.free) {								//非自由相机模式将可以操作史蒂夫
		vec3 prepos = rb.pos;
		cam.keyboard(key, x, y, rb.pos, rb.thlta[bodyThlta]);	//点击键盘wasd后将会移动史蒂夫，并且控制身体旋转
		if (check(rb.pos.x, rb.pos.y, rb.pos.z)) {
			rb.pos = prepos;						//若旋转后非法则恢复，不能移动
		}
	}else cam.keyboard(key, x, y, rb.pos, rb.thlta[bodyThlta]);
	switch (key) {
	case 033:									// Escape Key
	case 'q': case 'Q':
		cubeDraw.savemap();						//退出后保存地图
		exit(EXIT_SUCCESS);
		break;
	case ' ':									//空格跳跃
		if (fabs(rb.pos.y-(int)(rb.pos.y+0.01))<2e-2) {						
			idlejump();							//若此时在空中则不响应跳跃
		}
		break;
	case 'w':
	case 's':
	case 'a':
	case 'd':
		if(!cam.free)rb.shand();				//移动时将会调用史蒂夫的shand函数挥动手和脚
		break;
	case 'e':									//特殊击剑动作函数
		rb.ohyeah();
		rb2.ohyeah();
		rb.isfun = !rb.isfun;
		rb2.isfun = !rb2.isfun;
		if(rb.isfun)
			glutTimerFunc(10, idelfun, 1);
		break;
	case 'r':									//切换第一视角与第三视角
		cam.first = !cam.first;
		break;
	case 't':									//切换自由视角，此时不能控制史蒂夫移动
		cam.free = !cam.free;
		break;
	case '1':									//‘1’-‘5’分别表示不同的手持方块，点击后切换
		rb.handkeepNo = 1;
		break;
	case '2':
		rb.handkeepNo = 2;
		break;
	case '3':
		rb.handkeepNo = 3;
		break;
	case '4':
		rb.handkeepNo = 4;
		break;
	case '5':
		rb.handkeepNo = 5;
		break;
	case '6':
		npc = !npc;								//'6'决定是否调出npc
		break;
	}
	
}

//------------------------------------------------------------
//鼠标隐藏，并且使得鼠标移至中心点，实现类全屏效果
void cursor() {
	POINT p;
	GetCursorPos(&p);
	int _x = p.x - (windows_width / 2);
	int _y = p.y - ( windows_height / 2);
	cam.mousePrex -= _x;
	cam.mousePrey -= _y;
	SetCursorPos(windows_width / 2, windows_height / 2);
	while (ShowCursor(FALSE) >= 0)
		ShowCursor(FALSE);
}

//----------------------------------------------------------------
//空闲回调函数，主要负责一些全局都要运行的功能，比如说重力，鼠标中心控制，手上物体的旋转等
float grivity = 0.01;		//设置重力
void idelfunc() {
	if (!cam.free&&!checkdep(rb.pos.x, rb.pos.y- grivity, rb.pos.z)) {
		rb.pos.y -= grivity;						//若当前可以下降则下降
	}										
	rb.thlta[handkeepThlta] += 0.05;				//控制手上物体的旋转
	cursor();										//鼠标控制
	glutPostRedisplay();
}

//----------------------------------------------------------------------------
//主函数
int main(int argc, char** argv){
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH );
	glutInitWindowSize(windows_width, windows_height);
	glutInitContextVersion(3, 2);
	glutInitContextProfile(GLUT_CORE_PROFILE);
	glutCreateWindow("许伟鑫_2018161063_期末大作业");
	glewExperimental = GL_TRUE;
	glewInit();
	init();
	
	glutDisplayFunc(display);
	glutKeyboardFunc(keyboard);
	glutPassiveMotionFunc(mouse);
	glutMouseFunc(mousefunc);
	glutIdleFunc(idelfunc);
	glutMainLoop();
	return 0;
}
