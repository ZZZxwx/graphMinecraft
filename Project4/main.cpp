
#include "Angel.h"
#include <string>
#include "cubeDraw.h"
#include "skybox.h"
#include "camera.h"
#include "depmapShadow.h"
#include "robotMan.h"
using namespace std;
///
/// create by ��ΰ�� 2021/1/2
/// ͼ��ѧ����ҵ �ҵ������demo
/// 

//----------------------------------------------------------------------------
GLuint program;			//��ͨ����ɫ����ID
Camera cam;				//�����
cubedraw cubeDraw;		//����ļ���
Model crosshair;		//��Ļ�м��ѡ���
skybox skyBox;			//��պ�
robotMan rb,rb2;		//ʷ�ٷ�һ����ʷ�ٷ����
int npc=0;				//��ʾ�Ƿ����ʷ�ٷ����npc
depmapShadow shadow;	//��Ӱӳ����

//��ʼ������Ļ��Сx
int windows_width=1000, windows_height=1000;


//display ���������ڷ������û�������
void display(void){
	//clearcolor��Ϊ��ɫ����ÿ�λ��ƶ��������
	glClearColor(1.0, 1.0, 1.0,1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//---------------------��ȡ��Ӱ-----------------------------------
	shadow.draw();				//һЩ��Ӱӳ���ʼ��
	mat4 model = mat4(1.0f);	//
	glUniformMatrix4fv(glGetUniformLocation(shadow.programID, "model"), 1, true, model);
	cubeDraw.draw(shadow.programID);				//�������(ʹ����Ӱ��ɫ��)
	rb.draw(shadow.programID,cam.yaw,cam.pitch);	//ʷ�ٷ�һ�Ż���(��Ҫ����)
	if(npc)rb2.draw(shadow.programID, cam.yaw, cam.pitch);	//ʷ�ٷ���Ż���

	//��Ӱ������ɣ��ָ���ʼ״̬
	glBindFramebuffer(GL_FRAMEBUFFER, 0);			
	glViewport(0, 0, 1000, 1000);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//shadow.debug();		//shadow��debug������������ʾ����Ӱӳ��

	//-----------------��������---------------------------------------
	glUseProgram(program);	//ʹ���������ɫ��
	// ���ù۲���λ��
	GLint viewPosLoc = glGetUniformLocation(program, "viewPos");
	glUniform3f(viewPosLoc, cam.cameraPos.x, cam.cameraPos.y, cam.cameraPos.z);

	//��������ͶӰ������view����
	glUniformMatrix4fv(glGetUniformLocation(program,"projection"), 1, true, cam.getprojectMatrix());
	glUniformMatrix4fv(glGetUniformLocation(program,"view"), 1, true, cam.lookAt(rb.pos));

	//������Ӱ��ͼ
	glActiveTexture(GL_TEXTURE6);
	glBindTexture(GL_TEXTURE_2D, shadow.depthMap);
	glUniform1i(glGetUniformLocation(program,"shadowMap"), 6);

	cubeDraw.draw(program);							//�������(ʹ����Ӱ��ɫ��)
	rb.draw(program, cam.yaw, cam.pitch);			//ʷ�ٷ�һ�Ż���(��Ҫ����)
	if(npc)rb2.draw(program, 90, 0);						//ʷ�ٷ���Ż���

	
	vec3 tmp = cam.cameraPos + cam.cameraFront;		//�������ĵ�����
	mat4 d = Translate(tmp.x,tmp.y,tmp.z)*Scale(0.01,0.01,0.01);
	glUseProgram(program);
	glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, true, d);
	crosshair.draw(program);

	skyBox.skyBoxDraw(cam);							//������պ�
	glutSwapBuffers();								
}

//----------------------------------------------------------------------------
//debug�ã�����ʾ��ɫ���ı�����Ϣ
void debugCallback(GLenum src, GLenum type, GLuint id, GLenum severity, GLsizei legth, const GLchar* message, const void* param) {
	printf("(%d): %s\n", id, message);
}

//----------------------------------------------------------------------------
//��ʼ��
int init(void){	
	cubeDraw.init();											//�����ͼ��ʼ��
	program = InitShader("shader/vshader81.glsl", "shader/fshader81.glsl");	//������ͨ��ɫ��
	rb.init();													//ʷ�ٷ�һ�ų�ʼ��
	rb2.init();													//ʷ�ٷ���ų�ʼ��
	skyBox.init();												//��պг�ʼ��
	shadow.init();												//��Ӱӳ���ʼ��
																//���ĵ������ʼ������
	crosshair.loadModel("./resource/object/crosshair/crosshair.obj");

	glEnable(GL_DEPTH_TEST);									//����Ȳ���
	glEnable(GL_DEBUG_OUTPUT);									//��debug���Լ�debug��ز����޸�
	glDebugMessageCallback(debugCallback, 0);
	glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, 0, GL_TRUE);

	//���ù�Դ����
	glUseProgram(program);
	GLint lightAmbientLoc = glGetUniformLocation(program, "light.ambient");
	GLint lightDiffuseLoc = glGetUniformLocation(program, "light.diffuse");
	GLint lightSpecularLoc = glGetUniformLocation(program, "light.specular");
	GLint lightPosLoc = glGetUniformLocation(program, "light.position");
	GLint attConstant = glGetUniformLocation(program, "light.constant");
	GLint attLinear = glGetUniformLocation(program, "light.linear");
	GLint attQuadratic = glGetUniformLocation(program, "light.quadratic");
	glUniform3f(lightAmbientLoc, 0.3f, 0.3f, 0.3f);		//���������ʼϵ��
	glUniform3f(lightDiffuseLoc, 0.7f, 0.7f, 0.7f);		//�������ʼϵ��
	glUniform3f(lightSpecularLoc, 1.0f, 1.0f, 1.0f);	//���淴���ʼϵ��
	glUniform3f(lightPosLoc, shadow.lightPos.x, shadow.lightPos.y, shadow.lightPos.z);
	// ����˥��ϵ��
	glUniform1f(attConstant, 1.0f);
	glUniform1f(attLinear, 0.009f);
	glUniform1f(attQuadratic, 0.00032f);
	glUniformMatrix4fv(glGetUniformLocation(program, "lightSpaceMatrix"), 1, true, shadow.lightSpaceMatrix);
	return 1;
}

//----------------------------------------------------------------------------
//������ص����� �� ����ƶ��ص�����
//���ɾ�����飬�Ҽ���ӷ��飬����Ϊ��ǰ���ϵķ���
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
//����ƶ�����������ӽ�
void mouse( int x, int y)
{
	cam.MoveMouse(x, y);
	glutPostRedisplay();
}


//------------------------------------------------------------------------------
//ʷ�ٷ��뷽��֮�����ײ�����أ����ڶ��Ƿ��飬���Կ���ͨ��ӳ�䵽map����ʵ��
//�жϵ�ǰλ���Ƿ���ڷ���
int quad(int _x, int _y, int _z) {
	if (_x < 0 || _y < 0 || _z < 0)return 1;
	return cubeDraw.a[_x][_z][_y];
}
//��ֱ�жϷ���
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
//ˮƽ�жϷ���
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
//ʷ�ٷ�һ�ſ����붯������ػص���������Ҫ��ʱ��ص��������
//reshap���︴ԭ��ʱ��ص�������������ֱ����ԭΪֹ����һֱ���á�
void reshap(int value = 0) {
	if (rb.reshap()) {
		rb2.reshap();
		glutTimerFunc(10, reshap, 1);
	}
	glutPostRedisplay();
}
//������Ծ��ʱ��ص������������󽫻�����һ��ʱ�䡣
int jumpadd = 0;
void idlejump(int value = 0) {
	if (!check(rb.pos.x, rb.pos.y, rb.pos.z)) {
		rb.jump();			//���Ϸ����ϰ��ｫ�������������������ʷ�ٷ�һ�ŵ���Ծ�������ߡ�
	}
	else {
		jumpadd = 0;
		glutTimerFunc(10, reshap, 1);//���ߺ�����reshap������ԭʷ�ٷ�Ļ���״̬��
		return;
	}
	jumpadd++;
	if (jumpadd != 12) {
		glutTimerFunc(10, idlejump, 1);
	}
	else { 
		jumpadd = 0; 
		glutTimerFunc(10, reshap, 1);//���ߺ�����reshap������ԭʷ�ٷ�Ļ���״̬��
	}
	glutPostRedisplay();
}

//���⶯����ʱ��ص�����
float jijian = 1;			//���û����ٶ�
void idelfun(int value=0) {
	if (rb.isfun) {									//�����⶯����ť��
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
//���̿������
void keyboard(unsigned char key, int x, int y){
	if (!cam.free) {								//���������ģʽ�����Բ���ʷ�ٷ�
		vec3 prepos = rb.pos;
		cam.keyboard(key, x, y, rb.pos, rb.thlta[bodyThlta]);	//�������wasd�󽫻��ƶ�ʷ�ٷ򣬲��ҿ���������ת
		if (check(rb.pos.x, rb.pos.y, rb.pos.z)) {
			rb.pos = prepos;						//����ת��Ƿ���ָ��������ƶ�
		}
	}else cam.keyboard(key, x, y, rb.pos, rb.thlta[bodyThlta]);
	switch (key) {
	case 033:									// Escape Key
	case 'q': case 'Q':
		cubeDraw.savemap();						//�˳��󱣴��ͼ
		exit(EXIT_SUCCESS);
		break;
	case ' ':									//�ո���Ծ
		if (fabs(rb.pos.y-(int)(rb.pos.y+0.01))<2e-2) {						
			idlejump();							//����ʱ�ڿ�������Ӧ��Ծ
		}
		break;
	case 'w':
	case 's':
	case 'a':
	case 'd':
		if(!cam.free)rb.shand();				//�ƶ�ʱ�������ʷ�ٷ��shand�����Ӷ��ֺͽ�
		break;
	case 'e':									//���������������
		rb.ohyeah();
		rb2.ohyeah();
		rb.isfun = !rb.isfun;
		rb2.isfun = !rb2.isfun;
		if(rb.isfun)
			glutTimerFunc(10, idelfun, 1);
		break;
	case 'r':									//�л���һ�ӽ�������ӽ�
		cam.first = !cam.first;
		break;
	case 't':									//�л������ӽǣ���ʱ���ܿ���ʷ�ٷ��ƶ�
		cam.free = !cam.free;
		break;
	case '1':									//��1��-��5���ֱ��ʾ��ͬ���ֳַ��飬������л�
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
		npc = !npc;								//'6'�����Ƿ����npc
		break;
	}
	
}

//------------------------------------------------------------
//������أ�����ʹ������������ĵ㣬ʵ����ȫ��Ч��
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
//���лص���������Ҫ����һЩȫ�ֶ�Ҫ���еĹ��ܣ�����˵������������Ŀ��ƣ������������ת��
float grivity = 0.01;		//��������
void idelfunc() {
	if (!cam.free&&!checkdep(rb.pos.x, rb.pos.y- grivity, rb.pos.z)) {
		rb.pos.y -= grivity;						//����ǰ�����½����½�
	}										
	rb.thlta[handkeepThlta] += 0.05;				//���������������ת
	cursor();										//������
	glutPostRedisplay();
}

//----------------------------------------------------------------------------
//������
int main(int argc, char** argv){
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH );
	glutInitWindowSize(windows_width, windows_height);
	glutInitContextVersion(3, 2);
	glutInitContextProfile(GLUT_CORE_PROFILE);
	glutCreateWindow("��ΰ��_2018161063_��ĩ����ҵ");
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
