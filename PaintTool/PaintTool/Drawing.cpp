#include "Drawing.h"

//// 직교좌표계 방법
//void RectangularCoordinate(const float radius,const float e)
//{
//	glColor3f(1.0f, 0.0, 0.0f); // 빨간색
//
//	for (GLfloat x = -radius; x <= radius; x += 0.01)
//	{
//		for (GLfloat y = -radius; y <= radius; y += 0.01)
//		{
//			if (( (pow(radius-e, 2)) <= (pow(x, 2) + pow(y, 2))) && ( (pow(x, 2) + pow(y, 2)) <= (pow(radius+e, 2)) ))
//			{
//				DrawPoint(x, y);
//			}
//		}
//	}
//}

//
//// Bresenham 알고리즘
//void Bresenham(const int radius)
//{
//	glColor3f(0.0f, 0.0, 1.0f); // 파란색
//
//	GLint curX = 0, curY = radius;
//	GLint nextX = 0, nextY = 0;
//	GLint p = 3 - (radius << 2);
//	
//	int count = 0;
//
//	glBegin(GL_POINTS);
//		do
//		{
//			if (p < 0)
//			{
//				nextX = curX + 1; nextY = curY;
//				p = p + (curX << 2) + 6;
//			}
//			else
//			{
//				nextX = curX + 1; nextY = curY - 1;
//				p = p + ((curX - curY) << 2) + 10;
//			} 
//			SetPoint(curX, curY);
//	
//			curX = nextX; curY = nextY;
//		} while (curX <= curY);
//	glEnd();
//}

void Drawing::Rectangle::Draw()
{
	Object::Draw();

	glBegin(GL_POLYGON);
	{
		glVertex2f(transform.position.x, transform.position.y);
		glVertex2f(endPoint.x, transform.position.y);
		glVertex2f(endPoint.x, endPoint.y);
		glVertex2f(transform.position.x, endPoint.y);
	}
	glEnd();
}


// 극좌표계 방법
void Drawing::Circle::PolarCoordinate(const float _radius)
{
	GLfloat angle;
	GLfloat curX = 0, curY = 0;
	int count = 0;

	for (angle = 0; angle <= 360; angle += 1)
	{
		curX = cos(angle) * radius;
		curY = sin(angle) * radius;

		glBegin(GL_POINTS);
		glVertex2f(curX, curY);
		glEnd();
	}
	glFlush();
}


void Drawing::Circle::Draw()
{
	Object::Draw();

	PolarCoordinate(this->radius);
}