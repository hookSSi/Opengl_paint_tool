#include "Object.h"

/* Transform ���� */

Transform::Transform(const Transform& _transform)
{
	this->position = _transform.position;
	this->scale = _transform.scale;
	this->rotation = _transform.rotation;
}

Transform& Transform::operator=(const Transform& _transform)
{
	if (this != &_transform)
	{
		this->position = _transform.position;
		this->scale = _transform.scale;
		this->rotation = _transform.rotation;
	}

	return *this;
}

bool Transform::operator==(const Transform& _transform) const
{
	if (this->position == _transform.position && this->rotation == _transform.rotation && this->scale == _transform.scale)
		return true;
	else
		return false;
}

bool Transform::operator!=(const Transform& _transform) const
{
	if (this->position != _transform.position && this->rotation != _transform.rotation && this->scale != _transform.scale)
		return true;
	else
		return false;
}

/* Object ���� */

Object::Object(const Object& other)
{
	this->transform = other.transform;
}

Object& Object::operator=(const Object& other)
{
	if (this != &other)
	{
		this->transform = other.transform;
	}

	return *this;
}

void Object::Draw()
{
	glTranslatef(transform.position.x, transform.position.y, transform.position.z); // �̵� ��ȯ

	glScalef(transform.scale.x, transform.scale.y, transform.scale.z); // ������ ��ȯ

	glRotatef(transform.rotation.x, 1.0f, 0.0f, 0.0f); // x�� ȸ��
	glRotatef(transform.rotation.y, 0.0f, 1.0f, 0.0f); // y�� ȸ��
	glRotatef(transform.rotation.z, 0.0f, 0.0f, 1.0f); // z�� ȸ��
}