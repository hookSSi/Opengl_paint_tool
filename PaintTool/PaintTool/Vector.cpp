#include "Vector.h"

/* Vector2에 대한 연산자 정의 */

Vector2& Vector2::operator=(const Vector2& _vector2)
{
	if (this != &_vector2)
	{
		this->x = _vector2.x;
		this->y = _vector2.y;
	}
	
	return *this;
}

const Vector2 Vector2::operator+(const Vector2& _vector2) const
{
	Vector2 newVector(0,0);
	newVector.x = this->x + _vector2.x;
	newVector.y = this->y + _vector2.y;

	return newVector;
}

Vector2 Vector2::operator+=(const Vector2& _vector2)
{
	this->x += _vector2.x;
	this->y += _vector2.y;

	return *this;
}

const Vector2 Vector2::operator-(const Vector2& _vector2) const
{
	Vector2 newVector(0, 0);
	newVector.x = this->x - _vector2.x;
	newVector.y = this->y - _vector2.y;

	return newVector;
}

Vector2 Vector2::operator-=(const Vector2& _vector2)
{
	this->x -= _vector2.x;
	this->y -= _vector2.y;

	return *this;
}

const Vector2 Vector2::operator*(const float value) const
{
	Vector2 newVector(0, 0);
	newVector.x = this->x * value;
	newVector.y = this->y * value;

	return newVector;
}

Vector2 Vector2::operator*=(const float value)
{
	this->x *= value;
	this->y *= value;

	return *this;
}

const Vector2 Vector2::operator/(const float value) const
{
	Vector2 newVector(0, 0);
	newVector.x = this->x / value;
	newVector.y = this->y / value;

	return newVector;
}

Vector2 Vector2::operator/=(const float value)
{
	this->x /= value;
	this->y /= value;

	return *this;
}

const bool Vector2::operator==(const Vector2& _vector2) const
{
	if (this->x == _vector2.x && this->y == _vector2.y)
		return true;
	else
		return false;
}

const bool Vector2::operator!=(const Vector2& _vector2) const
{
	if (this->x != _vector2.x || this->y != _vector2.y)
		return true;
	else
		return false;
}

/* Vector3에 대한 연산자 정의 */

Vector3& Vector3::operator=(const Vector3& _vector3)
{
	if (this != &_vector3)
	{
		this->x = _vector3.x;
		this->y = _vector3.y;
		this->z = _vector3.z;
	}

	return *this;
}

const Vector3 Vector3::operator+(const Vector3& _vector3) const
{
	Vector3 newVector(0, 0, 0);
	newVector.x = this->x + _vector3.x;
	newVector.y = this->y + _vector3.y;
	newVector.z = this->z + _vector3.z;

	return newVector;
}

Vector3 Vector3::operator+=(const Vector3& _vector3)
{
	this->x += _vector3.x;
	this->y += _vector3.y;
	this->z += _vector3.z;

	return *this;
}

const Vector3 Vector3::operator-(const Vector3& _vector3) const
{
	Vector3 newVector(0, 0 , 0);
	newVector.x = this->x - _vector3.x;
	newVector.y = this->y - _vector3.y;
	newVector.z = this->z - _vector3.z;

	return newVector;
}

Vector3 Vector3::operator-=(const Vector3& _vector3)
{
	this->x -= _vector3.x;
	this->y -= _vector3.y;
	this->z -= _vector3.z;

	return *this;
}

const Vector3 Vector3::operator*(const float value) const
{
	Vector3 newVector(0, 0, 0);
	newVector.x = this->x * value;
	newVector.y = this->y * value;
	newVector.z = this->z * value;

	return newVector;
}

Vector3 Vector3::operator*=(const float value)
{
	this->x *= value;
	this->y *= value;
	this->z *= value;

	return *this;
}

const Vector3 Vector3::operator/(const float value) const
{
	Vector3 newVector(0, 0, 0);
	newVector.x = this->x / value;
	newVector.y = this->y / value;
	newVector.z = this->z / value;

	return newVector;
}

Vector3 Vector3::operator/=(const float value)
{
	this->x /= value;
	this->y /= value;
	this->z /= value;

	return *this;
}

const bool Vector3::operator==(const Vector3& _vector3) const
{
	if (this->x == _vector3.x && this->y == _vector3.y && this->z == _vector3.z)
		return true;
	else
		return false;
}

const bool Vector3::operator!=(const Vector3& _vector3) const
{
	if (this->x != _vector3.x || this->y != _vector3.y || this->z != _vector3.z)
		return true;
	else
		return false;
}