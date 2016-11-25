#include "Drawing.h"

const float PI = 3.1415926535897932384626433832795;

const float RadToDeg(const float radian)
{
	return radian * 180 / PI;
}

const float  DegToRad(const float degree)
{
	return degree * PI / 180;
}

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

/* Point Class */
void Drawing::Point::Draw(Vector2 _pos)
{
	Object::Draw();

	glBegin(GL_POLYGON);
	glVertex2f(_pos.x + 1, _pos.y + 1);
	glVertex2f(_pos.x - 1, _pos.y + 1);
	glVertex2f(_pos.x - 1, _pos.y - 1);
	glVertex2f(_pos.x + 1, _pos.y - 1);
	glEnd();
}


/* Rectangle Class */

void Drawing::Rectangle::Draw(Vector2 endPoint)
{
	Object::Draw();

	glBegin(GL_LINE_LOOP);
	{
		glVertex2f(transform.position.x, transform.position.y);
		glVertex2f(endPoint.x, transform.position.y);
		glVertex2f(endPoint.x, endPoint.y);
		glVertex2f(transform.position.x, endPoint.y);
	}
	glEnd();
}

/* Circle Class */

// 극좌표계 방법
void Drawing::Circle::PolarCoordinate(const float _radius)
{
	GLfloat angle;
	GLfloat curX = 0, curY = 0;
	int count = 0;

	glBegin(GL_LINE_LOOP);
	for (int i = 0; i <= 360; i += 1)
	{
		angle = DegToRad(i);
		curX = cos(angle) * radius;
		curY = sin(angle) * radius;
		
		glVertex2f(curX, curY);
	}
	glEnd();
	glFlush();
}


void Drawing::Circle::Draw()
{
	Object::Draw();

	PolarCoordinate(this->radius);
}