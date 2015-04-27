#include <GL/glut.h>
#include<stdio.h>

#define WIDTH 400
#define HEIGHT 400

static GLfloat angle = 0.0f;

void myDispaly(void )
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);//清空深度缓冲和颜色缓冲

	// 创建透视效果视图
	glMatrixMode(GL_PROJECTION);//启动投影矩阵
	glLoadIdentity();//把当前矩阵设置为单位矩阵
	gluPerspective(90.0f, 1.0f, 1.0f, 20.0f);//将当前可视空间设置位透视投影空间
	glMatrixMode(GL_MODELVIEW);//指定当前视图为模型视图矩阵
	glLoadIdentity();
	gluLookAt(0.0, 5.0, -10.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);

	//定义太阳光源，白色光源
	{
		GLfloat sun_light_position[] = {0.0f,0.0f,0.0f,1.0f};
		GLfloat sun_light_ambient[] = {0.0f,0.0f,0.0f,1.0f};
		GLfloat sun_light_diffuse[] = {1.0f,1.0f,1.0f,1.0f};
		GLfloat sun_light_specular[]={1.0f,1.0f,1.0f,1.0f};

		//GL_AMBIENT、GL_DIFFUSE、GL_SPECULAR属性。这三个属性表示了光源所发出的光的反射特性（以及颜色）。
		//GL_POSITION属性。表示光源所在的位置。
		glLightfv(GL_LIGHT0,GL_POSITION,sun_light_position);
		glLightfv(GL_LIGHT0,GL_AMBIENT,sun_light_ambient);
		glLightfv(GL_LIGHT0,GL_DIFFUSE,sun_light_diffuse);
		glLightfv(GL_LIGHT0,GL_SPECULAR,sun_light_specular);

		glEnable(GL_LIGHT0);//使用0号光源
		glEnable(GL_LIGHTING);//使用光照
		glEnable(GL_DEPTH_TEST);//启动深度测试，使绘制图形有遮挡效果
	}
	//定义太阳的材质，并绘制太阳
	{
		GLfloat sun_mat_ambient[] ={0.0f,0.0f,0.0f,1.0f};
		GLfloat sun_mat_diffuse[] ={0.0f,0.0f,0.0f,1.0f};
		GLfloat sun_mat_specular[]={0.0f,0.0f,0.0f,1.0f};
		GLfloat sun_mat_emission[]= {0.5f,0.0f,0.0f,1.0f};
		GLfloat sun_mat_shininess   = 0.0f;
		/*GL_SHININESS属性。称为“镜面指数”，取值范围是0到128。值越小，表示材质越粗糙，点光源发射的光线照射到上面，
			也可以产生较大的亮点。值越大，表示材质越类似于镜面，光源照射到上面后，产生较小的亮点。
		  GL_EMISSION属性。该属性由四个值组成，表示一种颜色。OpenGL认为该材质本身就微微的向外发射光线，以至于眼睛感觉到它有这样的颜色，
			 但这光线又比较微弱，以至于不会影响到其它物体的颜色。
		*/
		glMaterialfv(GL_FRONT,GL_AMBIENT,sun_mat_ambient);
		glMaterialfv(GL_FRONT,GL_DIFFUSE,sun_mat_diffuse);
		glMaterialfv(GL_FRONT,GL_SPECULAR,sun_mat_specular);
		glMaterialfv(GL_FRONT,GL_EMISSION,sun_mat_emission);
		glMaterialf(GL_FRONT,GL_SHININESS,sun_mat_shininess);

		glutSolidSphere(2.0f,40,32);
	}
	//定义地球的材质，并绘制地球
	{
		GLfloat earth_mat_ambient[] ={0.0f,0.0f,0.5f,1.0f};
		GLfloat earth_mat_diffuse[]={0.0f,0.0f,0.5f,1.0f};
		GLfloat earth_mat_specular[] ={0.0f,0.0f,1.0f,1.0f};
		GLfloat earth_mat_emission[]={0.0f,0.0f,0.0f,1.0f};
		GLfloat earth_mat_shininess = 30.0f;

		glMaterialfv(GL_FRONT,GL_AMBIENT,earth_mat_ambient);
		glMaterialfv(GL_FRONT,GL_DIFFUSE,earth_mat_diffuse);
		glMaterialfv(GL_FRONT,GL_SPECULAR,earth_mat_specular);
		glMaterialfv(GL_FRONT,GL_EMISSION,earth_mat_emission);
		glMaterialf(GL_FRONT,GL_SHININESS,earth_mat_shininess);


		glRotatef(angle,0.0f,-1.0f,0.0f);
		glTranslatef(5.0f,0.0f,0.0f);
		glutSolidSphere(2.0f,40,32);
	}
	//交换两个缓冲区指针。
	glutSwapBuffers();
}

/*
1.在调用glutInitDisplayMode函数时， 开启GLUT_DOUBLE，即glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);。这里将我们惯用的GLUT_SINGLE替换为GLUT_DOUBLE，意为要使用双缓冲而非单缓冲。
2. 调用glutDisplayFunc(display)注册回调函数时， 在回调函数中所有绘制操作完成后调用glutSwapBuffers()交换两个缓冲区指针。
3. 调用glutIdleFunc注册一个空闲时绘制操作函数， 注册的这个函数再调用display函数。
*/
void myIdle(void)
{
angle +=1.0f;
if(angle >=360.0f)
	angle =0.0f;
myDispaly();
}

int main(int argc,char*argv[])
{
	glutInit(&argc,argv);
	glutInitDisplayMode(GLUT_RGBA|GLUT_DOUBLE);
	glutInitWindowPosition(200,200);
	glutInitWindowSize(WIDTH,HEIGHT);
	glutCreateWindow("OpenGl Light Demo");
	glutDisplayFunc(&myDispaly);
	glutIdleFunc(&myIdle);
	glutMainLoop();
	return 0;

}