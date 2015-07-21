#pragma comment (lib,"glew32.lib")

//OpenGL
#include <GL/glew.h>
#include <GL/glut.h>

#include <iostream>
#include <stdio.h>
#include <vector>
#include <string>

#include "rx_trackball.h"
#define BMP_Header_Length 54

using namespace std;
//define program name
const string RX_PROGRAM_NAME = "TextureFilter Demo";

//windows size
int g_iWinW = 720;		//Width of the window
int g_iWinH = 720;		//Height of the window
int g_iWinX = 100;		//x position of the window
int g_iWinY = 100;		//y position of the window

bool g_bIdle = false;		//state of the idle
//int g_iDraw = RXD_FACE;	//!< drawing flag

double g_fAng = 0.0;	//!< 描画回転角

//background color
double g_fBGColor[4] = {0.8,0.9,1.0,1.0};

//rxtrackball,to control the viewpoint
rxTrackball g_tbView;

double g_fDt = 0.03;				//update time

bool	light;				// Lighting ON/OFF ( NEW )
bool	lp;					// L Pressed? ( NEW )
bool	fp;					// F Pressed? ( NEW )

GLfloat	xrot;				// X Rotation
GLfloat	yrot;				// Y Rotation
GLfloat xspeed;				// X Rotation Speed
GLfloat yspeed;				// Y Rotation Speed
GLfloat	z=-5.0f;			// Depth Into The Screen

GLfloat LightAmbient[]=		{ 0.5f, 0.5f, 0.5f, 1.0f };
GLfloat LightDiffuse[]=		{ 1.0f, 1.0f, 1.0f, 1.0f };
GLfloat LightPosition[]=	{ 0.0f, 0.0f, 2.0f, 1.0f };

GLuint	filter;				// Which Filter To Use
GLuint	texture[3];			// Storage For 3 Textures

//function declaration & definition 
void OnExitApp(int id =-1);
void Idle(void);
void Timer(int value);
void CleanGL(void);

/*
函数grab，抓去窗口中的像素，窗口的宽位windowidth，高度为windowheight
*/
#define BMP_Header_Length 54
void grab(void)
{
	glReadBuffer(GL_FRONT);
 FILE* pDummyFile;
 FILE* pWritingFile;
 GLubyte* pPixelData;
 GLubyte BMP_Header[BMP_Header_Length];
 GLint i,j;
 GLint PixelDataLength;
 //计算像素数据的实际长度
 i=g_iWinW*3;//得到每一行像素数据的长度
 while(i%4!=0)//补充数据，使其是4的倍数
	 ++i;

 PixelDataLength =i*g_iWinH;
 //分配内存，打开文件
 pPixelData = (GLubyte*)malloc(PixelDataLength);
 if(pPixelData==0)
	 exit(0);

 pDummyFile =fopen("dummy.bmp","rb");
 if(pDummyFile==0)
	 exit(0);

 pWritingFile = fopen("grab.bmp","wb");
 if(pWritingFile ==0)
	 exit(0);

 //读取像素
 glPixelStorei(GL_UNPACK_ALIGNMENT,4);
 glReadPixels(0,0,g_iWinW,g_iWinH,GL_BGR_EXT,GL_UNSIGNED_BYTE,pPixelData);
 //吧dummy.bmp的文件头复制为新文件的文件头
 fread(BMP_Header,sizeof(BMP_Header),1,pDummyFile);
 fwrite(BMP_Header,sizeof(BMP_Header),1,pWritingFile);
 fseek(pWritingFile,0x0012,SEEK_SET);
 i=g_iWinW;
 j=g_iWinH;
 fwrite(&i,sizeof(i),1,pWritingFile);
 fwrite(&j,sizeof(j),1,pWritingFile);
 //写入像素数据
 fseek(pWritingFile,0,SEEK_END);
 fwrite(pPixelData,PixelDataLength,1,pWritingFile);
 //释放内存并关闭文件
 fclose(pDummyFile);
 fclose(pWritingFile);
 free(pPixelData);
}

/*
函数power_of_tow
查看整数是否为2的整数次方，如果是，返回1，否则返回0
*/
int power_of_two(int n)
{
if(n<= 0)
	return 0;
return(n&(n-1)) ==0;
}
/*
函数load_texture
读取一个bmp文件作为纹理
如果失败，返回0，否则，返回纹理编号
*/
GLuint load_texture(const char*file_name)
{
GLint width,height,total_bytes;
GLubyte*pixels =0;
GLuint last_texture_ID,texture_ID =0;

//打开文件，如果失败，返回
FILE*pFile =fopen(file_name,"rb");
if(pFile == 0)
	return 0;

//读取文件中图像的宽度和高度
fseek(pFile,0x0012,SEEK_SET);
fread(&width,4,1,pFile);
fread(&height,4,1,pFile);
fseek(pFile,BMP_Header_Length,SEEK_SET);

//计算每行像素所占用字节数，并根据此数据计算总像素字节数
{
GLint line_bytes =width*3;
while(line_bytes%4!=0)
	++line_bytes;
total_bytes =line_bytes*height;
}
//根据总像素字节数分配内存
pixels =(GLubyte*)malloc(total_bytes);
if(pixels == 0)
{
	fclose(pFile);
	return 0;
}

//读取像素数据
if(fread(pixels,total_bytes,1,pFile)<=0)
{
free(pixels);
fclose(pFile);
return 0;
}
/*
在旧版本的opengl中，如果图像的宽度和高度不是整数次方，则需要进行缩放
这里，没有检查OpenGL的版本，出于对兼容性的考虑，按旧版本处理
同时，不管是新旧版本，当图像的宽度和高度超过当前OpenGL实现所支持的最大值时，也要进行缩放
*/
{
	GLint max;
	glGetIntegerv(GL_MAX_TEXTURE_SIZE,&max);
	if(!power_of_two(width)||!power_of_two(height)||width>max||height>max)
	{
	const GLint new_width =256;
	const GLint new_height =256;//规定缩放后新的边长大小位正方形
	GLint new_line_bytes,new_total_bytes;
	GLubyte*new_pixels=0;

	//计算每行需要的字节数和总子节数
	new_line_bytes =new_width*3;
	while(new_line_bytes%4!=0)
		++new_line_bytes;
	new_total_bytes=new_line_bytes*new_height;
	//分配内存
	new_pixels=(GLubyte*)malloc(new_total_bytes);
	if(new_pixels ==0)
	{
	free(pixels);
	fclose(pFile);
	return 0;

	}

	//进行像素缩放
	gluScaleImage(GL_RGB,width,height,GL_UNSIGNED_BYTE,pixels,
		new_width,new_height,GL_UNSIGNED_BYTE,new_pixels);

	//释放原来的像素数据，把pixel指向新的像素数据，并重新设施width和height
	free(pixels);
	pixels =new_pixels;
	width =new_width;
	height=new_height;
	}
}

//分配一个新的纹理编号
glGenTextures(1,&texture_ID);
if(texture_ID==0)
{
free(pixels);
fclose(pFile);
return 0;
}

//绑定新的纹理，载入纹理并设置纹理参数
//在绑定前，先获得原来绑定的纹理编号，以便在最后进行恢复
glGetIntegerv(GL_TEXTURE_BINDING_2D,(GLint*)&last_texture_ID);
glBindTexture(GL_TEXTURE_2D,texture_ID);
glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT);
glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_REPEAT);
glTexEnvf(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_REPLACE);
glTexImage2D(GL_TEXTURE_2D,0,GL_RGB,width,height,0,GL_BGR_EXT,GL_UNSIGNED_BYTE,pixels);
glBindTexture(GL_TEXTURE_2D,last_texture_ID);

//之前位pixels分配的内存可在使用glteximage2d后释放
//因为此时像素数据已经被opengl另行保存了一份（可能被保存到专门的图形硬件中）
free(pixels);
return texture_ID;
}


//两个纹理对象的编号
GLuint texGround;

//control procedure
//idle control,in which,ON is true,false is OFF
void SwitchIdle(int on)
{
	g_bIdle = (on==-1)?!g_bIdle:(on?true:false);
//	if(g_bIdle) glutTimerFunc(g_fDt*1000,Timer,0);
	cout<<"idle"<<(g_bIdle?"on":"off")<<endl;
}

//display on fullscreen / window
void SwitchFullScreen(void)
{
	static int fullscreen =0;
	static int pos0[2] = {0,0};
	static int win0[2] = {500,500};
	if(fullscreen){
		glutPositionWindow(pos0[0],pos0[1]);
		glutReshapeWindow(win0[0],win0[1]);
	}
	else{
		pos0[0] = glutGet(GLUT_WINDOW_X);
		pos0[1] = glutGet(GLUT_WINDOW_Y);
		win0[0] = glutGet(GLUT_WINDOW_WIDTH);
		win0[1] = glutGet(GLUT_WINDOW_HEIGHT);

		glutFullScreen();
	}
	fullscreen ^=1;
}

//Exit the application
void OnExitApp(int id)
{
	CleanGL();
	exit(1);
}

//display function
void Projection(void)
{
	gluPerspective(45.0f,(float)g_iWinW/(float)g_iWinH,0.2f,1000.0f);
}

void RenderScene(void)
{
	glPushMatrix();
	glEnable(GL_LIGHTING);
	glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);

	glColorMaterial(GL_FRONT_AND_BACK,GL_AMBIENT_AND_DIFFUSE);
	glEnable(GL_COLOR_MATERIAL);

	//draw object------------------------------------------------------------
    // 使用“地”纹理绘制土地

    glBindTexture(GL_TEXTURE_2D, texGround);
	glBegin(GL_QUADS);
		// Front Face
		glNormal3f( 0.0f, 0.0f, 1.0f);
		glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f, -1.0f,  1.0f);
		glTexCoord2f(1.0f, 0.0f); glVertex3f( 1.0f, -1.0f,  1.0f);
		glTexCoord2f(1.0f, 1.0f); glVertex3f( 1.0f,  1.0f,  1.0f);
		glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f,  1.0f,  1.0f);
		// Back Face
		glNormal3f( 0.0f, 0.0f,-1.0f);
		glTexCoord2f(1.0f, 0.0f); glVertex3f(-1.0f, -1.0f, -1.0f);
		glTexCoord2f(1.0f, 1.0f); glVertex3f(-1.0f,  1.0f, -1.0f);
		glTexCoord2f(0.0f, 1.0f); glVertex3f( 1.0f,  1.0f, -1.0f);
		glTexCoord2f(0.0f, 0.0f); glVertex3f( 1.0f, -1.0f, -1.0f);
		// Top Face
		glNormal3f( 0.0f, 1.0f, 0.0f);
		glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f,  1.0f, -1.0f);
		glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f,  1.0f,  1.0f);
		glTexCoord2f(1.0f, 0.0f); glVertex3f( 1.0f,  1.0f,  1.0f);
		glTexCoord2f(1.0f, 1.0f); glVertex3f( 1.0f,  1.0f, -1.0f);
		// Bottom Face
		glNormal3f( 0.0f,-1.0f, 0.0f);
		glTexCoord2f(1.0f, 1.0f); glVertex3f(-1.0f, -1.0f, -1.0f);
		glTexCoord2f(0.0f, 1.0f); glVertex3f( 1.0f, -1.0f, -1.0f);
		glTexCoord2f(0.0f, 0.0f); glVertex3f( 1.0f, -1.0f,  1.0f);
		glTexCoord2f(1.0f, 0.0f); glVertex3f(-1.0f, -1.0f,  1.0f);
		// Right face
		glNormal3f( 1.0f, 0.0f, 0.0f);
		glTexCoord2f(1.0f, 0.0f); glVertex3f( 1.0f, -1.0f, -1.0f);
		glTexCoord2f(1.0f, 1.0f); glVertex3f( 1.0f,  1.0f, -1.0f);
		glTexCoord2f(0.0f, 1.0f); glVertex3f( 1.0f,  1.0f,  1.0f);
		glTexCoord2f(0.0f, 0.0f); glVertex3f( 1.0f, -1.0f,  1.0f);
		// Left Face
		glNormal3f(-1.0f, 0.0f, 0.0f);
		glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f, -1.0f, -1.0f);
		glTexCoord2f(1.0f, 0.0f); glVertex3f(-1.0f, -1.0f,  1.0f);
		glTexCoord2f(1.0f, 1.0f); glVertex3f(-1.0f,  1.0f,  1.0f);
		glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f,  1.0f, -1.0f);
	 glEnd();

	//drawing teh rope ends here
	glPopMatrix();
}

void display1(void)
{
// 清除屏幕
     glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

     // 设置视角
     glMatrixMode(GL_PROJECTION);
     glLoadIdentity();
     gluPerspective(95, 1, 1, 21);
     glMatrixMode(GL_MODELVIEW);
     glLoadIdentity();
     gluLookAt(1, 5, 5, 0, 0, 0, 0, 0, 1);

     // 使用“地”纹理绘制土地
     glBindTexture(GL_TEXTURE_2D, texGround);
     glBegin(GL_QUADS);
         glTexCoord2f(0.0f, 0.0f); glVertex3f(-8.0f, -8.0f, 0.0f);
         glTexCoord2f(0.0f, 5.0f); glVertex3f(-8.0f, 8.0f, 0.0f);
         glTexCoord2f(5.0f, 5.0f); glVertex3f(8.0f, 8.0f, 0.0f);
         glTexCoord2f(5.0f, 0.0f); glVertex3f(8.0f, -8.0f, 0.0f);
     glEnd();
     // 使用“墙”纹理绘制栅栏
   //  glBindTexture(GL_TEXTURE_2D, texWall);
     glBegin(GL_QUADS);
         glTexCoord2f(0.0f, 0.0f); glVertex3f(-6.0f, -3.0f, 0.0f);
         glTexCoord2f(0.0f, 1.0f); glVertex3f(-6.0f, -3.0f, 1.5f);
         glTexCoord2f(5.0f, 1.0f); glVertex3f(6.0f, -3.0f, 1.5f);
         glTexCoord2f(5.0f, 0.0f); glVertex3f(6.0f, -3.0f, 0.0f);
     glEnd();

     // 旋转后再绘制一个
     glRotatef(-90, 0, 0, 1);
     glBegin(GL_QUADS);
         glTexCoord2f(0.0f, 0.0f); glVertex3f(-6.0f, -3.0f, 0.0f);
         glTexCoord2f(0.0f, 1.0f); glVertex3f(-6.0f, -3.0f, 1.5f);
         glTexCoord2f(5.0f, 1.0f); glVertex3f(6.0f, -3.0f, 1.5f);
         glTexCoord2f(5.0f, 0.0f); glVertex3f(6.0f, -3.0f, 0.0f);
     glEnd();

	 //再旋转45度
     glRotatef(-135, 0, 0, 1);
     glBegin(GL_QUADS);
         glTexCoord2f(0.0f, 0.0f); glVertex3f(-8.0f, -2.0f, 0.0f);
         glTexCoord2f(0.0f, 1.0f); glVertex3f(-8.0f, -2.0f, 1.5f);
         glTexCoord2f(5.0f, 1.0f); glVertex3f(8.0f, -2.0f, 1.5f);
         glTexCoord2f(5.0f, 0.0f); glVertex3f(8.0f, -2.0f, 0.0f);
     glEnd();

     // 交换缓冲区，并保存像素数据到文件
     glutSwapBuffers();
  //   grab();
}

//event handle
void Display(void)
{

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glPushMatrix();

	g_tbView.Apply();		//mouse control, which allows to rotate and move

	glPushMatrix();
	//glRotated(g_fAng,0,1,0);
	RenderScene();
	glPopMatrix();

	glPopMatrix();

	//word 
	vector<string> strs;
	strs.push_back("\"s\" key : idle on/off");
	strs.push_back("SHIFT+\"f\" key : fullscreen on/off" );
	strs.push_back("\"v\",\"e\",\"f\" key : switch vertex,edge,face drawing");
	//glColor3d(1.0,1.0,1.0);
	//drawString();                 //draw the string view of the teapot

	glutSwapBuffers();
}

//idle handle
void Idle(void)
{
	float dt = 0.002;

	glutPostRedisplay();
}

//init the GL
void InitGL(void)
{

	cout <<"OpenGL ver." << glGetString(GL_VERSION) << endl;

	GLenum err = glewInit();
	if(err == GLEW_OK){
		cout<<"GLEW OK : GLew ver. "<< glewGetString(GLEW_VERSION)<<endl;
	}
	else{
		cout << "GLEW Error :"<<glewGetErrorString(err) << endl;
	}

	//multisample
	GLint buf,sbuf;
	glGetIntegerv(GL_SAMPLER_BUFFER,&buf);
	cout<<"number of sample buffers is "<<buf<<endl;
	glGetIntegerv(GL_SAMPLES,&sbuf);
	cout<<"number of samples is " <<sbuf << endl;

	glClearColor((GLfloat)g_fBGColor[0],(GLfloat)g_fBGColor[1],(GLfloat)g_fBGColor[2],1.0f);		//background
	glClearDepth(1.0f);										//depth buffer setup

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_TEXTURE_2D);
	glDisable(GL_CULL_FACE);

	glEnable(GL_MULTISAMPLE);

	//nomalize the polygon
	glEnable(GL_AUTO_NORMAL);
	glEnable(GL_NORMALIZE);

	//set up the light
	static const GLfloat lpos[] = {0.0f,0.0f,1.0f,1.0f};//position of the light
	static const GLfloat difw[] = {0.7f,0.7f,0.7f,1.0f};//diffuse
	static const GLfloat spec[] = {0.2f,0.2f,0.2f,1.0f};//specular
	static const GLfloat ambi[] = {0.1f,0.1f,0.1f,1.0f};//ambient
	glLightfv(GL_LIGHT0,GL_POSITION,lpos);
	glLightfv(GL_LIGHT0,GL_DIFFUSE,difw);
	glLightfv(GL_LIGHT0,GL_SPECULAR,spec);
	glLightfv(GL_LIGHT0,GL_AMBIENT,ambi);
	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHTING);

	glShadeModel(GL_SMOOTH);							// Select Smooth Shading
	glHint (GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);		// Set Perspective Calculations To Most Accurate

	//init the trackball
	g_tbView.SetScaling(-10.0f);
}

//resize the window
void Resize(int w,int h)
{
	g_iWinW =w;
	g_iWinH = h;
	glViewport(0,0,w,h);
	g_tbView.SetRegion(w,h);

	//set the Parameters of the projection
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	Projection();

	//set up hte modelview
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

/*
set the mouse control button
@param[in] button: mouse button(GLUT_LEFT_BUTTON,GLUT_MIDDLE_BUTTON.GLUT_RIGHT_BUTTON)
@param[in] state: the state of the mouse button(GLUT_UP.GLUT_DOWN)
@param[in] x,y the positon of the mouse button

*/
void Mouse(int button,int state,int x,int y)
{
	if(x<0||y<0) return;

	int mod = glutGetModifiers();		//get the state mode of SHIFT,CTRL,ALT

	if(button == GLUT_LEFT_BUTTON){
		if(state == GLUT_DOWN)
		{
			g_tbView.Start(x,y,mod+1);
		}
		else if(state == GLUT_UP)
		{
			g_tbView.Stop(x,y);
		}
	}
	else if (button == GLUT_MIDDLE_BUTTON){
	}
	else if(button == GLUT_RIGHT_BUTTON)
	{
	}
	glutPostRedisplay();
}
//motion function(put down the button)
//@param[in] x,y :the positon of the mouse
void Motion(int x,int y)
{
	if(x<0 || y<0) return;
	int mod = glutGetModifiers();		//get the state mode of Shift,Ctrl,Alt
	g_tbView.Motion(x,y);
	glutPostRedisplay();
}

//motion function()
void PassiveMotion(int x,int y)
{
	if(x<0 ||y<0) return;
}

/*keybord function
@param[in] key, the type of the key
@param[in] x,y : the current position of the mouse
*/
void Keyboard(unsigned char key, int x, int y)
{
	int mod = glutGetModifiers();		//the state(Shift,Ctrl,Alt)

	switch(key){
	case '\033': // the ASCII code of ESC
		OnExitApp();
		break;

	case ' ':	//idle
		Idle();
		break;

	case 'g':	//animation ON/OFF
		SwitchIdle(-1);
		break;

	case 'F':   //Fullscreen -> Shift+f
		SwitchFullScreen();
		break;

	default:	
		{
			int idx = 0;
		}
		break;
	}
	
	glutPostRedisplay();
}

//special key
void SpecialKey(int key,int x,int y)
{
//	Vec3 ropeConnectionVel;
	switch(key){
	case GLUT_KEY_LEFT:
		
		break;

	case GLUT_KEY_RIGHT:
		
		break;

	case GLUT_KEY_UP:
		
		break;

	case GLUT_KEY_DOWN:
		
		break;

	default:
		break;
	}
	glutPostRedisplay();
}

//clean the GL
void CleanGL(void)
{
//	ropeSimulator->release();					// Release The ropeSimulation
//	delete(ropeSimulator);						// Delete The ropeSimulation
//	ropeSimulator = NULL;
}

//main menu
//@param[in] id :main ID
void OnMainMenu(int id)
{
	Keyboard((unsigned char)id,0,0);
}

//create the right click menu
void InitMenu(void)
{
	//main menu
	glutCreateMenu(OnMainMenu);
	glutAddMenuEntry("------------------------",         -1);
	glutAddMenuEntry("Toggle fullscreen [f]",         'f');
	glutAddMenuEntry("Toggle animation [s]" ,           's');
	glutAddMenuEntry("Step animation [ ]",              ' ');
	glutAddMenuEntry("------------------------",         -1);
	glutAddMenuEntry("Quit [ESC]",                       '\033');
	glutAttachMenu(GLUT_RIGHT_BUTTON);
}



int main(int argc, char* argv[])
{
     // GLUT初始化
     glutInit(&argc, argv);
     glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
     glutInitWindowPosition(100, 100);
	glutInitWindowSize(g_iWinW,g_iWinH);
glutCreateWindow(argv[0]);
	glutDisplayFunc(display1);

     // 在这里做一些初始化
     glEnable(GL_DEPTH_TEST);
     glEnable(GL_TEXTURE_2D);
     texGround = load_texture("ground.bmp");
  //   texWall = load_texture("wall.bmp");

     // 开始显示
     glutMainLoop();

    return 0;
}