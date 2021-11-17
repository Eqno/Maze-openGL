#include <GL/freeglut_std.h>
#include <GL/glut.h>
#include <gl/gl.h>
#include <stdio.h>
#include <stdlib.h>
#include <cmath>
#include <iostream>
#include <unistd.h>
#include <ctime>
#include <vector>
#include <winuser.h>
#include "./texture.cpp"
#include "./ball.cpp"
using namespace std;

int W = 1600, H = 900, D = 1000;

double eyeX = 0, eyeY = 0, eyeZ = 500;
double actX = 0, actY = 0, actZ = 0;
double facX = -100, facY = 100, facZ = 100;
double lastX = eyeX, lastY = eyeY;
double GX = 5000, GY = -1000, GZ = 5000;
double AX = 100, AY = 300, AZ = 100;
int ground = 0, ball = 0;
const int BALLACC = 20;

vector <Point> actor;

void addGround()
{
    glBindTexture(GL_TEXTURE_2D, ground);
	glBegin(GL_QUADS);
	glTexCoord2d(0, 0); glVertex3d(-GX/W, GY/H, -GZ/D);
	glTexCoord2d(0, 10); glVertex3d(-GX/W, GY/H, GZ/D);
	glTexCoord2d(10, 10); glVertex3d(GX/W, GY/H, GZ/D);
	glTexCoord2d(10, 0); glVertex3d(GX/W, GY/H, -GZ/D);
	glEnd();
}
void addActor()
{
	glBindTexture(GL_TEXTURE_2D, 0);
	glColor3d(YELLOW);
	for (int i=4; i<actor.size(); i+=5)
	{
        glBindTexture(GL_TEXTURE_2D, ball);
		glBegin(GL_POLYGON);
        glTexCoord2d(0, 0); glVertex3d(actor[i-0].x, actor[i-0].y, actor[i-0].z);
        glTexCoord2d(0, 10); glVertex3d(actor[i-1].x, actor[i-1].y, actor[i-1].z);
        glTexCoord2d(10, 10); glVertex3d(actor[i-2].x, actor[i-2].y, actor[i-2].z);
        glTexCoord2d(10, 0); glVertex3d(actor[i-3].x, actor[i-3].y, actor[i-3].z);
		// for (int j=0; j<3; j++)
		// 	glVertex3d(actor[i-j].x, actor[i-j].y, actor[i-j].z);
		glEnd();
	}
	printf("%d\n", actor.size());
}
void display(void)
{
    glClear(GL_COLOR_BUFFER_BIT);
    glLoadIdentity();
    gluLookAt(eyeX/facX, eyeY/facY, eyeZ/facZ, actX/facX, actY/facY, actZ/facZ, 0, 1, 0);
	addGround();
	addActor();
    glutSwapBuffers();
}
void reshape (int w, int h)
{
    glViewport(0, 0, w, h); 
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60, (double)w/h, 1, 20);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}
void mouseClick(int button, int state, int x, int y)
{
    if (state == GLUT_DOWN) lastX = x, lastY = y;
}
void obverseChange(double m, double &last, double &eye, double act, double fac)
{
    double radius = sqrt((eye-act)*(eye-act)+(eyeZ-actZ)*(eyeZ-actZ));
    double del = m - last, ang = del / radius;
	if (eyeZ - actZ >= 0)
	{
		double newAng = atan((eye-act)/(eyeZ-actZ)) + ang;
		eye = act + radius*sin(newAng);
		eyeZ = actZ + radius*cos(newAng);
	}
	else
	{
		double newAng = atan((eye-act)/(actZ-eyeZ)) + fac*ang;
		eye = act + radius*sin(newAng);
		eyeZ = actZ - radius*cos(newAng);
	}
    last = m;
}
void mouseClickMove(int x, int y)
{
    obverseChange(x, lastX, eyeX, actX, -1);
    obverseChange(y, lastY, eyeY, actY, 1);
    glutPostRedisplay();
}
void mouseInMove(int x, int y)
{
    (x/W);
}
int main(int argc, char** argv)
{
    // window
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(W, H); 
    glutInitWindowPosition(0, 0);
    glutCreateWindow("Maze");

	// init
	getBall(0, 0, 0, actor, BALLACC);
	ground = loadTexture("1.bmp");
    ball = loadTexture("2.bmp");
    
    // rendering
    glEnable(GL_TEXTURE_2D);
    glutDisplayFunc(display); 
    glutReshapeFunc(reshape);

    // listener
    glutMouseFunc(mouseClick);
    glutMotionFunc(mouseClickMove);
    // glutPassiveMotionFunc(mouseInMove);

    // loop
    glutMainLoop();
    return 0;
}