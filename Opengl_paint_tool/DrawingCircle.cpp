#include "DrawingCircle.h"

void DrawPoint(GLfloat x1, GLfloat y1)
{
	glBegin(GL_POINTS);
		glVertex2f(x1, y1);
	glEnd();
}

void SetPoint(GLfloat x1, GLfloat y1)
{
	//glBegin(GL_POINTS);
		glVertex2f(x1, y1);
		glVertex2f(x1, -y1);
		glVertex2f(y1, x1);
		glVertex2f(y1, -x1);
		glVertex2f(-x1, -y1);
		glVertex2f(-y1, -x1);
		glVertex2f(-x1, y1);
		glVertex2f(-y1, x1);
	//glEnd();
}

// 직교좌표계 방법
void RectangularCoordinate(float radius, float positionX, float positionY, float e)
{
	glLoadIdentity();
	glTranslatef(positionX, positionY, -100);

	glColor3f(1.0f, 0.0, 0.0f); // 빨간색

	for (GLfloat x = -radius; x <= radius; x += 0.01)
	{
		for (GLfloat y = -radius; y <= radius; y += 0.01)
		{
			if (( (pow(radius-e, 2)) <= (pow(x, 2) + pow(y, 2))) && ( (pow(x, 2) + pow(y, 2)) <= (pow(radius+e, 2)) ))
			{
				DrawPoint(x, y);
			}
		}
	}
	glFlush();
}


// 극좌표계 방법
void PolarCoordinate(float radius,float positionX, float positionY)
{
	glLoadIdentity();
	glTranslatef(positionX, positionY, -100);

	glColor3f(0.0f, 1.0f, 0.0f); // 초록색

	GLfloat angle;
	GLfloat curX = 0, curY = 0;
	int count = 0;

	for (angle = 0; angle <= 360; angle += 1)
	{
		curX = cos(angle) * radius;
		curY = sin(angle) * radius;

		DrawPoint(curX, curY);
	}
	glFlush();
}

// Bresenham 알고리즘
void Bresenham(const int radius, const int positionX, const int positionY)
{
	glLoadIdentity();
	glTranslatef(positionX, positionY, -100);

	glColor3f(0.0f, 0.0, 1.0f); // 파란색

	GLint curX = 0, curY = radius;
	GLint nextX = 0, nextY = 0;
	GLint p = 3 - (radius << 2);
	
	int count = 0;

	glBegin(GL_POINTS);
		do
		{
			if (p < 0)
			{
				nextX = curX + 1; nextY = curY;
				p = p + (curX << 2) + 6;
			}
			else
			{
				nextX = curX + 1; nextY = curY - 1;
				p = p + ((curX - curY) << 2) + 10;
			} 
			SetPoint(curX, curY);
	
			curX = nextX; curY = nextY;
		} while (curX <= curY);
	glEnd();
	glFlush();
}

float Circle::Draw(float positionX, float positionY, int mode)
{
	__int64 freq, start, end;
	float fgap;
	QueryPerformanceFrequency((LARGE_INTEGER*)(&freq));

	QueryPerformanceCounter((LARGE_INTEGER*)&start);

	switch (mode)
	{
	case 0:
		RectangularCoordinate(this->GetRadius(), positionX, positionY, 0.001);
		break;
	case 1:
		PolarCoordinate(this->GetRadius(), positionX, positionY);
		break;
	case 2:
		Bresenham(this->GetRadius(), positionX, positionY);
		break;
	}

	QueryPerformanceCounter((LARGE_INTEGER*)&end);

	fgap = (float)(end - start) / freq;

	return fgap;
}