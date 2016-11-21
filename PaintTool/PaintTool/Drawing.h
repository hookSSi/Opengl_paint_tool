#ifndef __DRAWING_CIRCLE_H_
#define __DRAWING_CIRCLE_H_

// OpenGL �������
#include <GL/glut.h> 
// OpenGL ��ƿ��Ƽ
#include<gl/glu.h> 
#include<gl/glut.h>
// OpenGL ���� �Լ���
#include<gl/glaux.h>
// ���� ���� �Լ���
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
		virtual void Draw();
	};

	class TRIANGLE : public Object
	{
	public:
		virtual void Draw();
	};

	class Rectangle : public Object
	{
	public:
		Vector2 endPoint;

		Rectangle() :Object() { Rectangle::Rectangle(Vector2(0, 0)); }
		Rectangle(Vector2 _startPoint) { this->transform.position = (Vector3)_startPoint; }
		Rectangle(Vector3 _startPoint) { this->transform.position = _startPoint; }

		virtual ~Rectangle() {}

		virtual void Draw();	
	};

	class Circle : public Object
	{
	private:
		void PolarCoordinate(const float _radius);
	public:
		float radius;

		Circle() :Object() { Circle::Circle(1); }
		Circle(float _radius) :radius(_radius) {}

		virtual ~Circle() {}

		virtual void Draw();
	};

	class TEXT : public Object
	{
	public:
		virtual void Draw();
	};
}

#endif