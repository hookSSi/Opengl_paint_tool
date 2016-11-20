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