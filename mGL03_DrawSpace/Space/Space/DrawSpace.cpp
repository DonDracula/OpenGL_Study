#include <GL/glut.h>
#include <time.h>
#include <math.h>
#include<stdio.h>

double CalFrequency();
//太阳，月亮和地球
//假设每个月30天
//一年十二个月，总共360天
static int day = 200; // day的变化：从0到359
void myDisplay(void)
{
	double FPS =CalFrequency();
	printf("FPS = %f\n",FPS);
	glEnable(GL_DEPTH_TEST);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(75, 1, 1, 40000000);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(0, -20000000, 20000000, 0, 0, 0, 0, 0, 1);

	// 绘制红色的“太阳”
	glColor3f(1.0f, 0.0f, 0.0f);
	glutSolidSphere(6960000, 20, 20);
	// 绘制蓝色的“地球”
	glColor3f(0.0f, 0.0f, 1.0f);
	glRotatef(day/360.0*360.0, 0.0f, 0.0f, -1.0f);
	glTranslatef(15000000, 0.0f, 0.0f);
	glutSolidSphere(1594500, 20, 20);
	// 绘制黄色的“月亮”
	glColor3f(1.0f, 1.0f, 0.0f);
	glRotatef(day/30.0*360.0 - day/360.0*360.0, 0.0f, 0.0f, -1.0f);
	glTranslatef(3800000, 0.0f, 0.0f);
	glutSolidSphere(434500, 20, 20);

	glFlush();
	glutSwapBuffers();
}

double CalFrequency(){
static int count ;
static double save;
	static clock_t last,current;
	double timegap;
	++count ;
	if(count<=50)
		return save;
	count =0;
	last =current;
	current =clock();
	timegap =(current-last)/(double)CLK_TCK;
	save=50.0f/timegap;
	return save;
}

void myIdele(void)
{//新的函数，在空闲时调用，作用是把日期往后移动一天并重新绘制，达到动画效果
	++day;
	if(day>=360)
		day=0;
	myDisplay();
}

int main(int argc, char *argv[])

{
	glutInit(&argc, argv);

	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);//修改了参数为GLUT_DOUBLE

	glutInitWindowPosition(100, 100);

	glutInitWindowSize(400, 400);

	glutCreateWindow("DrawSpace");

	glutDisplayFunc(&myDisplay);
	glutIdleFunc(&myIdele);	//加入新循环

	glutMainLoop();

	return 0;

}