#ifndef  __OBJECT_H_
#define __OBJECT_H_

#include "Vector.h"
#include <gl/glut.h>

class Transform
{
public:
	Vector3 position;
	Vector3 scale;
	Vector3 rotation;

	Transform() {scale = Vector3(1, 1, 1);}; // �⺻ ������
	Transform(const Transform& _transform); // ���� ������

	~Transform() {}; // �Ҹ���

	Transform& operator=(const Transform& _transform); // ���� ���� ������
	bool operator==(const Transform& _transform) const;
	bool operator!=(const Transform& _transform) const;

	// �⺻ ������, ���� ������, �Ҹ���, ���� ���� �����ڴ� �⺻������ ��������� �͵�
};

class Object
{
public:
	Transform transform;

	Object() {};
	Object(const Object& other);

	virtual ~Object() {};

	Object& operator=(const Object& other);

	virtual void Draw();
};

#endif // ! __DRAWING_OBJECT_H_
