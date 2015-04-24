#include <GL/glut.h>

#include <math.h>
const GLfloat Pi = 3.1415926536f;
void myDisplay(void)
{
     GLfloat a = 1 / (2-2*cos(72*Pi/180));
     GLfloat bx = a * cos(18 * Pi/180);
     GLfloat by = a * sin(18 * Pi/180);
     GLfloat cy = -a * cos(18 * Pi/180);
     GLfloat
         PointA[2] = { 0, a },
         PointB[2] = { bx, by },
         PointC[2] = { 0.5, cy },
         PointD[2] = { -0.5, cy },
         PointE[2] = { -bx, by };

     glClear(GL_COLOR_BUFFER_BIT);
     // 按照A->C->E->B->D->A的顺序，可以一笔将五角星画出
	// glPointSize(5.0f);  //设置点大小
	 // glLineWidth(2.0f); //设置线的粗细
	 glEnable(GL_LINE_STIPPLE);//设置虚线
	 glLineStipple(2,0x0F0F);
	 glLineWidth(10.0f);
     glBegin(GL_LINE_LOOP);
         glVertex2fv(PointA);
         glVertex2fv(PointC);
         glVertex2fv(PointE);
         glVertex2fv(PointB);
         glVertex2fv(PointD);
     glEnd();
     glFlush();
}


int main(int argc, char *argv[])

{

     glutInit(&argc, argv);

     glutInitDisplayMode(GLUT_RGB | GLUT_SINGLE);

     glutInitWindowPosition(100, 100);

     glutInitWindowSize(400, 400);

     glutCreateWindow("第一个OpenGL程序");

     glutDisplayFunc(&myDisplay);

     glutMainLoop();

     return 0;

}