// OpenGL �������
#include <GL/glut.h> 
// OpenGL ��ƿ��Ƽ
#include<gl/glu.h> 
#include<gl/glut.h>
// OpenGL ���� �Լ���
#include<gl/glaux.h>
// ���� ���� �Լ���
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