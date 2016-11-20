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

	Transform() {scale = Vector3(1, 1, 1);}; // 기본 생성자
	Transform(const Transform& _transform); // 복사 생성자

	~Transform() {}; // 소멸자

	Transform& operator=(const Transform& _transform); // 복사 대입 생성자
	bool operator==(const Transform& _transform) const;
	bool operator!=(const Transform& _transform) const;

	// 기본 생성자, 복사 생성자, 소멸자, 복사 대입 생성자는 기본적으로 만들어지는 것들
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
