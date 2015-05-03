#include <GL/glut.h>
#include <stdio.h>
#define FileName "car.bmp"

static GLint ImageWidth;
static GLint ImageHeight;
static GLint PixelLength;
static GLubyte* PixelData;

void display(void)
{
	//绘制像素
	glDrawPixels(ImageWidth,ImageHeight,GL_BGR_EXT,GL_UNSIGNED_BYTE,PixelData);
	//完成绘制
	glutSwapBuffers();
}

int main(int argc,char*argv[])
{
	//打开文件
	FILE*pFile= fopen(FileName,"rb");

	if(pFile ==0){
		printf("nooooooooooooooooo");
		exit(0);}
	//读取图像的大小信息
	fseek(pFile,0x0012,SEEK_SET);//移动到0x0012位置
	fread(&ImageWidth,sizeof(ImageWidth),1,pFile);//读取图像的宽度
	fread(&ImageHeight,sizeof(ImageHeight),1,pFile);//读取图像的高度

	PixelLength=ImageWidth*3;//每行数据长度大致为图像宽度乘以每像素的字节数
	while(PixelLength%4!=0)//修正LineLength，使其为4的倍数
		++PixelLength;
	PixelLength*=ImageHeight; // 数据总长 = 每行长度x 图像高度

	//读取像素数据
	PixelData =(GLubyte*)malloc(PixelLength);
	if(PixelData==0)
		exit(0);

	fseek(pFile,54,SEEK_SET);
	fread(PixelData,PixelLength,1,pFile);
	//关闭文件
	fclose(pFile);
	//初始化GLUT并运行
	glutInit(&argc,argv);
	glutInitDisplayMode(GLUT_DOUBLE|GLUT_RGBA);
	glutInitWindowPosition(100,100);
	glutInitWindowSize(ImageWidth,ImageHeight);
	glutCreateWindow(FileName);
	glutDisplayFunc(&display);
	glutMainLoop();
	//释放内存，
	//事实上，glutMainLoop函数永远不会反悔，这里也永远不会到达
	//该程序在结束时操作系统会自动回收所有内存
	free(PixelData);
	return 0;

}