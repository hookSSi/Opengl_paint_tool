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

	class TRIANGLE : public Object
	{
	public:
		virtual void Draw();
	};

	class Rectangle : public Object
	{
	public:
		Rectangle() :Object() { Rectangle::Rectangle(Vector2(0, 0)); }
		Rectangle(Vector2 _startPoint) { this->transform.position = (Vector3)_startPoint; }
		Rectangle(Vector3 _startPoint) { this->transform.position = _startPoint; }

		virtual ~Rectangle() {}

		virtual void Draw(Vector2 endPoint);	
	};

	class Circle : public Object
	{
	private:
		void PolarCoordinate(const float _radius);
	public:
		float radius;
		int mode;

		Circle() :Object() { Circle::Circle(1); }
		Circle(float _radius) :radius(_radius), mode(0) { }

		virtual ~Circle() {}

		virtual void Draw(float _lastX, float _lastY, bool _fill);
	};

	class TEXT : public Object
	{
	public:
		virtual void Draw();
	};
}

#endif