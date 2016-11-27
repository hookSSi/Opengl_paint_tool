#include "Object.h"

/* Transform 정의 */

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

/* Object 정의 */

Object::Object(const Object& other)
{
	this->transform = other.transform;
	this->color = other.color;
}

Object& Object::operator=(const Object& other)
{
	if (this != &other)
	{
		this->transform = other.transform;
		this->color = other.color;
	}

	return *this;
}

void Object::Draw()
{
	glPushAttrib(GL_ALL_ATTRIB_BITS);
	glColor4f(color.r, color.g, color.b, color.a);
}