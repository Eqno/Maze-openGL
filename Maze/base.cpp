#include <GL/freeglut_std.h>
#include <GL/glut.h>
#include <gl/gl.h>
#include <gl/glu.h>
#include <stdio.h>
#include <stdlib.h>
#include <cmath>
#include <iostream>
#include <unistd.h>
#include <ctime>
#include <vector>
#include <algorithm>

const double WHITE[3] = {1, 1, 1};
const double RED[3] = {1, 0, 0};
const double YELLOW[3] = {1, 1, 0};
const double GREEN[3] = {0, 1, 0};
const double BLACK[3] = {-1, -1, -1};

#define ACTCOLOR WHITE
#define GROUNDCOLOR RED

typedef unsigned long long ull;

const double pi = 4*atan(1);
struct Point { double x, y, z; };