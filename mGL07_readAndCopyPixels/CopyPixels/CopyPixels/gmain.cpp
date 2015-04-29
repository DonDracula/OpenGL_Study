#include <GL/glut.h>
#include <stdio.h>

#define WindowWidth 400
#define WindowHeight 400

//grab函数，抓取窗口中的像素，假设窗口宽和高为windowwidth，和windowheight
#define BMP_Header_Length 54
void grab(void)
{
     FILE*     pDummyFile;
     FILE*     pWritingFile;
     GLubyte* pPixelData;
     GLubyte   BMP_Header[BMP_Header_Length];
     GLint     i, j;
     GLint     PixelDataLength;

     // 计算像素数据的实际长度
     i = WindowWidth * 3;    // 得到每一行的像素数据长度
     while( i%4 != 0 )       // 补充数据，直到i是的倍数
         ++i;                // 本来还有更快的算法，
                            // 但这里仅追求直观，对速度没有太高要求
     PixelDataLength = i * WindowHeight;

     // 分配内存和打开文件
     pPixelData = (GLubyte*)malloc(PixelDataLength);
     if( pPixelData == 0 )
         exit(0);

     pDummyFile = fopen("dummy.bmp", "rb");
     if( pDummyFile == 0 )
         exit(0);

     pWritingFile = fopen("grab.bmp", "wb");
     if( pWritingFile == 0 )
         exit(0);

     // 读取像素
     glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
     glReadPixels(0, 0, WindowWidth, WindowHeight,
         GL_BGR_EXT, GL_UNSIGNED_BYTE, pPixelData);

     // 把dummy.bmp的文件头复制为新文件的文件头
     fread(BMP_Header, sizeof(BMP_Header), 1, pDummyFile);
     fwrite(BMP_Header, sizeof(BMP_Header), 1, pWritingFile);
     fseek(pWritingFile, 0x0012, SEEK_SET);
     i = WindowWidth;
     j = WindowHeight;
     fwrite(&i, sizeof(i), 1, pWritingFile);
     fwrite(&j, sizeof(j), 1, pWritingFile);

     // 写入像素数据
     fseek(pWritingFile, 0, SEEK_END);
     fwrite(pPixelData, PixelDataLength, 1, pWritingFile);

     // 释放内存和关闭文件
     fclose(pDummyFile);
     fclose(pWritingFile);
     free(pPixelData);
}

void mydisplay(void)
{
	glClear(GL_COLOR_BUFFER_BIT);

	glBegin(GL_TRIANGLES);
	glColor3f(1.0f,0.0f,0.0f); glVertex2f(0.0f,0.0f);
	glColor3f(0.0f,1.0f,0.0f);glVertex2f(1.0f,0.0f);
	glColor3f(0.0f,0.0f,1.0f); glVertex2f(0.5f,1.0f);
	glEnd();

	glPixelZoom(-0.5f,-0.5f);
	glRasterPos2i(1,1);
	glCopyPixels(WindowWidth/2,WindowHeight/2,WindowWidth/2,WindowHeight/2,GL_COLOR);

	glutSwapBuffers();
	grab();
}

int main(int argc,char*argv[])
{
	glutInit(&argc,argv);
	glutInitDisplayMode(GLUT_RGBA|GLUT_DOUBLE);
	glutInitWindowPosition(100,100);
	glutInitWindowSize(WindowWidth,WindowHeight);
	glutCreateWindow("OpenGL read and copy pixels");
	glutDisplayFunc(&mydisplay);
	glutMainLoop();

	return 0;
}