/* This program illustrates the use of the glut library for
interfacing with a window system */

#define NULL 0
#define LINE 1
#define RECTANGLE 2
#define TRIANGLE  3
#define POINTS 4
#define TEXT 5

#include <GL/glut.h>
#include <stdlib.h>

void mouse(int, int, int, int);
void key(unsigned char, int, int);
void display(void);
void drawSquare(int, int);
void myReshape(GLsizei, GLsizei);

void myinit(void);

void screen_box(int, int, int);
void right_menu(int);
void middle_menu(int);
void color_menu(int);
void pixel_menu(int);
void fill_menu(int);
int pick(int, int);

/* globals */

GLsizei height = 500, width = 500; /* initial window size */
GLfloat size = 3.0;   /* half side length of square */
int draw_mode = 0; /* drawing mode */
int rx, ry; /*raster position*/

GLfloat r = 1.0, g = 1.0, b = 1.0; /* drawing color */
int fill = 0; /* fill flag */

void drawSquare(int x, int y)
{

	y = height - y;
	glColor3ub((char)rand() % 256, (char)rand() % 256, (char)rand() % 256);
	glBegin(GL_POLYGON);
	glVertex2f(x + size, y + size);
	glVertex2f(x - size, y + size);
	glVertex2f(x - size, y - size);
	glVertex2f(x + size, y - size);
	glEnd();
}


/* rehaping routine called whenever window is resized
or moved */

void myReshape(GLsizei w, GLsizei h)
{

	/* adjust clipping box */

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0.0, (GLdouble)w, 0.0, (GLdouble)h, -1.0, 1.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	/* adjust viewport and  clear */

	glViewport(0, 0, w, h);
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f); // 배경색 설정
	glClear(GL_COLOR_BUFFER_BIT);
	display();
	glFlush();

	/* set global size for use by drawing routine */

	width = w;
	height = h;
}

void myinit(void)
{
	glViewport(0, 0, width, height);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0.0, (GLdouble)width, 0.0, (GLdouble)height, -1.0, 1.0);

	glClearColor(1.0f, 1.0f, 1.0f, 1.0f); // 배경색 설정
	glClear(GL_COLOR_BUFFER_BIT);
	glFlush();
}


void mouse(int btn, int state, int x, int y)
{
	static int count;
	int where;
	static int xp[2], yp[2];
	if (btn == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
	{
		glPushAttrib(GL_ALL_ATTRIB_BITS);

		where = pick(x, y);
		glColor3f(r, g, b);
		if (where != 0)
		{
			count = 0;
			draw_mode = where;
		}
		else switch (draw_mode)
		{
		case(LINE) :
			if (count == 0)
			{
				count++;
				xp[0] = x;
				yp[0] = y;
			}
			else
			{
				glBegin(GL_LINES);
				glVertex2i(x, height - y);
				glVertex2i(xp[0], height - yp[0]);
				glEnd();
				draw_mode = 0;
				count = 0;
			}
				   break;
		case(RECTANGLE) :
			if (count == 0)
			{
				count++;
				xp[0] = x;
				yp[0] = y;
			}
			else
			{
				if (fill) glBegin(GL_POLYGON);
				else glBegin(GL_LINE_LOOP);
				glVertex2i(x, height - y);
				glVertex2i(x, height - yp[0]);
				glVertex2i(xp[0], height - yp[0]);
				glVertex2i(xp[0], height - y);
				glEnd();
				draw_mode = 0;
				count = 0;
			}
						break;
		case (TRIANGLE) :
			switch (count)
		{
			case(0) :
				count++;
				xp[0] = x;
				yp[0] = y;
				break;
			case(1) :
				count++;
				xp[1] = x;
				yp[1] = y;
				break;
			case(2) :
				if (fill) glBegin(GL_POLYGON);
				else glBegin(GL_LINE_LOOP);
				glVertex2i(xp[0], height - yp[0]);
				glVertex2i(xp[1], height - yp[1]);
				glVertex2i(x, height - y);
				glEnd();
				draw_mode = 0;
				count = 0;
		}
						break;
		case(POINTS) :
		{
			drawSquare(x, y);
			count++;
		}
					 break;
		case(TEXT) :
		{
			rx = x;
			ry = height - y;
			glRasterPos2i(rx, ry);
			count = 0;
		}
		}

		glPopAttrib();
		glFlush();
	}
}

int pick(int x, int y)
{
	y = height - y;
	if (y < height - width / 10) return 0;
	else if (x < width / 10) return LINE;
	else if (x < width / 5) return RECTANGLE;
	else if (x < 3 * width / 10) return TRIANGLE;
	else if (x < 2 * width / 5) return POINTS;
	else if (x < width / 2) return TEXT;
	else return 0;
}

void screen_box(int x, int y, int s)
{
	glBegin(GL_QUADS);
	glVertex2i(x, y);
	glVertex2i(x + s, y);
	glVertex2i(x + s, y + s);
	glVertex2i(x, y + s);
	glEnd();
}

void right_menu(int id)
{
	if (id == 1) exit(0);
	else display();
}

void middle_menu(int id)
{

}

void color_menu(int id)
{
	if (id == 1) { r = 1.0; g = 0.0; b = 0.0; }
	else if (id == 2) { r = 0.0; g = 1.0; b = 0.0; }
	else if (id == 3) { r = 0.0; g = 0.0; b = 1.0; }
	else if (id == 4) { r = 0.0; g = 1.0; b = 1.0; }
	else if (id == 5) { r = 1.0; g = 0.0; b = 1.0; }
	else if (id == 6) { r = 1.0; g = 1.0; b = 0.0; }
	else if (id == 7) { r = 1.0; g = 1.0; b = 1.0; }
	else if (id == 8) { r = 0.0; g = 0.0; b = 0.0; }
}


void pixel_menu(int id)
{
	if (id == 1) size = 2 * size;
	else if (size > 1) size = size / 2;
}

void fill_menu(int id)
{
	if (id == 1) fill = 1;
	else fill = 0;
}

void key(unsigned char k, int xx, int yy)
{
	if (draw_mode != TEXT) return;
	glColor3f(0.0, 0.0, 0.0);
	glRasterPos2i(rx, ry);
	glutBitmapCharacter(GLUT_BITMAP_9_BY_15, k);
	/*glutStrokeCharacter(GLUT_STROKE_ROMAN,i); */
	rx += glutBitmapWidth(GLUT_BITMAP_9_BY_15, k);

}

void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // 화면과 깊이버퍼를 비움
	glLoadIdentity();

	int shift = 0;
	glPushAttrib(GL_ALL_ATTRIB_BITS);
	glColor3f(1.0, 1.0, 1.0);
	screen_box(0, height - width / 10, width / 10);
	glColor3f(1.0, 0.0, 0.0);
	screen_box(width / 10, height - width / 10, width / 10);
	glColor3f(0.0, 1.0, 0.0);
	screen_box(width / 5, height - width / 10, width / 10);
	glColor3f(0.0, 0.0, 1.0);
	screen_box(3 * width / 10, height - width / 10, width / 10);
	glColor3f(1.0, 1.0, 0.0);
	screen_box(2 * width / 5, height - width / 10, width / 10);
	glColor3f(0.0, 0.0, 0.0);
	screen_box(width / 10 + width / 40, height - width / 10 + width / 40, width / 20);
	glBegin(GL_LINES);
	glVertex2i(height / 40, height - width / 20);
	glVertex2i(height / 40 + width / 20, height - width / 20);
	glEnd();
	glBegin(GL_TRIANGLES);
	glVertex2i(width / 5 + width / 40, height - width / 10 + width / 40);
	glVertex2i(width / 5 + width / 20, height - width / 40);
	glVertex2i(width / 5 + 3 * width / 40, height - width / 10 + width / 40);
	glEnd();
	glPointSize(3.0);
	glBegin(GL_POINTS);
	glVertex2i(3 * width / 10 + width / 20, height - width / 20);
	glEnd();
	glRasterPos2i(2 * width / 5, height - width / 20);
	glutBitmapCharacter(GLUT_BITMAP_9_BY_15, 'A');
	shift = glutBitmapWidth(GLUT_BITMAP_9_BY_15, 'A');
	glRasterPos2i(2 * width / 5 + shift, height - width / 20);
	glutBitmapCharacter(GLUT_BITMAP_9_BY_15, 'B');
	shift += glutBitmapWidth(GLUT_BITMAP_9_BY_15, 'B');
	glRasterPos2i(2 * width / 5 + shift, height - width / 20);
	glutBitmapCharacter(GLUT_BITMAP_9_BY_15, 'C');

	glFlush();
	glPopAttrib();
}


int main(int argc, char** argv)
{
	int c_menu, p_menu, f_menu;

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGBA); // 디스플레이 모드 선택
	glutInitWindowSize(500, 500);
	glutCreateWindow("square");

	c_menu = glutCreateMenu(color_menu);
	glutAddMenuEntry("Red", 1);
	glutAddMenuEntry("Green", 2);
	glutAddMenuEntry("Blue", 3);
	glutAddMenuEntry("Cyan", 4);
	glutAddMenuEntry("Magenta", 5);
	glutAddMenuEntry("Yellow", 6);
	glutAddMenuEntry("White", 7);
	glutAddMenuEntry("Black", 8);
	p_menu = glutCreateMenu(pixel_menu);
	glutAddMenuEntry("increase pixel size", 1);
	glutAddMenuEntry("decrease pixel size", 2);
	f_menu = glutCreateMenu(fill_menu);
	glutAddMenuEntry("fill on", 1);
	glutAddMenuEntry("fill off", 2);
	glutCreateMenu(right_menu);
	glutAddMenuEntry("quit", 1);
	glutAddMenuEntry("clear", 2);
	glutAttachMenu(GLUT_RIGHT_BUTTON);

	glutCreateMenu(middle_menu);
	glutAddSubMenu("Colors", c_menu);
	glutAddSubMenu("Pixel Size", p_menu);
	glutAddSubMenu("Fill", f_menu);
	glutAttachMenu(GLUT_MIDDLE_BUTTON);

	glutDisplayFunc(display); // 드로잉

	myinit(); // 초기화
	glutReshapeFunc(myReshape); // 윈도우 창이 변경 되었을 때
	glutKeyboardFunc(key); // 키보드 입력
	glutMouseFunc(mouse); // 마우스 입력

	glutMainLoop(); // 메인 루프
}
