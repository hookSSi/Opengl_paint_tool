#ifndef __LAYER_H_
#define __LAYER_H_

#include<gl/glut.h>

class Layer
{
private:
	GLubyte *buffer; // È­¸é ÇÈ¼¿

public:
	bool isOn;
	

	Layer();
	Layer(const Layer& _layer);

	~Layer() { delete buffer; };

	const Layer& operator=(const Layer& _layer);

	void SetBuffer(const GLubyte& screenPixelData);
	const GLubyte* GetBuffer() const { return buffer; }
};

#endif // !__LAYER_H_
