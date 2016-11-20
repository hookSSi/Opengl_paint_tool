#ifndef  __BUTTON_H_
#define __BUTTON_H_

class Button
{
private:
	int x, y;
public:
	Button();
	Button(int posX, int posY);
	~Button();
	void CreateButton();
};

#endif // ! __BUTTON_H_
