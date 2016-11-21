#ifndef __VECTOR_H_
#define __VECTOR_H_

class Vector2
{
public:
	float x, y;

	Vector2(float _x = 0, float _y = 0) :x(_x), y(_y) {};
	Vector2(const Vector2& _vector2) { x = _vector2.x; y = _vector2.y; }

	~Vector2() {};

	Vector2& operator=(const Vector2& _vector2);
	const Vector2 operator+(const Vector2& _vector2) const;
	Vector2 operator+=(const Vector2& _vector2);
	const Vector2 operator-(const Vector2& _vector2) const;
	Vector2 operator-=(const Vector2& _vector2);
	const Vector2 operator*(const float value) const;
	Vector2 operator*=(const float value);
	const Vector2 operator/(const float value) const;
	Vector2 operator/=(const float value);

	const bool operator==(const Vector2& _vector2) const;
	const bool operator!=(const Vector2& vector2) const;
};

class Vector3 : public Vector2
{
public:
	float z;

	Vector3(float _x = 0, float _y = 0, float _z = 0) :Vector2(_x,_y), z(_z) {};
	explicit Vector3(const Vector2& _vector2) { x = _vector2.x; y = _vector2.y; z = 0; }
	Vector3(const Vector3& _vector3) { x = _vector3.x; y = _vector3.y; z = _vector3.z; }

	~Vector3() {};

	Vector3& operator=(const Vector3& _vector3);
	const Vector3 operator+(const Vector3& _vector3) const;
	Vector3 operator+=(const Vector3& _vector3);
	const Vector3 operator-(const Vector3& _vector3) const;
	Vector3 operator-=(const Vector3& _vector3);
	const Vector3 operator*(const float value) const;
	Vector3 operator*=(const float value);
	const Vector3 operator/(const float value) const;
	Vector3 operator/=(const float value);

	const bool operator==(const Vector3& _vector3) const;
	const bool operator!=(const Vector3& vector3) const;

	operator Vector2() const;
};
#endif // !__VECTOR_H_
