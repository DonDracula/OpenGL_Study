#include<Gl/glut.h>
#include<math.h>
#define WIDTH 400
#define HEIGH 400

#define ColoredVertex(c,v) do{glColor3fv(c),glVertex3fv(v);}while(0)

GLfloat angle =0.0f;
void myDisplay(void)
{
	static int list =0;
	if(list ==0)
	{
		GLfloat
			PointA[] = {0.5f,-sqrt(6.0f)/12,-sqrt(3.0f)/6},
			PointB[]={-0.5f,-sqrt(6.0f)/12,-sqrt(3.0f)/6},
			PointC[]={0.0f,-sqrt(6.0f)/12,sqrt(3.0f)/3},
			PointD[]={0.0f,sqrt(6.0f)/4,0};
		GLfloat
			ColorR[] ={1,0,0},
			ColorG[] = {0,1,0},
			ColorB[] ={0,0,1},
			ColorY[] ={1,1,0};

		list = glGenLists(1);
		glNewList(list,GL_COMPILE);
		glBegin(GL_TRIANGLES);
		// •½–ÊABC
		ColoredVertex(ColorR,PointA);
		ColoredVertex(ColorG,PointB);
		ColoredVertex(ColorB,PointC);
		// •½–ÊACD
		ColoredVertex(ColorR, PointA);
		ColoredVertex(ColorB, PointC);
		ColoredVertex(ColorY, PointD);
		// •½–ÊCBD
		ColoredVertex(ColorB, PointC);
		ColoredVertex(ColorG, PointB);
		ColoredVertex(ColorY, PointD);
		// •½–ÊBAD
		ColoredVertex(ColorG, PointB);
		ColoredVertex(ColorR, PointA);
		ColoredVertex(ColorY, PointD);
		glEnd();
		glEndList();

		glEnable(GL_DEPTH_TEST);
	}

	glClear(GL_COLOR_BUFFER_BIT |GL_DEPTH_BUFFER_BIT);
	glPushMatrix();
	glRotatef(angle,1,0.5f,0);
	glCallList(list);
	glPopMatrix();
	glutSwapBuffers();
}

void myIdle(void)
{
	++angle;
	if(angle >=360)
		angle =0.0f;
	myDisplay();
}

int main(int argc, char* argv[])
{
	glutInit(&argc,argv);
	glutInitDisplayMode(GLUT_RGBA|GLUT_DOUBLE);
	glutInitWindowPosition(200,200);
	glutInitWindowSize(WIDTH,HEIGH);
	glutCreateWindow("OpenGL glList Demo");
	glutDisplayFunc(&myDisplay);
	glutIdleFunc(&myIdle);
	glutMainLoop();
	return 0;
}