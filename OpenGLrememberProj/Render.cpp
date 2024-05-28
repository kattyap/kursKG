#include "Render.h"

#include <sstream>
#include <iostream>

#include <windows.h>
#include <GL\GL.h>
#include <GL\GLU.h>

#include "MyOGL.h"

#include "Camera.h"
#include "Light.h"
#include "Primitives.h"

#include "GUItextRectangle.h"

bool textureMode = true;
bool lightMode = true;
bool textureReplace = true;

//����� ��� ��������� ������
class CustomCamera : public Camera
{
public:
	//��������� ������
	double camDist;
	//���� �������� ������
	double fi1, fi2;


	//������� ������ �� ���������
	CustomCamera()
	{
		camDist = 15;
		fi1 = 1;
		fi2 = 1;
	}


	//������� ������� ������, ������ �� ����� ��������, ���������� �������
	void SetUpCamera()
	{
		//�������� �� ������� ������ ������
		lookPoint.setCoords(0, 0, 0);

		pos.setCoords(camDist * cos(fi2) * cos(fi1),
			camDist * cos(fi2) * sin(fi1),
			camDist * sin(fi2));

		if (cos(fi2) <= 0)
			normal.setCoords(0, 0, -1);
		else
			normal.setCoords(0, 0, 1);

		LookAt();
	}

	void CustomCamera::LookAt()
	{
		//������� ��������� ������
		gluLookAt(pos.X(), pos.Y(), pos.Z(), lookPoint.X(), lookPoint.Y(), lookPoint.Z(), normal.X(), normal.Y(), normal.Z());
	}



}  camera;   //������� ������ ������

//����� ��� ��������� �����
class CustomLight : public Light
{
public:
	CustomLight()
	{
		//��������� ������� �����
		pos = Vector3(1, 1, 3);
	}


	//������ ����� � ����� ��� ���������� �����, ���������� �������
	void  DrawLightGhismo()
	{
		glDisable(GL_LIGHTING);


		glColor3d(0.9, 0.8, 0);
		Sphere s;
		s.pos = pos;
		s.scale = s.scale * 0.08;
		s.Show();

		if (OpenGL::isKeyPressed('G'))
		{
			glColor3d(0, 0, 0);
			//����� �� ��������� ����� �� ����������
			glBegin(GL_LINES);
			glVertex3d(pos.X(), pos.Y(), pos.Z());
			glVertex3d(pos.X(), pos.Y(), 0);
			glEnd();

			//������ ���������
			Circle c;
			c.pos.setCoords(pos.X(), pos.Y(), 0);
			c.scale = c.scale * 1.5;
			c.Show();
		}

	}

	void SetUpLight()
	{
		GLfloat amb[] = { 0.2, 0.2, 0.2, 0 };
		GLfloat dif[] = { 1.0, 1.0, 1.0, 0 };
		GLfloat spec[] = { .7, .7, .7, 0 };
		GLfloat position[] = { pos.X(), pos.Y(), pos.Z(), 1. };

		// ��������� ��������� �����
		glLightfv(GL_LIGHT0, GL_POSITION, position);
		// �������������� ����������� �����
		// ������� ��������� (���������� ����)
		glLightfv(GL_LIGHT0, GL_AMBIENT, amb);
		// ��������� ������������ �����
		glLightfv(GL_LIGHT0, GL_DIFFUSE, dif);
		// ��������� ���������� ������������ �����
		glLightfv(GL_LIGHT0, GL_SPECULAR, spec);

		glEnable(GL_LIGHT0);
	}


} light;  //������� �������� �����

//������ ���������� ����
int mouseX = 0, mouseY = 0;

void mouseEvent(OpenGL* ogl, int mX, int mY)
{
	int dx = mouseX - mX;
	int dy = mouseY - mY;
	mouseX = mX;
	mouseY = mY;

	//������ ���� ������ ��� ������� ����� ������ ����
	if (OpenGL::isKeyPressed(VK_RBUTTON))
	{
		camera.fi1 += 0.01 * dx;
		camera.fi2 += -0.01 * dy;
	}


	//������� ���� �� ���������, � ����� ��� ����
	if (OpenGL::isKeyPressed('G') && !OpenGL::isKeyPressed(VK_LBUTTON))
	{
		LPPOINT POINT = new tagPOINT();
		GetCursorPos(POINT);
		ScreenToClient(ogl->getHwnd(), POINT);
		POINT->y = ogl->getHeight() - POINT->y;

		Ray r = camera.getLookRay(POINT->x, POINT->y);

		double z = light.pos.Z();

		double k = 0, x = 0, y = 0;
		if (r.direction.Z() == 0)
			k = 0;
		else
			k = (z - r.origin.Z()) / r.direction.Z();

		x = k * r.direction.X() + r.origin.X();
		y = k * r.direction.Y() + r.origin.Y();

		light.pos = Vector3(x, y, z);
	}

	if (OpenGL::isKeyPressed('G') && OpenGL::isKeyPressed(VK_LBUTTON))
	{
		light.pos = light.pos + Vector3(0, 0, 0.02 * dy);
	}


}

void mouseWheelEvent(OpenGL* ogl, int delta)
{

	if (delta < 0 && camera.camDist <= 1)
		return;
	if (delta > 0 && camera.camDist >= 100)
		return;

	camera.camDist += 0.01 * delta;

}

void keyDownEvent(OpenGL* ogl, int key)
{
	if (key == 'L')
	{
		lightMode = !lightMode;
	}

	if (key == 'T')
	{
		textureMode = !textureMode;
	}

	if (key == 'R')
	{
		camera.fi1 = 1;
		camera.fi2 = 1;
		camera.camDist = 15;

		light.pos = Vector3(1, 1, 3);
	}

	if (key == 'F')
	{
		light.pos = camera.pos;
	}
}

void keyUpEvent(OpenGL* ogl, int key)
{

}

GLuint texId;
GLuint texId2;

//����������� ����� ������ ��������
void initRender(OpenGL* ogl)
{
	//��������� �������

	//4 ����� �� �������� �������
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

	//��������� ������ ��������� �������
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	//�������� ��������
	glEnable(GL_TEXTURE_2D);


	//������ ����������� ���������  (R G B)
	RGBTRIPLE* texarray;

	//������ ��������, (������*������*4      4, ���������   ����, �� ������� ������������ �� 4 ����� �� ������� �������� - R G B A)
	char* texCharArray;
	int texW, texH;
	OpenGL::LoadBMP("texture.bmp", &texW, &texH, &texarray);
	OpenGL::RGBtoChar(texarray, texW, texH, &texCharArray);



	//���������� �� ��� ��������
	glGenTextures(1, &texId);
	//������ ��������, ��� ��� ����� ����������� � ���������, ����� ����������� �� ����� ��
	glBindTexture(GL_TEXTURE_2D, texId);

	//��������� �������� � �����������, � ���������� ��� ������  ��� �� �����
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texW, texH, 0, GL_RGBA, GL_UNSIGNED_BYTE, texCharArray);

	//�������� ������
	free(texCharArray);
	free(texarray);

	//������� ����
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);


	//������ � ���� ����������� � "������"
	ogl->mainCamera = &camera;
	ogl->mainLight = &light;

	// ������������ �������� : �� ����� ����� ����� 1
	glEnable(GL_NORMALIZE);

	// ���������� ������������� ��� �����
	glEnable(GL_LINE_SMOOTH);


	//   ������ ��������� ���������
	//  �������� GL_LIGHT_MODEL_TWO_SIDE - 
	//                0 -  ������� � ���������� �������� ���������(�� ���������), 
	//                1 - ������� � ���������� �������������� ������� ��������       
	//                �������������� ������� � ���������� ��������� ����������.    
	//  �������� GL_LIGHT_MODEL_AMBIENT - ������ ������� ���������, 
	//                �� ��������� �� ���������
	// �� ��������� (0.2, 0.2, 0.2, 1.0)

	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, 0);

	camera.fi1 = -1.3;
	camera.fi2 = 0.8;
}







double A[] = { -8, 8, 0 };
double B[] = { -2, 4, 0 };
double C[] = { -1, 7, 0 };
double D[] = { 6, 0, 0 };
double E[] = { 0, 0, 0 };
double F[] = { -2, -4, 0 };
double G[] = { -7, -2, 0 };
double H[] = { -4, 2, 0 };


double A1[] = { -8, 8, 2 };
double B1[] = { -2, 4,2 };
double C1[] = { -1, 7, 2 };
double D1[] = { 6, 0, 2 };
double E1[] = { 0, 0, 2 };
double F1[] = { -2, -4, 2 };
double G1[] = { -7, -2, 2 };
double H1[] = { -4, 2, 2 };





void normal(double B[], double A1[], double A[]) {

	double a[] = { A[0] - A1[0], A[1] - A1[1], A[2] - A1[2] };
	double b[] = { B[0] - A1[0], B[1] - A1[1], B[2] - A1[2] };

	double n[] = { (a[1] * b[2] - b[1] * b[2]), (-a[0] * b[2] + b[0] * a[2]), (a[0] * b[1] - b[0] * a[1]) };
	double length_n = sqrt(n[0] * n[0] + n[1] * n[1] + n[2] * n[2]);

	for (int i = 0; i < 2; i++)
		n[i] /= length_n;


	glNormal3d(n[0], n[1], n[2]);
}



void buildWall(double x, double y, double xx, double yy, double h, double step_h) {

	double next_x = x * 0.9998 - y * 0.0174;
	double next_y = x * 0.0174 + y * 0.9998;
	double next_xx = xx * 0.9998 - yy * 0.0174;
	double next_yy = xx * 0.0174 + yy * 0.9998;

	double array_normal[4][4];

	array_normal[0][0] = x;
	array_normal[0][1] = y;
	array_normal[0][3] = h;

	array_normal[1][0] = xx;
	array_normal[1][1] = yy;
	array_normal[1][3] = h;

	array_normal[2][0] = next_x;
	array_normal[2][1] = next_y;
	array_normal[2][3] = h + step_h;

	normal(array_normal[2], array_normal[1], array_normal[0]);
	glBegin(GL_QUADS);
	glVertex3d(x, y, h);
	glVertex3d(next_x, next_y, h + step_h);
	glVertex3d(next_xx, next_yy, h + step_h);
	glVertex3d(xx, yy, h);
	glEnd();

	normal(array_normal[0], array_normal[1], array_normal[2]);
	glBegin(GL_QUADS);
	glVertex3d(x, y, h);
	glVertex3d(next_x, next_y, h + step_h);
	glVertex3d(next_xx, next_yy, h + step_h);
	glVertex3d(xx, yy, h);
	glEnd();
}

void convexity() {

	double x = -1;
	double y = 8;
	double result;
	double array[2][2];

	double array_normal1[3];
	double array_normal2[3];
	double array_normal3[3];

	while (x <= 6) {

		y = 8;

		while (y >= 0) {

			result = (x + 1) * (x + 1) + (y * y);

			if (result * 100 - 4900 > -1 && result * 100 - 4900 < 1) {

				array[0][0] = x;
				array[0][1] = y;



				array_normal1[0] = array[1][0];
				array_normal1[1] = array[1][1];
				array_normal1[2] = 2;

				array_normal2[0] = x;
				array_normal2[1] = y;
				array_normal2[2] = 2;

				array_normal3[0] = x;
				array_normal3[1] = y;
				array_normal3[2] = 0;


				normal(array_normal3, array_normal1, array_normal2);
				glBegin(GL_QUADS);
				glVertex3d(array[0][0], array[0][1], 0);
				glVertex3d(array[1][0], array[1][1], 0);
				glVertex3d(array[1][0], array[1][1], 2);
				glVertex3d(array[0][0], array[0][1], 2);
				glEnd();





				array_normal1[0] = x;
				array_normal1[1] = y;
				array_normal1[2] = 0;

				normal(C, array_normal1, D);
				glBegin(GL_QUADS);
				glVertex3d(-1, 7, 0);
				glVertex3d(6, 0, 0);
				glVertex3d(x, y, 0);
				glVertex3d(x, y, 0);
				glEnd();

				array_normal1[2] = 2;

				normal(D1, array_normal1, C1);
				glBegin(GL_QUADS);
				glVertex3d(-1, 7, 2);
				glVertex3d(6, 0, 2);
				glVertex3d(x, y, 2);
				glVertex3d(x, y, 2);
				glEnd();

				array[1][0] = array[0][0];
				array[1][1] = array[0][1];
			}

			y -= 0.01;
		}

		x += 0.01;
	}
}

void concavity() {

	double x = -7;
	double y;
	double result;
	double array[2][2];
	double array_normal1[3];
	double array_normal2[3];
	double array_normal3[3];


	while (x <= -2) {

		y = -4;

		while (y <= 0) {

			result = (x + 5.8) * (x + 5.8) + (y + 6.3) * (y + 6.3);

			if (((result * 100 - 2000) > -1) && ((result * 100 - 2000) < 1)) {

				array[0][0] = x;
				array[0][1] = y;

				array_normal1[0] = array[1][0];
				array_normal1[1] = array[1][1];
				array_normal1[2] = 2;

				array_normal2[0] = x;
				array_normal2[1] = y;
				array_normal2[2] = 2;

				array_normal3[0] = x;
				array_normal3[1] = y;
				array_normal3[2] = 0;


				normal(array_normal3, array_normal2, array_normal1);
				glBegin(GL_QUADS);
				glVertex3d(array[0][0], array[0][1], 0);
				glVertex3d(array[1][0], array[1][1], 0);
				glVertex3d(array[1][0], array[1][1], 2);
				glVertex3d(array[0][0], array[0][1], 2);
				glEnd();



				array_normal1[0] = x;
				array_normal1[1] = y;
				array_normal1[2] = 0;

				normal(E, array_normal1, H);
				glBegin(GL_TRIANGLES);
				glVertex3d(-4, 2, 0);
				glVertex3d(0, 0, 0);
				glVertex3d(x, y, 0);
				glEnd();

				array_normal1[2] = 2;

				normal(H1, array_normal1, E1);
				glBegin(GL_TRIANGLES);
				glVertex3d(-4, 2, 2);
				glVertex3d(0, 0, 2);
				glVertex3d(x, y, 2);
				glEnd();

				array[1][0] = array[0][0];
				array[1][1] = array[0][1];
			}

			y += 0.01;
		}

		x += 0.01;
	}
}



void figure1() {

	glBegin(GL_QUADS);
	normal(H, A, B);
	glTexCoord2d(0, 0);
	glVertex3dv(H);
	glTexCoord2d(0, 1);
	glVertex3dv(G);
	glTexCoord2d(1, 1);
	glVertex3dv(F);
	glTexCoord2d(1, 0);
	glVertex3dv(E);
	glEnd();

	glBegin(GL_QUADS);
	glVertex3dv(A);
	glVertex3dv(B);
	glVertex3dv(E);
	glVertex3dv(H);
	glEnd();

	glBegin(GL_QUADS);
	glVertex3dv(B);
	glVertex3dv(C);
	glVertex3dv(D);
	glVertex3dv(E);
	glEnd();

	glBegin(GL_QUADS);
	normal(B1, A1, H1);
	glTexCoord2d(1, 0);
	glVertex3dv(H1);
	glTexCoord2d(1, 1);
	glVertex3dv(G1);
	glTexCoord2d(0, 1);
	glVertex3dv(F1);
	glTexCoord2d(0, 0);
	glVertex3dv(E1);
	glEnd();

	glBegin(GL_QUADS);
	glVertex3dv(A1);
	glVertex3dv(B1);
	glVertex3dv(E1);
	glVertex3dv(H1);
	glEnd();

	glBegin(GL_QUADS);
	glVertex3dv(B1);
	glVertex3dv(C1);
	glVertex3dv(D1);
	glVertex3dv(E1);
	glEnd();






	glBegin(GL_QUADS);
	normal(A, A1, B);
	glColor3d(0.25, 0.25, 0.25);
	glTexCoord2d(1, 0);
	glVertex3dv(A);
	glTexCoord2d(0, 0);
	glVertex3dv(B);
	glTexCoord2d(0, 1);
	glVertex3dv(B1);
	glTexCoord2d(1, 1);
	glVertex3dv(A1);
	glEnd();

	glBegin(GL_QUADS);
	normal(B, B1, C);
	glTexCoord2d(1, 0);
	glVertex3dv(B);
	glTexCoord2d(0, 0);
	glVertex3dv(C);
	glTexCoord2d(0, 1);
	glVertex3dv(C1);
	glTexCoord2d(1, 1);
	glVertex3dv(B1);
	glEnd();

	glBegin(GL_QUADS);
	normal(D, D1, E);
	glTexCoord2d(1, 0);
	glVertex3dv(D);
	glTexCoord2d(0, 0);
	glVertex3dv(E);
	glTexCoord2d(0, 1);
	glVertex3dv(E1);
	glTexCoord2d(1, 1);
	glVertex3dv(D1);
	glEnd();

	glBegin(GL_QUADS);
	normal(E, E1, F);
	glTexCoord2d(1, 0);
	glVertex3dv(E);
	glTexCoord2d(0, 0);
	glVertex3dv(F);
	glTexCoord2d(0, 1);
	glVertex3dv(F1);
	glTexCoord2d(1, 1);
	glVertex3dv(E1);
	glEnd();

	glBegin(GL_QUADS);
	normal(H1, G, G1);
	glTexCoord2d(1, 0);
	glVertex3dv(G);
	glTexCoord2d(0, 0);
	glVertex3dv(H);
	glTexCoord2d(0, 1);
	glVertex3dv(H1);
	glTexCoord2d(1, 1);
	glVertex3dv(G1);
	glEnd();

	glBegin(GL_QUADS);
	normal(H, H1, A);
	glTexCoord2d(1, 0);
	glVertex3dv(H);
	glTexCoord2d(0, 0);
	glVertex3dv(A);
	glTexCoord2d(0, 1);
	glVertex3dv(A1);
	glTexCoord2d(1, 1);
	glVertex3dv(H1);
	glEnd();

	glBegin(GL_QUADS);
	normal(F, F1, G1);
	glTexCoord2d(1, 0);
	glVertex3dv(F);
	glTexCoord2d(0, 0);
	glVertex3dv(G);
	glTexCoord2d(0, 1);
	glVertex3dv(G1);
	glTexCoord2d(1, 1);
	glVertex3dv(F1);
	glEnd();

	glBegin(GL_QUADS);
	normal(C, C1, D);
	glTexCoord2d(0, 0);
	glVertex3dv(D);
	glTexCoord2d(1, 0);
	glVertex3dv(C);
	glTexCoord2d(1, 1);
	glVertex3dv(C1);
	glTexCoord2d(0, 1);
	glVertex3dv(D1);
	glEnd();

}

void figure2() {

	glBegin(GL_QUADS);
	normal(H, A, B);
	glTexCoord2d(1, 1);
	glVertex3dv(A);
	glTexCoord2d(0, 1);
	glVertex3dv(B);
	glTexCoord2d(0, 0);
	glVertex3dv(E);
	glTexCoord2d(1, 0);
	glVertex3dv(H);
	glEnd();

	glBegin(GL_QUADS);
	glVertex3dv(B);
	glVertex3dv(C);
	glVertex3dv(D);
	glVertex3dv(E);
	glEnd();

	/*glEnable(GL_COLOR_MATERIAL);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glColor4f(0, 1, 0, 0.5);

	glDisable(GL_BLEND);*/


	glBegin(GL_QUADS);
	normal(B1, A1, H1);
	glVertex3dv(B1);
	glVertex3dv(C1);
	glVertex3dv(D1);
	glVertex3dv(E1);
	glEnd();



	glBegin(GL_QUADS);
	glVertex3dv(A1);
	glVertex3dv(B1);
	glVertex3dv(E1);
	glVertex3dv(H1);
	glEnd();




	glBegin(GL_QUADS);
	normal(A, A1, B);
	glColor3d(0.25, 0.25, 0.25);
	glTexCoord2d(1, 0);
	glVertex3dv(A);
	glTexCoord2d(0, 0);
	glVertex3dv(B);
	glTexCoord2d(0, 1);
	glVertex3dv(B1);
	glTexCoord2d(1, 1);
	glVertex3dv(A1);
	glEnd();

	glBegin(GL_QUADS);
	normal(B, B1, C);
	glTexCoord2d(1, 0);
	glVertex3dv(B);
	glTexCoord2d(0, 0);
	glVertex3dv(C);
	glTexCoord2d(0, 1);
	glVertex3dv(C1);
	glTexCoord2d(1, 1);
	glVertex3dv(B1);
	glEnd();

	glBegin(GL_QUADS);
	normal(D, D1, E);
	glTexCoord2d(1, 0);
	glVertex3dv(D);
	glTexCoord2d(0, 0);
	glVertex3dv(E);
	glTexCoord2d(0, 1);
	glVertex3dv(E1);
	glTexCoord2d(1, 1);
	glVertex3dv(D1);
	glEnd();

	glBegin(GL_QUADS);
	normal(E, E1, F);
	glTexCoord2d(1, 0);
	glVertex3dv(E);
	glTexCoord2d(0, 0);
	glVertex3dv(F);
	glTexCoord2d(0, 1);
	glVertex3dv(F1);
	glTexCoord2d(1, 1);
	glVertex3dv(E1);
	glEnd();

	glBegin(GL_QUADS);
	normal(H1, G, G1);
	glTexCoord2d(1, 0);
	glVertex3dv(G);
	glTexCoord2d(0, 0);
	glVertex3dv(H);
	glTexCoord2d(0, 1);
	glVertex3dv(H1);
	glTexCoord2d(1, 1);
	glVertex3dv(G1);
	glEnd();

	glBegin(GL_QUADS);
	normal(H, H1, A);
	glTexCoord2d(1, 0);
	glVertex3dv(H);
	glTexCoord2d(0, 0);
	glVertex3dv(A);
	glTexCoord2d(0, 1);
	glVertex3dv(A1);
	glTexCoord2d(1, 1);
	glVertex3dv(H1);
	glEnd();



	convexity();
	concavity();
}

void figure3(float corner, float height) {

	normal(H, A, B);
	glBegin(GL_QUADS);
	glVertex3dv(A);
	glVertex3dv(B);
	glVertex3dv(E);
	glVertex3dv(H);
	glEnd();

	glBegin(GL_QUADS);
	glVertex3dv(H);
	glVertex3dv(G);
	glVertex3dv(F);
	glVertex3dv(E);
	glEnd();

	glBegin(GL_QUADS);
	glVertex3dv(B);
	glVertex3dv(C);
	glVertex3dv(D);
	glVertex3dv(E);
	glEnd();


	double g = -height / corner;
	for (double i = 0; i < corner - 1; i += 1) {

		g += height / corner;

		buildWall(-8, 8, -2, 4, g, height / corner);
		buildWall(-2, 4, -1, 7, g, height / corner);
		buildWall(-1, 7, 6, 0, g, height / corner);
		buildWall(6, 0, 0, 0, g, height / corner);
		buildWall(0, 0, -2, -4, g, height / corner);
		buildWall(-2, -4, -7, -2, g, height / corner);
		buildWall(-7, -2, -4, 2, g, height / corner);
		buildWall(-4, 2, -8, 8, g, height / corner);
		if (i < corner - 2)
			glRotated(1, 0, 0, 1);
	}



	A1[2] = g;
	B1[2] = g;
	C1[2] = g;
	D1[2] = g;
	E1[2] = g;
	F1[2] = g;
	G1[2] = g;
	H1[2] = g;

	normal(B1, A1, H1);
	glBegin(GL_QUADS);
	glVertex3d(-8, 8, g);
	glVertex3d(-2, 4, g);
	glVertex3d(0, 0, g);
	glVertex3d(-4, 2, g);
	glEnd();

	glBegin(GL_QUADS);
	glVertex3d(-4, 2, g);
	glVertex3d(-7, -2, g);
	glVertex3d(-2, -4, g);
	glVertex3d(0, 0, g);
	glEnd();

	glBegin(GL_QUADS);
	glVertex3d(-2, 4, g);
	glVertex3d(-1, 7, g);
	glVertex3d(6, 0, g);
	glVertex3d(0, 0, g);
	glEnd();
}

void circle() {

	glBegin(GL_POLYGON);
	for (double i = 0; i <= 2; i += 0.01)
	{
		double x = 9 * cos(i * 3.141593);
		double y = 9 * sin(i * 3.141593);

		double tx = cos(i * 3.141593) * 0.5 + 0.5;
		double ty = sin(i * 3.141593) * 0.5 + 0.5;

		glColor3d(0.5f, 0.5f, 0.5f);
		glNormal3d(0, 0, 1);
		glTexCoord2d(tx, ty);
		glVertex3d(x, y, 10);

	}
	glEnd();



}




void Render(OpenGL* ogl)
{



	glDisable(GL_TEXTURE_2D);
	glDisable(GL_LIGHTING);

	glEnable(GL_DEPTH_TEST);
	if (textureMode)
		glEnable(GL_TEXTURE_2D);

	if (lightMode)
		glEnable(GL_LIGHTING);

	if (textureReplace)
		glBindTexture(GL_TEXTURE_2D, texId);
	else
		glBindTexture(GL_TEXTURE_2D, texId2);




	//��������������
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_BLEND);


	//��������� ���������
	GLfloat amb[] = { 0.2, 0.2, 0.1, 1. };
	GLfloat dif[] = { 0.5, 0.5, 0.5, 1. };
	GLfloat spec[] = { 0.9, 0.8, 0.3, 1. };
	GLfloat sh = 0.1f * 256;


	//�������
	glMaterialfv(GL_FRONT, GL_AMBIENT, amb);
	//��������
	glMaterialfv(GL_FRONT, GL_DIFFUSE, dif);
	//����������
	glMaterialfv(GL_FRONT, GL_SPECULAR, spec); \
		//������ �����
		glMaterialf(GL_FRONT, GL_SHININESS, sh);

	//���� ���� �������, ��� ����������� (����������� ���������)
	glShadeModel(GL_SMOOTH);
	//===================================
	//������� ���  


	//figure1();
	figure2();
	//figure3(20, 10);
	circle();












	//��������� ������ ������


	glMatrixMode(GL_PROJECTION);	//������ �������� ������� ��������. 
	//(���� ��������� ��������, ����� �� ������������.)
	glPushMatrix();   //��������� ������� ������� ������������� (������� ��������� ������������� ��������) � ���� 				    
	glLoadIdentity();	  //��������� ��������� �������
	glOrtho(0, ogl->getWidth(), 0, ogl->getHeight(), 0, 1);	 //������� ����� ������������� ��������

	glMatrixMode(GL_MODELVIEW);		//������������� �� �����-��� �������
	glPushMatrix();			  //��������� ������� ������� � ���� (��������� ������, ����������)
	glLoadIdentity();		  //���������� �� � ������

	glDisable(GL_LIGHTING);



	GuiTextRectangle rec;		   //������� ����� ��������� ��� ������� ������ � �������� ������.
	rec.setSize(300, 150);
	rec.setPosition(10, ogl->getHeight() - 150 - 10);


	std::stringstream ss;
	ss << "T - ���/���� �������" << std::endl;
	ss << "L - ���/���� ���������" << std::endl;
	ss << "F - ���� �� ������" << std::endl;
	ss << "G - ������� ���� �� �����������" << std::endl;
	ss << "G+��� ������� ���� �� ���������" << std::endl;
	ss << "�����. �����: (" << light.pos.X() << ", " << light.pos.Y() << ", " << light.pos.Z() << ")" << std::endl;
	ss << "�����. ������: (" << camera.pos.X() << ", " << camera.pos.Y() << ", " << camera.pos.Z() << ")" << std::endl;
	ss << "��������� ������: R=" << camera.camDist << ", fi1=" << camera.fi1 << ", fi2=" << camera.fi2 << std::endl;

	rec.setText(ss.str().c_str());
	rec.Draw();

	glMatrixMode(GL_PROJECTION);	  //��������������� ������� �������� � �����-��� �������� �� �����.
	glPopMatrix();


	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();



}