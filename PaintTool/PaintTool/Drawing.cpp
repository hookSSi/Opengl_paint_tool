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
	glFinish();
	glPopAttrib();
}

/* Line Class */
void Drawing::Line::Draw(float _lastX, float _lastY)
{
	Object::Draw();

	glLineWidth(transform.scale.x);

	glBegin(GL_LINES);
	{
		glVertex2f(transform.position.x, transform.position.y);
		glVertex2f(_lastX, _lastY);
	}
	glEnd();

	glFlush();
	glFinish();
	glPopAttrib();
}

/* Circle Class */

// 직교좌표계 방법
void Drawing::Circle::RectangularCoordinate(Vector2 _centerPos, const float radius, bool _fill ,const float e)
{
	if (_fill)
	{
		glBegin(GL_POLYGON);
	}
	else
	{
		glBegin(GL_LINE_LOOP);
		glScalef(1, 1, 1);
		glLineWidth(transform.scale.x);
	}

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
		for (GLfloat y = _centerPos.y - radius; y <= _centerPos.y; y += 0.5)
		{
			if (((pow(radius - e, 2)) <= (pow(x - _centerPos.x, 2) + pow(y - _centerPos.y, 2))) && ((pow(x - _centerPos.x, 2) + pow(y - _centerPos.y, 2)) <= (pow(radius + e, 2))))
			{
				glVertex2f(x, y);
			}
		}
	}
	glEnd();

}

// 극좌표계 방법
void Drawing::Circle::PolarCoordinate(Vector2 _centerPos, const float _radius, bool _fill)
{
	GLfloat angle;
	GLfloat curX = 0, curY = 0;
	int count = 0;

	if (_fill)
		glBegin(GL_POLYGON);
	else
	{
		glBegin(GL_LINE_LOOP);
		glScalef(1, 1, 1);
		glLineWidth(transform.scale.x);
	}
	for (int i = 0; i <= 360; i += 1)
	{
		angle = DegToRad(i);
		curX = _centerPos.x + cos(angle) * radius;
		curY = _centerPos.y + sin(angle) * radius;

		glVertex2f(curX, curY);
	}
	glEnd();
}


// Bresenham 알고리즘
void Drawing::Circle::Bresenham(Vector2 _centerPos, const int radius, bool _fill)
{
	if (radius <= 0)
		return;

	GLint curX = 0 , curY = radius;
	GLint nextX = 0, nextY = 0;
	GLint p = 3 - (radius << 2); // 판별식

	int count = 0;

	if (_fill)
		glBegin(GL_POLYGON);
	else
	{
		glBegin(GL_POINTS);
	}

	do
	{
		CalculatingDiscrit(p, curX, curY, nextX, nextY);
		glVertex2f(_centerPos.x - curX, _centerPos.y + curY);
	} while (curX <= curY);

	curX = 0, curY = radius, p = 3 - (radius << 2);
	
	curX = 0, curY = radius, p = 3 - (radius << 2);

	do
	{
		glVertex2f(_centerPos.x - curY, _centerPos.y + curX);
		CalculatingDiscrit(p, curX, curY, nextX, nextY);

	} while (curX <= curY);
	
	curX = 0, curY = radius, p = 3 - (radius << 2);

	do
	{
		CalculatingDiscrit(p, curX, curY, nextX, nextY);
		glVertex2f(_centerPos.x - curY, _centerPos.y - curX);
	} while (curX <= curY);

	curX = 0, curY = radius, p = 3 - (radius << 2);
	
	do
	{
		CalculatingDiscrit(p, curX, curY, nextX, nextY);
		glVertex2f(_centerPos.x - curX, _centerPos.y - curY);
	} while (curX <= curY);

	curX = 0, curY = radius, p = 3 - (radius << 2);

	do
	{
		CalculatingDiscrit(p, curX, curY, nextX, nextY);
		glVertex2f(_centerPos.x + curX, _centerPos.y - curY);
	} while (curX <= curY);

	curX = 0, curY = radius, p = 3 - (radius << 2);

	do
	{
		CalculatingDiscrit(p, curX, curY, nextX, nextY);
		glVertex2f(_centerPos.x + curY, _centerPos.y - curX);
	} while (curX <= curY);

	curX = 0, curY = radius, p = 3 - (radius << 2);

	do
	{
		CalculatingDiscrit(p, curX, curY, nextX, nextY);
		glVertex2f(_centerPos.x + curY, _centerPos.y + curX);
	} while (curX <= curY);

	curX = 0, curY = radius, p = 3 - (radius << 2);

	do
	{
	CalculatingDiscrit(p, curX, curY, nextX, nextY);
	glVertex2f(_centerPos.x + curX, _centerPos.y + curY);

	} while (curX <= curY);

	glEnd();
}

void Drawing::Circle::CalculatingDiscrit(int &_p, int &_curX, int &_curY, int &_nextX, int &_nextY) // 판별식으로 판별 중
{
	if (_p < 0)
	{
		_nextX = _curX + 1; _nextY = _curY;
		_p = _p + (_curX << 2) + 6;
	}
	else
	{
		_nextX = _curX + 1; _nextY = _curY - 1;
		_p = _p + ((_curX - _curY) << 2) + 10;
	}

	_curX = _nextX; _curY = _nextY;
}

void Drawing::Circle::Draw(float _lastX, float _lastY, bool _fill)
{
	Object::Draw();

	this->radius = abs((long)(transform.position.x - _lastX));

	Vector2 center((transform.position.x + _lastX) / 2, (transform.position.y + _lastY) / 2);

	switch (this->mode)
	{
	case 0:
		RectangularCoordinate(center, this->radius,_fill, 0.001f);
		break;
	case 1:
		PolarCoordinate(center, this->radius,_fill);
		break;
	case 2:
		Bresenham(center, this->radius,_fill);
		break;
	}

	glFlush();
	glFinish();
	glPopAttrib();
}

/* Rectangle Class */

void Drawing::Rectangle::Draw(float _lastX, float _lastY, bool _fill)
{
	Object::Draw();

	
	if (_fill)
		glBegin(GL_POLYGON);
	else
	{
		glScalef(1, 1, 1);
		glLineWidth(transform.scale.x);
		glBegin(GL_LINE_LOOP);
	}
	
	glVertex2f(transform.position.x, transform.position.y);
	glVertex2f(_lastX, transform.position.y);
	glVertex2f(_lastX, _lastY);
	glVertex2f(transform.position.x, _lastY);
	glEnd();

	glFlush();
	glFinish();
	glPopAttrib();
}

/* Triangle Class */

void Drawing::Triangle::Draw(float _lastX, float _lastY, bool _fill)
{
	Object::Draw();

	Vector2 center((transform.position.x + _lastX) / 2, (transform.position.y + _lastY) / 2);
	
	NormalTriangle(_lastX, _lastY, _fill);

	glFlush();
	glFinish();
	glPopAttrib();
}

void Drawing::Triangle::NormalTriangle(float _lastX, float _lastY, bool _fill)
{
	if (_fill)
		glBegin(GL_POLYGON);
	else
	{
		glScalef(1, 1, 1);
		glLineWidth(transform.scale.x);
		glBegin(GL_LINE_LOOP);
	}
	glVertex2f(transform.position.x, transform.position.y);
	glVertex2f(secondPos.x, secondPos.y);
	glVertex2f(_lastX, _lastY);

	glEnd();
}


/* Text Class */

bool Drawing::Text::AddChar(unsigned char& ch)
{
	length = textList.size();
	textList.push_back(ch);
}
bool Drawing::Text::DeleteChar()
{
	textList.pop_back();
}


GLvoid Drawing::Text::glPrint(const char *text)
{
	glPushAttrib(GL_LIST_BIT);
	glListBase(base - 32);
	glCallLists(strlen(text), GL_UNSIGNED_BYTE, text);
	glPopAttrib();
}

void Drawing::Text::Draw()
{
	Object::Draw();

	float tempX = transform.position.x;
	float tempY = transform.position.y;

	int i = 0;

	char buffer[255] = { 0 };


	for (std::list<unsigned char>::iterator iter = textList.begin(); iter != textList.end(); ++iter)
	{
		
		if (textList.size() == 0)
			break;
		else
		{
			buffer[i] = *iter;
		}
		i++;
	}

	glRasterPos2i(transform.position.x, transform.position.y);
	glPrint(buffer);

	glFlush();
	glFinish();
	glPopAttrib();
}

/* Heart Class */
void Drawing::Heart::Draw(float _lastX, float _lastY, bool _fill)
{
	Object::Draw();

	Vector2 center((transform.position.x + _lastX) / 2, (transform.position.y + _lastY) / 2);

	//NormalTriangle(_lastX, _lastY, _fill);

	glFlush();
	glFinish();
	glPopAttrib();
}