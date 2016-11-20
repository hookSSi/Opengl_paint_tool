#ifndef __DEBUG_H_
#define __DEBUG_H_

#include<Windows.h>
#include<gl/glut.h>

namespace Debug
{
	void Log(wchar_t* buffer, float value)
	{
		swprintf_s(buffer, 256, L"%f", value);

		MessageBox(NULL, buffer, TEXT("Debug"), MB_OK);
	}
	void Log(wchar_t* buffer)
	{
		MessageBox(NULL, buffer, TEXT("Debug"), MB_OK);
	}

	void Draw()
	{
		static GLubyte bitmap[] = {
			0x07, 0xe0, 0x18, 0x18, 0x20, 0x04, 0x43, 0xc2,
			0x44, 0x22, 0x88, 0x11, 0x81, 0x81, 0x81, 0x81,
			0x80, 0x01, 0x80, 0x01, 0x92, 0x49, 0x4c, 0x32,
			0x40, 0x02, 0x20, 0x04, 0x18, 0x18, 0x07, 0xe0,
		};
		glClear(GL_COLOR_BUFFER_BIT);

		glPixelStorei(GL_UNPACK_ALIGNMENT, 2);
		glColor3f(1, 1, 0);
		glRasterPos2f(0.5, 0.5);
		glBitmap(16, 16, 0, 0, 20, 0, bitmap);
		glBitmap(16, 16, 0, 0, 20, 0, bitmap);
		glBitmap(16, 16, 0, 10, 20, 0, bitmap);

		glFlush();
	}

	void Circle()
	{
		glLoadIdentity();
		glTranslatef(0, 0, -10);
		glScalef(2, 2, 2);
		int radius = 2;

		glColor3f(0.0f, 1.0f, 0.0f); // ÃÊ·Ï»ö

		GLfloat angle;
		GLfloat curX = 0, curY = 0;
		int count = 0;

		for (angle = 0; angle <= 360; angle += 1)
		{
			for (int i = 0; i <= radius; i += 1)
			{
				curX = cos(angle) * i;
				curY = sin(angle) * i;

				glBegin(GL_POINTS);
				glVertex2f(curX, curY);
				glEnd();
			}
		}
		glFlush();
	}
}

#endif