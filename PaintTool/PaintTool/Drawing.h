#ifndef __DRAWING_CIRCLE_H_
#define __DRAWING_CIRCLE_H_

// OpenGL 헤더파일
#include <GL/glut.h> 
// OpenGL 유틸리티
#include<gl/glu.h> 
#include<gl/glut.h>
// OpenGL 보조 함수들
#include<gl/glaux.h>
// 수학 관련 함수들
#include<math.h>
// 리스트
#include<list>


#include "Object.h"

namespace Drawing
{
	class Point : public Object
	{
	public:
		virtual void Draw();
	};

	class Line : public Object
	{
	public:
		virtual void Draw(float _lastX, float _lastY);
	};

	class Triangle : public Object
	{
	private:
		void NormalTriangle(float _lastX, float _lastY, bool _fill);
		void AngledTriangle(float _lastX, float _lastY, bool _fill);
	public:
		int count = 0;
		Vector2 secondPos;

		Triangle() :Object(), count(0) { secondPos = Vector2(0, 0); }

		virtual ~Triangle() {}

		virtual void Draw(float _lastX, float _lastY, bool _fill);
		void PlusCount() { count = (count + 1) % 3; }
	};

	class Rectangle : public Object
	{
	public:
		Rectangle() :Object(){}

		virtual ~Rectangle() {}

		virtual void Draw(float _lastX, float _lastY, bool _fill);
	};

	class Circle : public Object
	{
	private:
		void PolarCoordinate(Vector2 _centerPos, const float _radius, bool _fill);
		void RectangularCoordinate(Vector2 _centerPos, const float radius, bool _fill, const float e);
		void Bresenham(Vector2 _centerPos,const int radius, bool _fill);
		void CalculatingDiscrit(int &_p, int &_curX, int &_curY, int &_nextX, int &_nextY);
	public:
		float radius;
		int mode;

		Circle() :Object() { Circle::Circle(1); }
		Circle(float _radius) :radius(_radius), mode(0) { }

		virtual ~Circle() {}

		virtual void Draw(float _lastX, float _lastY, bool _fill);
	};

	class Text : public Object
	{
	public:
		std::list<unsigned char> textList;
		int length = textList.size();
		int base = 0;

		GLvoid glPrint(const char *text);
		virtual void Draw();
		bool AddChar(unsigned char& ch);
		bool DeleteChar();
		virtual ~Text() { textList.~list(); }
	};
}

#endif