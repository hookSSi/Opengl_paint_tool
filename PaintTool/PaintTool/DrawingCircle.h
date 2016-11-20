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

class Circle : public Object
{
public:
	int radius;
	Circle():Object(){ Circle::Circle(1); }
	Circle(int _radius){ SetRadius(_radius); }
	virtual void Draw();
	void SetRadius(int _radius){ radius = _radius; }
	int GetRadius(){ return radius; }
};

#endif