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


/* Point Class */
void Drawing::Point::Draw()
{
	Object::Draw();

	float sizeX = transform.scale.x;
	float sizeY = transform.scale.y;
	float x = transform.position.x;
	float y = transform.position.y;

	glBegin(GL_POLYGON);
	glVertex2f(x + sizeX, y + sizeY);
	glVertex2f(x - sizeX, y + sizeY);
	glVertex2f(x - sizeX, y - sizeY);
	glVertex2f(x + sizeX, y - sizeY);
	glEnd();

	glFlush();
	glPopAttrib();
}

/* Line Class */
void Drawing::Line::Draw(float _lastX, float _lastY)
{
	Object::Draw();

	glScalef(1, 1, 1);
	glLineWidth(transform.scale.x * 2);

	glBegin(GL_LINES);
	{
		glVertex2f(transform.position.x, transform.position.y);
		glVertex2f(_lastX, _lastY);
	}
	glEnd();


	glFlush();
	glPopAttrib();
}

/* Circle Class */

// 직교좌표계 방법
void RectangularCoordinate(Vector2 _centerPos, const float radius, bool _fill ,const float e)
{
	if (_fill)
		glBegin(GL_POLYGON);
	else
		glBegin(GL_LINE_LOOP);

	for (GLfloat x = _centerPos.x - radius; x <= _centerPos.x + radius ; x += 0.1)
	{
		for (GLfloat y = _centerPos.y; y < _centerPos.y + radius; y += 0.1)
		{
			if (((pow(radius - e, 2)) <= (pow(x - _centerPos.x, 2) + pow(y - _centerPos.y, 2))) && ((pow(x - _centerPos.x, 2) + pow(y - _centerPos.y, 2)) <= (pow(radius + e, 2))))
			{
				glVertex2f(x, y);
			}
		}
		for (GLfloat y = _centerPos.y + radius; y >= _centerPos.y; y -= 0.1)
		{
			if (((pow(radius - e, 2)) <= (pow(x - _centerPos.x, 2) + pow(y - _centerPos.y, 2))) && ((pow(x - _centerPos.x, 2) + pow(y - _centerPos.y, 2)) <= (pow(radius + e, 2))))
			{
				glVertex2f(x, y);
			}
		}
	}
	for (GLfloat x = _centerPos.x + radius; x >= _centerPos.x - radius; x -= 0.1)
	{
		for (GLfloat y = _centerPos.y; y > _centerPos.y - radius; y -= 0.1)
		{
			if (((pow(radius - e, 2)) <= (pow(x - _centerPos.x, 2) + pow(y - _centerPos.y, 2))) && ((pow(x - _centerPos.x, 2) + pow(y - _centerPos.y, 2)) <= (pow(radius + e, 2))))
			{
				glVertex2f(x, y);
			}
		}
		for (GLfloat y = _centerPos.y - radius; y <= _centerPos.y; y += 0.1)
		{
			if (((pow(radius - e, 2)) <= (pow(x - _centerPos.x, 2) + pow(y - _centerPos.y, 2))) && ((pow(x - _centerPos.x, 2) + pow(y - _centerPos.y, 2)) <= (pow(radius + e, 2))))
			{
				glVertex2f(x, y);
			}
		}
	}
	glEnd();

}


// Bresenham 알고리즘
void Bresenham(const int radius)
{
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
		//SetPoint(curX, curY);

		curX = nextX; curY = nextY;
	} while (curX <= curY);
	glEnd();
}

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
}


void Drawing::Circle::Draw(float _lastX, float _lastY, bool _fill)
{
	Object::Draw();

	this->radius = abs((long)(transform.position.x - _lastX));

	if (_fill)
	{
		glScalef(1, 1, 1);
		glLineWidth(transform.scale.x * 2);
	}

	Vector2 center((transform.position.x + _lastX) / 2, (transform.position.y + _lastY) / 2);

	switch (this->mode)
	{
	case 0:
		RectangularCoordinate(center, this->radius,_fill, 0.001f);
		break;
	case 1:
		PolarCoordinate(this->radius);
		break;
	case 2:
		Bresenham(this->radius);
		break;
	}

	glFlush();
	glPopAttrib();
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