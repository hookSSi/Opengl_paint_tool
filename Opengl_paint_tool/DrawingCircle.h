// OpenGL 헤더파일
#include <GL/glut.h> 
// OpenGL 유틸리티
#include<gl/glu.h> 
#include<gl/glut.h>
// OpenGL 보조 함수들
#include<gl/glaux.h>
// 수학 관련 함수들
#include<math.h>

class Circle
{
private:
	int radius;
public:
	Circle(){ Circle::Circle(1); }
	Circle(int _radius){ SetRadius(_radius); }
	DWORD Draw(float x, float y, int mode);
	void SetRadius(int _radius){ radius = _radius; }
	int GetRadius(){ return radius; }
};