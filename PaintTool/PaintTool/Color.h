#ifndef  __COLOR_H_
#define __COLOR_H_

class Color
{
private:
	void ResizeValue(Color& const _color) const;

public:
	float r, g, b, a;

	Color() { Color::Color(0, 0, 0, 0); }
	Color(float _r, float _g, float _b) : r(_r), g(_g), b(_b), a(1) { ResizeValue(*this); }
	Color(float _r, float _g, float _b, float _a) :r(_r), g(_g), b(_b), a(1) { ResizeValue(*this);}
	Color(const Color& _color) { r = _color.r; g = _color.g; b = _color.b; a = _color.a; };

	~Color() {};

	Color& operator=(const Color& _color);
	const Color operator+(const Color& _color) const;
	Color operator+=(const Color& _color);
	const Color operator-(const Color& _color) const;
	Color operator-=(const Color& _color);
	const Color operator*(const float value) const;
	Color operator*=(const float value);
	const Color operator/(const float value) const;
	Color operator/=(const float value);

	const bool operator==(const Color& _color) const;
	const bool operator!=(const Color& _color) const;
};

#endif // ! __COLOR_H_
