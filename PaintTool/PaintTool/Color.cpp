#include "Color.h"

// private
void Color::ResizeValue(Color& const _color) const
{
	if (_color.r < 0)
		_color.r = 0;
	else if (_color.r > 1)
		_color.r = 1;

	if (_color.g < 0)
		_color.g = 0;
	else if (_color.g > 1)
		_color.g = 1;

	if (_color.b < 0)
		_color.b = 0;
	else if (_color.b > 1)
		_color.b = 1;

	if (_color.a < 0)
		_color.a = 0;
	else if (_color.a > 1)
		_color.a = 1;
}

// public
Color::Color(const COLORREF& _color)
{
	this->r = ((BYTE)(_color));
	this->g = ((BYTE)(((WORD)(_color)) >> 8));
	this->b = ((BYTE)((_color) >> 16));

	this->r /= 256;
	this->g /= 256;
	this->b /= 256;
	this->a = 1;

	ResizeValue(*this);
}

Color& Color::operator=(const Color& _color)
{
	if (this != &_color)
	{
		this->r = _color.r;
		this->g = _color.g;
		this->b = _color.b;
		this->a = _color.a;
	}

	return *this;
}

const Color Color::operator+(const Color& _color) const
{
	Color newColor(0,0,0,0);
	newColor.r = this->r + _color.r;
	newColor.g = this->g + _color.g;
	newColor.b = this->b + _color.b;
	newColor.a = this->a + _color.a;

	ResizeValue(newColor);

	return newColor;
}

Color Color::operator+=(const Color& _color)
{
	this->r += _color.r;
	this->g += _color.g;
	this->b += _color.b;
	this->a += _color.a;

	ResizeValue(*this);

	return *this;
}

const Color Color::operator-(const Color& _color) const
{
	Color newColor(0, 0, 0, 0);
	newColor.r = this->r - _color.r;
	newColor.g = this->g - _color.g;
	newColor.b = this->b - _color.b;
	newColor.a = this->a - _color.a;

	ResizeValue(newColor);

	return newColor;
}

Color Color::operator-=(const Color& _color)
{
	this->r -= _color.r;
	this->g -= _color.g;
	this->b -= _color.b;
	this->a -= _color.a;

	ResizeValue(*this);

	return *this;
}

const Color Color::operator*(const float value) const
{
	Color newColor(0, 0, 0, 0);
	newColor.r = this->r * value;
	newColor.g = this->g * value;
	newColor.b = this->b * value;
	newColor.a = this->a * value;

	ResizeValue(newColor);

	return newColor;
}

Color Color::operator*=(const float value)
{
	this->r *= value;
	this->g *= value;
	this->b *= value;
	this->a *= value;

	ResizeValue(*this);

	return *this;
}

const Color Color::operator/(const float value) const
{
	Color newColor(0, 0, 0, 0);
	newColor.r = this->r / value;
	newColor.g = this->g / value;
	newColor.b = this->b / value;
	newColor.a = this->a / value;

	ResizeValue(newColor);

	return newColor;
}

Color Color::operator/=(const float value)
{
	this->r /= value;
	this->g /= value;
	this->b /= value;
	this->a /= value;

	ResizeValue(*this);

	return *this;
}

const bool Color::operator==(const Color& _color) const
{
	if (this->r == _color.r && this->g == _color.g && this->b == _color.b && this->a == _color.	a)
		return true;
	else
		return false;
}

const bool Color::operator!=(const Color& _color) const
{
	if (this->r != _color.r || this->g != _color.g || this->b != _color.b || this->a != _color.a)
		return true;
	else
		return false;
}