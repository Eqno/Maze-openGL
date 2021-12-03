#include "./texture.cpp"
#include "./scene.cpp"
#include <cmath>
#include <gl/freeglut_std.h>
#include <gl/gl.h>
#include <gl/glu.h>
#include <stdio.h>
#include <stdlib.h>
#include <synchapi.h>
#include <vector>
#include <wingdi.h>
using namespace std;

// wasd 移动，qe 旋转，m 打开地图，r 编辑地图，t 增加墙，y 删除墙。

int groundTexId = 0, ball = 0, wallTexId = 0;
int W = 1600, H = 900, F = 100;
unsigned int SCENESPEED = 5, SCENEID = 0, BALLACC = 20;
double GX = 9000, GY = -200, GZ = 16000, BALLSIZE = 1;

double obvRadius = 8, obvFactor = 2,
    obvRotateX = 0, obvRotateY = 0, actStep = 7,
    turnLeftStep = 5, turnRightStep = 5,
    zoomStep = 0.05, obvRadiusMax = 10, obvRadiusMin = 1.8,
    mapViewFac = 0.7, mapViewHeight = 5000, wallHeight = 2000;
    
bool openMap = false, firstPerson = false,
    mouseLeftDown = false, mouseRightDown = false, mouseControlMove = false,
    turnLeft = false, turnRight = false, zoomIn = false, zoomOut = false,
    moveLeft = false, moveRight = false, moveForward = false, moveBack = false,
    editMap = false, drawWall = false;

double actRotateX = obvRotateX,
    lastRotateX = obvRotateX,
    lastRotateY = obvRotateY,
    actMoveStartX = 0, actMoveStartZ = 0,
    actMoveEndX = 0, actMoveEndZ = 0;

vector <Point> actor, ground, tmpWall;
vector <vector <Point>> wall, wallEx;

void updateObverse()
{   
    if (openMap)
    {
        gluLookAt((ground[0].x+GX)/F, (ground[0].y+mapViewHeight)/F, (ground[0].z+GZ)/F,
            (ground[0].x+GX)/F, ground[0].y/F, (ground[0].z+GZ)/F, 1, 0, 0);
    }
    else
    {
        if (firstPerson) glTranslated(0, 0, 0);
        else glTranslated(0, 0, -obvRadius);
        if (obvRotateX > 180) obvRotateX = -180;
        if (obvRotateX < -180) obvRotateX = 180;
        if (obvRotateY > 180) obvRotateY = -180;
        if (obvRotateY < -180) obvRotateY = 180;
        if (actRotateX > 180) actRotateX = -180;
        if (actRotateX < -180) actRotateX = 180;
        glRotatef(obvRotateY, 1, 0, 0);
        glRotatef(obvRotateX, 0, 1, 0);
        if (mouseRightDown)
        {
            rotateDY(actor, -obvRotateX);
            actRotateX = obvRotateX;
        }
    }
}
void getGround()
{
    ground.push_back((Point) {-GX, GY, -GZ});
    ground.push_back((Point) {-GX, GY, GZ});
    ground.push_back((Point) {GX, GY, GZ});
    ground.push_back((Point) {GX, GY, -GZ});
}
void addGround()
{
    if (editMap) glColor3dv(RED);   
    else glBindTexture(GL_TEXTURE_2D, groundTexId);
    glBegin(GL_QUADS);
    glTexCoord2d(0, 0); glVertex3d(ground[0].x/F, ground[0].y/F, ground[0].z/F);
    glTexCoord2d(0, 20); glVertex3d(ground[1].x/F, ground[1].y/F, ground[1].z/F);
    glTexCoord2d(20, 20); glVertex3d(ground[2].x/F, ground[2].y/F, ground[2].z/F);
    glTexCoord2d(20, 0); glVertex3d(ground[3].x/F, ground[3].y/F, ground[3].z/F);
    glEnd();
}
void drawLine(double x0, double y0, double z0,
    double x1, double y1, double z1, const double color[3])
{
    glBindTexture(GL_TEXTURE_2D, 0);
    glColor3dv(color);
    glBegin(GL_LINE_LOOP);
    glVertex3d(x0/F, y0/F, z0/F);
    glVertex3d(x1/F, y1/F, z1/F);
    glEnd();
}
void addWall()
{
    if (openMap && tmpWall.size())
    {
        drawLine(tmpWall[1].x, tmpWall[1].y, tmpWall[1].z,
            tmpWall[2].x, tmpWall[2].y, tmpWall[2].z, WHITE);
    }
    for (auto i: wall)
    {
        if (openMap) drawLine(i[1].x, i[1].y, i[1].z, i[2].x, i[2].y, i[2].z, BLACK);
        glBindTexture(GL_TEXTURE_2D, wallTexId);
        glBegin(GL_QUADS);
        glTexCoord2d(0, 0); glVertex3d(i[0].x/F, i[0].y/F, i[0].z/F);
        glTexCoord2d(0, 1); glVertex3d(i[1].x/F, i[1].y/F, i[1].z/F);
        glTexCoord2d(1, 1); glVertex3d(i[2].x/F, i[2].y/F, i[2].z/F);
        glTexCoord2d(1, 0); glVertex3d(i[3].x/F, i[3].y/F, i[3].z/F);
        glEnd();
    }
}
void addActor()
{
	glBindTexture(GL_TEXTURE_2D, 0);
	glColor3dv(YELLOW);
	for (int i=3; i<actor.size(); i+=4)
	{
        // glBindTexture(GL_TEXTURE_2D, ball);
		glBegin(GL_LINE_LOOP);
		for (int j=0; j<3; j++)
			glVertex3d(actor[i-j].x, actor[i-j].y, actor[i-j].z);
		glEnd();
	}
}
void display(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    glLineWidth(3);
    updateObverse();
	addGround();
    addWall();
	addActor();

    drawLine(-300, 0, 0, 300, 0, 0, WHITE);
    drawLine(0, -300, 0, 0, 300, 0, WHITE);
    drawLine(0, 0, -300, 0, 0, 300, WHITE);
    glutSwapBuffers();
}
void reshape (int w, int h)
{
    glViewport(0, 0, w, h); 
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    if (openMap) glOrtho(-GZ*mapViewFac*w/h/F, GZ*mapViewFac*w/h/F,
        -GZ*mapViewFac/F, GZ*mapViewFac/F, 0.1, 200);
    else gluPerspective(60, (double)w/h, 0.1, 200);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    W = w, H = h;
}
void move(double delX, double delZ)
{
    for (unsigned long long i=0; i<ground.size(); i++)
    {
        ground[i].x += delX;
        ground[i].z += delZ;
    }
    actMoveEndX = ground[0].x + GX - actMoveStartX;
    actMoveEndZ = ground[0].z + GZ - actMoveStartZ;
    if (drawWall && tmpWall.size())
    {
        tmpWall[0] = (Point) {actMoveEndX, GY, actMoveEndZ};
        tmpWall[1] = (Point) {actMoveEndX, GY+wallHeight, actMoveEndZ};
    }
    for (unsigned long long i=0; i<wall.size(); i++)
        for (unsigned long long j=0; j<wall[i].size(); j++)
        {
            wall[i][j].x += delX;
            wall[i][j].z += delZ;
        }
    for (unsigned long long i=0; i<wallEx.size(); i++)
        for (unsigned long long j=0; j<wallEx[i].size(); j++)
        {
            wallEx[i][j].x += delX;
            wallEx[i][j].z += delZ;
        }
}
void mouseClick(int button, int state, int x, int y)
{
    if (! openMap)
    {
        if (state == GLUT_DOWN)
        {
            lastRotateX = x, lastRotateY = y;
            if (button == GLUT_LEFT_BUTTON) mouseLeftDown = true;
            if (button == GLUT_RIGHT_BUTTON) mouseRightDown = true;
        }
        if (state == GLUT_UP)
        {
            if (button == GLUT_LEFT_BUTTON) mouseLeftDown = false;
            if (button == GLUT_RIGHT_BUTTON) mouseRightDown = false;
        }
        if (mouseLeftDown && mouseRightDown) mouseControlMove = true;
        else mouseControlMove = false;
    }    
}
void obverseChange(double &obvRotate, double coord, double &lastRotate)
{
    obvRotate += obvFactor * (coord-lastRotate) / obvRadius;
    lastRotate = coord;
}
void mouseClickMove(int x, int y)
{
    if (! openMap)
    {
        obverseChange(obvRotateX, x, lastRotateX);
        obverseChange(obvRotateY, y, lastRotateY);
    }
}
void keyboardListener(unsigned char cmd, int x, int y)
{
    switch (cmd)
    {
        case 'w': moveForward = true; break;
        case 's': moveBack = true; break;
        case 'a': moveLeft = true; break;
        case 'd': moveRight = true; break;
        case 'q': turnLeft = true; break;
        case 'e': turnRight = true; break;
        default: break;
    }
    if (openMap)
    {
        if (editMap)
        {
            if (drawWall)
            {
                if (cmd == 't')
                {
                    drawWall = false; 

                    const Point &p = tmpWall[1];
                    const Point &q = tmpWall[2];
                    vector <Point> v;
                    getLine(p, q, v);
                    wallEx.push_back(v);
                    wall.push_back(tmpWall);
                    tmpWall.clear();
                }
                else
                {

                }
            }
            else
            {
                switch (cmd)
                {
                    case 't':
                        drawWall = true;
                        tmpWall = vector <Point> ({
                            (Point) {0, GY, 0},
                            (Point) {0, GY+wallHeight, 0},
                            (Point) {0, GY+wallHeight, 0},
                            (Point) {0, GY, 0},
                        });
                        actMoveStartX = ground[0].x+GX;
                        actMoveStartZ = ground[0].z+GZ;
                    break;
                    case 'r': editMap = false; break;
                    default: break;
                }
            }
        }
        else
        {
            switch (cmd)
            {
                case 'r': editMap = true; break;
                case 'm':
                    openMap = false;
                    zoomIn = zoomOut = false;
                    reshape(W, H);
                    glutReshapeWindow(W, H);
                break;
                default: break;
            }
        }
    }
    else
    {
        switch (cmd)
        {
            case 'v': firstPerson = !firstPerson; zoomIn = zoomOut = false; break;
            case 'm':
                openMap = true;
                zoomIn = zoomOut = false;
                reshape(W, H);
                glutReshapeWindow(W, H);
            break;
            default: break;
        }
    }
}
void keyboardUpListener(unsigned char cmd, int x, int y)
{
    switch (cmd)
    {
        case 'w': moveForward = false; break;
        case 's': moveBack = false; break;
        case 'a': moveLeft = false; break;
        case 'd': moveRight = false; break;
        case 'q': turnLeft = false; break;
        case 'e': turnRight = false; break;
        default: break;
    }
}
void specialListener(int cmd, int x, int y)
{
    if (firstPerson || openMap)
    {
        switch (cmd)
        {
            case GLUT_KEY_LEFT: turnLeft = true; break;
            case GLUT_KEY_RIGHT: turnRight = true; break;
            default: break;
        }
    }
    else
    {
        switch (cmd)
        {
            case GLUT_KEY_UP: zoomIn = true; break;
            case GLUT_KEY_DOWN: zoomOut = true; break;
            case GLUT_KEY_LEFT: turnLeft = true; break;
            case GLUT_KEY_RIGHT: turnRight = true; break;
            default: break;
        }
    }
}
void specialUpListener(int cmd, int x, int y)
{
    if (firstPerson || openMap)
    {
        switch (cmd)
        {
            case GLUT_KEY_LEFT: turnLeft = false; break;
            case GLUT_KEY_RIGHT: turnRight = false; break;
            default: break;
        }
    }
    else
    {
        switch (cmd)
        {
            case GLUT_KEY_UP: zoomIn = false; break;
            case GLUT_KEY_DOWN: zoomOut = false; break;
            case GLUT_KEY_LEFT: turnLeft = false; break;
            case GLUT_KEY_RIGHT: turnRight = false; break;
            default: break;
        }
    }
}
void moveAlongWall(double delX, double delZ, double u, double v)
{
    double r = sqrt(u*u+v*v);
    double s = u/r;
    double c = v/r;
    double l = 0;
    const double EPS = 1e-5;
    if (abs(delX)>=EPS && abs(u)>=EPS)
        l = cos(atan(delZ/delX)-atan(v/u))*sqrt(delX*delX+delZ*delZ);
    else if (abs(delX) < EPS)
        l = cos(pi/2-atan(v/u))*delZ;
    else if (abs(u) < EPS)
        l = cos(pi/2-atan(-delZ/delX))*sqrt(delX*delX+delZ*delZ);
    l = abs(l);
    if (delX*u+delZ*v > 0) move(s*l, c*l);
    else move(-s*l, -c*l);
}
bool checkMovable(double delX, double delZ)
{
    static const int EPS = 3;
    static const double EPS2 = 5;
    for (int i=0; i<wallEx.size(); i++)
        for (int j=0; j<wallEx[i].size(); j++)
            if (sqrt((wallEx[i][j].x+delX)*(wallEx[i][j].x+delX)
                +(wallEx[i][j].z+delX)*(wallEx[i][j].z+delZ)) <= F+EPS2)
            {
                double a1 = wallEx[i].front().z, b1 = wallEx[i].front().x,
                    a2 = wallEx[i].back().z, b2 = wallEx[i].back().x,
                    c = wall[i][2].x-wall[i][1].x, d = wall[i][2].z-wall[i][1].z;
                if (j>EPS && j<wallEx[i].size()-1-EPS) moveAlongWall(delX, delZ, c, d);
                else if (j <= EPS)
                {
                    if (a1*c+b1*d < 0) moveAlongWall(delX, delZ, -a1, b1);
                    else moveAlongWall(delX, delZ, c, d);
                }
                else if (j >= wallEx[i].size()-1-EPS)
                {
                    if (a2*c+b2*d >= 0) moveAlongWall(delX, delZ, a2, -b2);
                    else moveAlongWall(delX, delZ, c, d);
                }
                return false;
            }
    return true;
}
void sceneMoveLoop(int id)
{
    double delX = 0, delZ = 0;
    if (moveLeft) 
    {
        delX = actStep * cos(actRotateX/180*pi);
        delZ = actStep * sin(actRotateX/180*pi);
    }
    if (moveRight) 
    {
        delX = -actStep * cos(actRotateX/180*pi);
        delZ = -actStep * sin(actRotateX/180*pi);
    }
    if (moveForward || mouseControlMove) 
    {
        delX = -actStep * sin(actRotateX/180*pi);
        delZ = actStep * cos(actRotateX/180*pi);
    }
    if (moveBack) 
    {
        delX = actStep * sin(actRotateX/180*pi);
        delZ = -actStep * cos(actRotateX/180*pi);
    }
    if (turnLeft)
    {
        actRotateX -= obvFactor * turnLeftStep / obvRadius;
        obvRotateX = actRotateX;
        rotateDY(actor, -obvRotateX);
    }
    if (turnRight)
    {
        actRotateX += obvFactor * turnLeftStep / obvRadius;
        obvRotateX = actRotateX;
        rotateDY(actor, -obvRotateX);
    }
    if (zoomIn && obvRadius>obvRadiusMin)
        obvRadius -= zoomStep;
    if (zoomOut && obvRadius<obvRadiusMax)
        obvRadius += zoomStep;
    if (editMap || checkMovable(delX, delZ))
        move(delX, delZ);
    glutPostRedisplay();
    glutTimerFunc(SCENESPEED, sceneMoveLoop, id);
}
int main(int argc, char** argv)
{
    // window
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(W, H); 
    glutInitWindowPosition(0, 0);
    glutCreateWindow("Maze");

	// init
    getGround();
	getBall(0, 0, 0, BALLSIZE, actor, BALLACC);
	groundTexId = loadTexture("1.bmp");
    // ball = loadTexture("2.bmp");
    wallTexId = loadTexture("3.bmp");
    
    // rendering
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_TEXTURE_2D);
    glutDisplayFunc(display); 
    glutReshapeFunc(reshape);

    // listener
    glutMouseFunc(mouseClick);
    glutMotionFunc(mouseClickMove);
    // glutPassiveMotionFunc(mouseInMove);
    glutKeyboardFunc(keyboardListener);
    glutKeyboardUpFunc(keyboardUpListener);
    glutSpecialFunc(specialListener);
    glutSpecialUpFunc(specialUpListener);

    // loop
    glutTimerFunc(SCENESPEED, sceneMoveLoop, SCENEID);
    glutMainLoop();
    return 0;
}