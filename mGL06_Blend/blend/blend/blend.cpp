#include <GL/glut.h>
#define WIDTH 400
#define HEIGHT 400

void setLight(void)
{
	static const GLfloat light_position[]={1.0f,1.0f,-1.0f,1.0f};
	static const GLfloat light_ambient[] ={0.2f,0.2f,0.2f,1.0f};
	static const GLfloat light_diffuse[]={1.0f,1.0f,1.0f,1.0f};
	static const GLfloat light_specular[]={1.0f,1.0f,1.0f,1.0f};

	glLightfv(GL_LIGHT0,GL_POSITION,light_position);
	glLightfv(GL_LIGHT0,GL_AMBIENT,light_ambient);
	glLightfv(GL_LIGHT0,GL_DIFFUSE,light_diffuse);
	glLightfv(GL_LIGHT0,GL_SPECULAR,light_specular);

	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHTING);
	glEnable(GL_DEPTH_TEST);
}

void setMatirial(const GLfloat mat_diffuse[4],GLfloat mat_shininess)
{
	static const GLfloat mat_specular[] = {0.0f,0.0f,0.0f,1.0f};
	static const GLfloat mat_emission[] ={0.0f,0.0f,0.0f,1.0f};
	
	glMaterialfv(GL_FRONT,GL_AMBIENT_AND_DIFFUSE ,mat_diffuse);
	glMaterialfv(GL_FRONT,GL_SPECULAR,mat_specular);
	glMaterialfv(GL_FRONT,GL_EMISSION,mat_emission);
	glMaterialf(GL_FRONT,GL_SHININESS,mat_shininess);

}

void myDisPlay(void)
{
	//define the matirial colors
	const static GLfloat red_color[]={1.0f,0.0f,0.0f,1.0f};
	const static GLfloat green_color[]={0.0f,1.0f,0.0f,0.3333f};
	const static GLfloat blue_color[]={0.0f,0.0f,1.0f,0.5f};
	// clean the screen
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

	//enable the blend and set the function of it
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
	//set lifht
	setLight();
	//draw an opacity red sphere
	setMatirial(red_color,30.0);
	glPushMatrix();
	glTranslatef(0.0f,0.0f,0.5f);
	glutSolidSphere(0.3,30,30);
	glPopMatrix();
	//turn the depthmask to false
	glDepthMask(GL_FALSE);
	//draw a half-tansparent blue sphere 
	setMatirial(blue_color,30.0);
	glPushMatrix();
	glTranslatef(0.2f,0.0f,-0.5f);
	glutSolidSphere(0.2,30,30);
	glPopMatrix();
	//draw a half-transparent green sphere
	setMatirial(green_color,30.0);
	glPushMatrix();
	glTranslatef(0.1f,0.0f,0.0f);
	glutSolidSphere(0.15,30,30);
	glPopMatrix();
	//turn the depthmask to true,which can be read and write
	glDepthMask(GL_TRUE);
	glutSwapBuffers();

}

int main(int argc,char*argv[])
{
glutInit(&argc,argv);
glutInitDisplayMode(GLUT_RGBA|GLUT_DOUBLE);
glutInitWindowPosition(200,200);
glutInitWindowSize(WIDTH,HEIGHT);
glutCreateWindow("GL Blend");
glutDisplayFunc(&myDisPlay);
glutMainLoop();
return 0;
}