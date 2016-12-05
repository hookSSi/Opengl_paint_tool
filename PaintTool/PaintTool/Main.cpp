#define WIN32_LEAN_AND_MEAN // ���ʿ��� �������� ��ɵ� ����
#define _CRT_SECURE_NO_WARNINGS 

// ������ �������
#include<Windows.h>
// ���ҽ� ����
#include "resource.h"
// C++ ���̺귯��
#include<iostream>
// OpenGL �������
#include <GL/glut.h>
// OpenGL ��ƿ��Ƽ
#include<gl/glu.h>
// OpenGL ���� �Լ���
#include<gl/glaux.h>
// stirng ���
#include<string>
#include <commctrl.h>

#include "FileHandle.h"
#include "Layer.h"
#include "Debug.h"
#include "Drawing.h"
#include "Util.h"
#include "Transformation.h"

const LPCWSTR PROGRAMNAME = TEXT("PaintHook"); // ���α׷� �̸�

HGLRC hRC = NULL; // ������ ���ؽ�Ʈ
HDC hDC = NULL; // GDI ��ġ ���ؽ�Ʈ
HWND hWnd = NULL; // ������ �ڵ�
HINSTANCE hInstance; // �������α׷� �ν��Ͻ�
HMENU hMenu = NULL; // �޴� �ν��Ͻ�

bool keys[256]; // Ű���� ��ƾ�� ����ϴ� �迭
bool active = TRUE; // ������ Ȱ��ȭ �÷���, ����Ʈ���� TRUE
bool fullscreen = TRUE; // ��üȭ�� �÷���, ����Ʈ���� TRUE


BITMAPINFOHEADER bitmapInfoHeader; // BMP Info Header

const int MAX_PATH_LENGTH = 40; // ���� ��� �ִ� ����

static wchar_t lpstrFile1[MAX_PATH_LENGTH]; // ���� ��� ���ڿ� ����
static wchar_t lpstrFile2[MAX_PATH_LENGTH];

unsigned char* imagebuffer; // �̹��� �а� ���� ����
unsigned char* imageData; // �̹��� ����
bool isOpen = false;

static int windowWidth = 640; // ������ �ʺ�
static int windowHeight = 480; // ������ ����

static int pixelWidth = windowWidth;
static int pixelHeight = windowHeight;



/* ���콺 �Է� ���� ����*/

static const Vector2 CENTER(windowWidth / 2, windowHeight / 2);

static Vector2 tempPos(CENTER);
static Vector2 mouseStartPos(CENTER);
static Vector2 mouseLastPos(CENTER);


static bool mouseButtonDown_L = false; // ���� ��ư UP
static bool mouseButtonDown_R = false; // ������ ��ư UP
static bool mouseButtonUp_L = true; // ���� ��ư DOWN
static bool mouseButtonUp_R = true; // ������ ��ư DOWN
static bool draging_R = false; // �巡����?
static bool draging_L = false;
static bool preview = true; // �̸����� Ȱ��ȭ?
static bool change = true; // �̹��� ��ȭ?


OPENFILENAME OFN; // ����
OPENFILENAME SFN; // ����

LRESULT CALLBACK WndProc(
	HWND hwnd, // �� â�� �ڵ�
	UINT message, // �� â�� �޽���
	WPARAM wParam,  // �߰� �޽��� ����
	LPARAM lParam // �߰� �޽��� ����
	);

int DrawGLScene(GLvoid); // ��� �Լ�

void ReadImageDatas(); // �̹��� ���� �б�
void WriteImageDatas(); // �̹��� ���� ����
void ObjectManager(Object &object, Color color, float size); // ������Ʈ ���� �Լ�
void DrawingManager(); // �׸��� �Լ�
void PreviewManager(); // �̸����� �Լ�
void DrawingRoutine(); // �׸��� ��ƾ


const int maxObjectSize = 30;
const int minObjectSize = 1;

const int MODE_POINT = 0; // ��
const int MODE_LINE = 1; // ��
const int MODE_CIRCLE = 2; // ��
const int MODE_RECT = 3; // �簢��
const int MODE_TRIANGLE = 4; // �ﰢ��
const int MODE_ERASER = 5; // ���찳
const int MODE_CHAR = 6; // �ؽ�Ʈ
const int MODE_HEART = 7; // ��Ÿ ����
const int MODE_STAR = 8;

static int mode = 0;
static bool fill_Circle = true;
static bool fill_Rect = true;
static bool fill_Tri = true;
static bool fill_ETC = true;
static bool randColor = false;
static bool keyboardInputMode = false;
static float objectSize = 5;

static Drawing::Point point;
static Drawing::Line line;
static Drawing::Circle circle;
static Drawing::Rectangle rect;
static Drawing::Triangle triangle;
static Drawing::Text text;
static Drawing::Line line1; // �ﰢ�� �� ����
static Drawing::Heart heart;
static Drawing::Star star;

//static Drawing::TEXT text;

/* ������ ���� */
CHOOSECOLOR colorChooser;
static COLORREF arrayCustomColor[16];
static DWORD rgbCurrent;

const static Color WHITE = Color(1, 1, 1);
const static Color BLACK = Color(0, 0, 0);

static Color colorMode[2] = { BLACK,WHITE }; // ����, ������

void ReadImageDatas()
{
	if (imageData != nullptr)
	{
		glPushAttrib(GL_ALL_ATTRIB_BITS);
		glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
		glRasterPos2i(0, 0);

		glDrawPixels(pixelWidth, pixelHeight, GL_RGB, GL_UNSIGNED_BYTE, imageData);
		glFlush();
		glPopAttrib();
	}
}

void WriteImageDatas()
{
	if (imageData == nullptr)
	{
		imageData = (unsigned char*)malloc(pixelWidth * pixelHeight * 3);
		memset(imageData, 1, pixelWidth * pixelHeight * 3);
	}
	else
	{
		realloc(imageData, sizeof(unsigned char) * (pixelWidth * pixelHeight * 3));
		memset(imageData, 1, pixelWidth * pixelHeight * 3);
	}
	
	glReadPixels(0,0, pixelWidth * 1, pixelHeight * 1, GL_RGB, GL_UNSIGNED_BYTE, imageData);
}

GLvoid ReSizeGLScene(GLsizei width, GLsizei height) // GL �����츦 �ʱ�ȭ�ϰ� ũ�⸦ �����Ѵ�.
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0.0, (GLdouble)width, 0.0, (GLdouble)height, -1.0, 1.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	/* adjust viewport and  clear */	

	glViewport(0, 0, width, height);
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f); // ���� ����
	glClear(GL_COLOR_BUFFER_BIT);
	ReadImageDatas();
	DrawGLScene();
	glFlush();

	/* set global size for use by drawing routine */

	windowWidth = width;
	windowHeight = height;

	change = true;
}

/* ���ڿ� ��� ���� �Լ� */
static GLuint base;

GLvoid BuildFont()
{
	HFONT font; // <1>
	base = glGenLists(96); // <2>
	font = CreateFont(-24, // <3-1>
		0,
		0,
		0,
		FW_BOLD, // <3-2>
		FALSE, // <3-3>
		FALSE, // <3-4>
		FALSE, // <3-5>
		ANSI_CHARSET, // <3-6>
		OUT_TT_PRECIS,
		CLIP_DEFAULT_PRECIS,
		ANTIALIASED_QUALITY,
		FF_DONTCARE | DEFAULT_PITCH,
		TEXT("Courier New")); // <3-6>

	SelectObject(hDC, font); // <4>
	wglUseFontBitmaps(hDC, 32, 96, base); // <5>
	text.base = base;
}

GLvoid KillFont(GLvoid)
{
	glDeleteLists(base, 96);
}


int InitGL(GLvoid)
{
	ReSizeGLScene(windowWidth, windowHeight);
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f); // ���� ����

	BuildFont();

	return TRUE; // �ʱ�ȭ�� ������ ������
}

int DrawGLScene(GLvoid) // ��� ������� ó���ϴ� ��
{
	//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();

	glPushAttrib(GL_ALL_ATTRIB_BITS);
	if (isOpen)
	{
		glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
		glRasterPos2i(0, 0);

		glDrawPixels(bitmapInfoHeader.biWidth, bitmapInfoHeader.biHeight, GL_RGB, GL_UNSIGNED_BYTE, imagebuffer);
		isOpen = FALSE;
		change = TRUE;
	}
	glFlush();
	glPopAttrib();

	DrawingRoutine();

	return TRUE; // ������ ��ħ
}

void ObjectManager(Object &object, Color color, float size) // �׸����� ������Ʈ ����
{
	object.color = color;
	object.transform.scale = Vector3(size,size,0);
}

void PreviewManager()
{
	glClear(GL_COLOR_BUFFER_BIT); // �̸����� �����
	ReadImageDatas(); // �̹��� �ҷ�����

	switch (mode)
	{
	case MODE_POINT:
	{
		ObjectManager(point, colorMode[0], objectSize);

		point.transform.position = (Vector3)mouseLastPos;
		tempPos = mouseLastPos;
		point.Draw();
		break;
	}
	case MODE_LINE:
	{
		if (draging_L)
		{
			ObjectManager(line, colorMode[0], objectSize); // ������Ʈ ����

			line.transform.position = (Vector3)mouseStartPos;
			line.Draw(mouseLastPos.x, mouseLastPos.y);
		}
		else if (draging_R)
		{
			ObjectManager(line, colorMode[1], objectSize); // ������Ʈ ����

			line.transform.position = (Vector3)mouseStartPos;
			line.Draw(mouseLastPos.x, mouseLastPos.y);
		}
		else
		{
			ObjectManager(point, colorMode[0], 1); // ������Ʈ ����			
			point.transform.position = (Vector3)mouseLastPos;
			point.Draw();
		}
		break;
	}
	case MODE_CIRCLE:
	{
		if (draging_L)
		{
			ObjectManager(circle, colorMode[0], objectSize); // ������Ʈ ����

			circle.transform.position = (Vector3)mouseStartPos;
			circle.Draw(mouseLastPos.x, mouseLastPos.y, fill_Circle);
		}
		else if (draging_R)
		{
			ObjectManager(circle, colorMode[1], objectSize); // ������Ʈ ����

			circle.transform.position = (Vector3)mouseStartPos;
			circle.Draw(mouseLastPos.x, mouseLastPos.y, fill_Circle);
		}
		else
		{
			ObjectManager(point, colorMode[0], 1);
			point.transform.position = (Vector3)mouseLastPos;
			point.Draw();
		}
		break;
	}
	case MODE_RECT:
	{
		if (draging_L)
		{
			ObjectManager(rect, colorMode[0], objectSize); // ������Ʈ ����

			rect.transform.position = (Vector3)mouseStartPos;
			rect.Draw(mouseLastPos.x, mouseLastPos.y, fill_Rect);
		}
		else if (draging_R)
		{
			ObjectManager(rect, colorMode[1], objectSize); // ������Ʈ ����

			rect.transform.position = (Vector3)mouseStartPos;
			rect.Draw(mouseLastPos.x, mouseLastPos.y, fill_Rect);
		}
		else
		{
			ObjectManager(point, colorMode[0], 1);
			point.transform.position = (Vector3)mouseLastPos;
			point.Draw();
		}

		break;
	}
	case MODE_TRIANGLE:
	{
		if (triangle.count == 0 && mouseButtonDown_L)
		{
			ObjectManager(line1, colorMode[0], objectSize);

			line1.transform.position = (Vector3)mouseStartPos;
		}
		else if (triangle.count == 0 && mouseButtonDown_R)
		{
			ObjectManager(line1, colorMode[1], objectSize);

			line1.transform.position = (Vector3)mouseStartPos;
		}

		if (triangle.count == 0 && (draging_L || draging_R))
		{
			line1.Draw(mouseLastPos.x, mouseLastPos.y);
		}

		if (triangle.count == 1 && (mouseButtonUp_L || mouseButtonUp_R))
		{
			triangle.Draw(mouseLastPos.x, mouseLastPos.y, fill_Tri);
		}
		else
		{
			ObjectManager(point, colorMode[0], 1);
			point.transform.position = (Vector3)mouseLastPos;
			point.Draw();
		}

		break;
	}
	case MODE_CHAR:
	{
		if (mouseButtonDown_L)
		{
			ObjectManager(text, colorMode[0], objectSize);
			text.transform.position = (Vector3)mouseStartPos;
			keyboardInputMode = true;
		}

		if (!keys[VK_RETURN])
		{
			if (keyboardInputMode)
			{
				text.Draw();
			}
		}
		else if(keys[VK_RETURN])
		{
			keyboardInputMode = false;
		}

		break;
	}
	case MODE_ERASER:
	{
		ObjectManager(point, WHITE, objectSize);

		point.transform.position = (Vector3)mouseLastPos;
		tempPos = mouseLastPos;
		point.Draw();
		break;
	}
	case MODE_HEART:
	{
		if (draging_L)
		{
			ObjectManager(heart, colorMode[0], objectSize); // ������Ʈ ����

			heart.transform.position = (Vector3)mouseStartPos;
			heart.Draw(mouseLastPos.x, mouseLastPos.y, fill_ETC);
		}
		else if (draging_R)
		{
			ObjectManager(heart, colorMode[1], objectSize); // ������Ʈ ����

			heart.transform.position = (Vector3)mouseStartPos;
			heart.Draw(mouseLastPos.x, mouseLastPos.y, fill_ETC);
		}
		else
		{
			ObjectManager(point, colorMode[0], 1);
			point.transform.position = (Vector3)mouseLastPos;
			point.Draw();
		}
		break;
	}
	case MODE_STAR:
	{
		if (draging_L)
		{
			ObjectManager(star, colorMode[0], objectSize); // ������Ʈ ����

			star.transform.position = (Vector3)mouseStartPos;
			star.Draw(mouseLastPos.x, mouseLastPos.y, fill_ETC);
		}
		else if (draging_R)
		{
			ObjectManager(star, colorMode[1], objectSize); // ������Ʈ ����

			star.transform.position = (Vector3)mouseStartPos;
			star.Draw(mouseLastPos.x, mouseLastPos.y, fill_ETC);
		}
		else
		{
			ObjectManager(point, colorMode[0], 1);
			point.transform.position = (Vector3)mouseLastPos;
			point.Draw();
		}
		break;
	}
	}
}

void DrawingRoutine()
{

	switch (mode)
	{
		/*DrawingManager();

		if (change)
		{
			if (randColor)
			{
				colorMode[0] = Color((float)(rand() % 256) / 256.0f, (float)(rand() % 256) / 256.0f, (float)(rand() % 256) / 256.0f);
				colorMode[1] = Color((float)(rand() % 256) / 256.0f, (float)(rand() % 256) / 256.0f, (float)(rand() % 256) / 256.0f);
			}

			WriteImageDatas();
			change = false;
		}
		else if (preview)
		{
			Sleep(15);
			PreviewManager();
			SwapBuffers(hDC);
		}

		Sleep(15);
		SwapBuffers(hDC);
		PreviewManager();
		SwapBuffers(hDC);
		Sleep(15);
		break;*/
	case MODE_POINT:
	case MODE_STAR:
	case MODE_HEART:
	case MODE_ERASER:
	case MODE_TRIANGLE:
	case MODE_RECT:
	case MODE_CIRCLE:
	case MODE_LINE:
	case MODE_CHAR:
		if (change)
		{
			if (randColor)
			{
				colorMode[0] = Color((float)(rand() % 256) / 256.0f, (float)(rand() % 256) / 256.0f, (float)(rand() % 256) / 256.0f);
				colorMode[1] = Color((float)(rand() % 256) / 256.0f, (float)(rand() % 256) / 256.0f, (float)(rand() % 256) / 256.0f);
			}

			WriteImageDatas();
			change = false;
		}
		else if (preview)
		{
			PreviewManager();
			SwapBuffers(hDC);
		}

		Sleep(15);
		SwapBuffers(hDC);
		DrawingManager();
		SwapBuffers(hDC);
		Sleep(15);
		break;
	}
}

void DrawingManager()
{
	switch (mode)
	{
		/* �� */
	case MODE_POINT:
	{
		if (draging_L || mouseButtonDown_L)
		{
			ObjectManager(point, colorMode[0], objectSize);
			point.transform.position = (Vector3)mouseLastPos;
			point.Draw();
			preview = false;
			change = true;
		}
		else if (draging_R || mouseButtonDown_R)
		{
			ObjectManager(point, colorMode[1], objectSize);
			point.transform.position = (Vector3)mouseLastPos;
			point.Draw();
			preview = false;
			change = true;
		}
	}
	break;
	/* �� */
	case MODE_LINE:
	{
		if (mouseButtonDown_L)
		{
			ObjectManager(line, colorMode[0], objectSize);
			line.transform.position = (Vector3)mouseStartPos;
		}
		else if (mouseButtonDown_R)
		{
			ObjectManager(line, colorMode[1], objectSize);
			line.transform.position = (Vector3)mouseStartPos;
		}

		if (!preview)
		{
			line.Draw(mouseLastPos.x, mouseLastPos.y);
			preview = true;
			change = true;
		}
	}
	break;
	/* �� */
	case MODE_CIRCLE:
	{
		if (mouseButtonDown_L)
		{
			ObjectManager(circle, colorMode[0], objectSize);
			circle.transform.position = (Vector3)mouseStartPos;
		}
		else if (mouseButtonDown_R)
		{
			ObjectManager(circle, colorMode[1], objectSize);
			circle.transform.position = (Vector3)mouseStartPos;
		}

		if (!preview)
		{
			circle.Draw(mouseLastPos.x, mouseLastPos.y,fill_Circle);
			preview = true;
			change = true;
		}
		break;
	}
	/* �簢�� */
	case MODE_RECT:
	{
		if (mouseButtonDown_L)
		{
			ObjectManager(rect, colorMode[0], objectSize);
			rect.transform.position = (Vector3)mouseStartPos;
		}
		else if (mouseButtonDown_R)
		{
			ObjectManager(rect, colorMode[1], objectSize);
			rect.transform.position = (Vector3)mouseStartPos;
		}
		if (!preview)
		{
			rect.Draw(mouseLastPos.x, mouseLastPos.y, fill_Rect);
			preview = true;
			change = true;
		}
		break;
	}
	/* �ﰢ�� */
	case MODE_TRIANGLE:
	{
		if (mouseButtonDown_L && triangle.count == 0)
		{
			ObjectManager(triangle, colorMode[0], objectSize);
			triangle.transform.position = (Vector3)mouseStartPos;
		}
		else if (mouseButtonDown_R && triangle.count == 0)
		{
			ObjectManager(triangle, colorMode[1], objectSize);
			triangle.transform.position = (Vector3)mouseStartPos;
		}

		if (triangle.count == 0 && (draging_L || draging_R))
		{
			triangle.secondPos = mouseLastPos;
		}
		
		if (triangle.count == 2 && (mouseButtonDown_L || mouseButtonDown_R))
		{
			triangle.Draw(mouseStartPos.x, mouseStartPos.y, fill_Tri);
			triangle.PlusCount();
			change = true;
		}
		break;
	}
	/* �ؽ�Ʈ */
	case MODE_CHAR: 
	{
		if (!keyboardInputMode && keys[VK_RETURN])
		{
			text.Draw();
			text.textList.clear();
			keyboardInputMode = false;
			change = true;
		}
		break;
	}
	/* ���찳 */
	case MODE_ERASER:
	{
		ObjectManager(point, WHITE, objectSize);

		if (mouseButtonDown_L || mouseButtonDown_R)
		{
			point.transform.position = (Vector3)mouseLastPos;
			point.Draw();
			change = true;
		}

		ObjectManager(triangle, BLACK, objectSize);
	}
	break;
	/* ��Ÿ ������ */
	case MODE_HEART:
		if (mouseButtonDown_L)
		{
			ObjectManager(heart, colorMode[0], objectSize);
			heart.transform.position = (Vector3)mouseStartPos;
		}
		else if (mouseButtonDown_R)
		{
			ObjectManager(heart, colorMode[1], objectSize);
			heart.transform.position = (Vector3)mouseStartPos;
		}
		if (!preview)
		{
			heart.Draw(mouseLastPos.x, mouseLastPos.y, fill_ETC);
			preview = true;
			change = true;
		}
		break;
	case MODE_STAR:
		if (mouseButtonDown_L)
		{
			ObjectManager(star, colorMode[0], objectSize);
			star.transform.position = (Vector3)mouseStartPos;
		}
		else if (mouseButtonDown_R)
		{
			ObjectManager(star, colorMode[1], objectSize);
			star.transform.position = (Vector3)mouseStartPos;
		}

		if (!preview)
		{
			star.Draw(mouseLastPos.x, mouseLastPos.y, fill_ETC);
			preview = true;
			change = true;
		}
		break;
	}
}

// Ű���� �Է� �Ŵ��� �Լ�
void KeyboardInputManager(WPARAM &wParam)
{
	if (!keyboardInputMode && mode == MODE_CHAR && !keys[VK_RETURN])
		keyboardInputMode = true;

	if (mode == MODE_CHAR && keys[VK_BACK])
	{
		text.DeleteChar();
	}
	else if (mode == MODE_CHAR && keyboardInputMode)
	{
		unsigned char ch = MapVirtualKey(wParam, MAPVK_VK_TO_CHAR);
		if (ch == 229) // �ѱ� �Է� ����
			return;
		text.AddChar(ch);
	}
}

// �޴� �Ŵ��� �Լ�
void MenuManager(WPARAM &wParam, LPARAM &lParam)
{
	switch (LOWORD(wParam))
	{
		/* ���� */
	case MENU1_NEWFILE:
		glClear(GL_COLOR_BUFFER_BIT);
		WriteImageDatas();
		return;
	case MENU1_OPEN:
		memset(&OFN, 0, sizeof(OPENFILENAME));
		OFN.lStructSize = sizeof(OPENFILENAME);
		OFN.hwndOwner = hWnd;
		OFN.lpstrFilter = TEXT("BMP files(*.bmp)\0*.bmp");;
		OFN.lpstrFile = lpstrFile1;
		OFN.nMaxFile = 256;
		OFN.lpstrInitialDir = TEXT("c:\\");
		if (GetOpenFileName(&OFN) != 0)
		{
			imagebuffer = BMP::LoadBMP(Util::ConvertWCtoC(OFN.lpstrFile), &bitmapInfoHeader); // �о ImageBuffer�� ����
			isOpen = true;
		}
		return;
	case MENU1_SAVE:
		WriteImageDatas();
		BMP::SaveScreenshot(pixelWidth, pixelHeight, imageData);
		return;
	case MENU1_SAVE_ANOTHER_NAME:
		memset(&SFN, 0, sizeof(OPENFILENAME));
		SFN.lStructSize = sizeof(OPENFILENAME);
		SFN.hwndOwner = hWnd;
		SFN.lpstrFilter = TEXT("BMP files(*.bmp)\0*.bmp");;
		SFN.lpstrFile = lpstrFile2;
		SFN.nMaxFile = 256;
		SFN.lpstrInitialDir = TEXT(".");
		if (GetSaveFileName(&SFN) != 0)
		{
			std::string fileName;
			fileName.append(Util::ConvertWCtoC(SFN.lpstrFile));

			if (strstr(fileName.data(), "bmp") == nullptr)
			{
				fileName.append(".bmp");
			}
			WriteImageDatas();
			BMP::SaveBMP(fileName.data(), pixelWidth, pixelHeight, imageData); // �̹��� ���� ����
		}
		return;
	case MENU1_EXIT:
		PostQuitMessage(0); // ���� �޽����� ����
		return;
		/* �� */
	case ID_POINT:
		mode = MODE_POINT;
		return;
		/* �� */
	case ID_LINE:
		mode = MODE_LINE;
		return;
		/* ���� */
	case ID_CIRCLE_FILL:
		fill_Circle = !fill_Circle;
		if (fill_Circle)
			CheckMenuItem(hMenu, ID_CIRCLE_FILL, MF_CHECKED);
		else
			CheckMenuItem(hMenu, ID_CIRCLE_FILL, MF_UNCHECKED);
		return;
	case ID_RECT_FILL:
		fill_Rect = !fill_Rect;
		if (fill_Rect)
			CheckMenuItem(hMenu, ID_RECT_FILL, MF_CHECKED);
		else
			CheckMenuItem(hMenu, ID_RECT_FILL, MF_UNCHECKED);
		return;
	case ID_TRI_FILL:
		fill_Tri = !fill_Tri;
		if (fill_Tri)
			CheckMenuItem(hMenu, ID_TRI_FILL, MF_CHECKED);
		else
			CheckMenuItem(hMenu, ID_TRI_FILL, MF_UNCHECKED);
		return;
	case ID_FILL_ETC:
		fill_ETC = !fill_ETC;
		if (fill_ETC)
			CheckMenuItem(hMenu, ID_FILL_ETC, MF_CHECKED);
		else
			CheckMenuItem(hMenu, ID_FILL_ETC, MF_UNCHECKED);
		return;
	case ID_CIRCLE_COORD:
		mode = MODE_CIRCLE;
		circle.mode = 0;

		CheckMenuItem(hMenu, ID_CIRCLE_COORD, MF_CHECKED);
		CheckMenuItem(hMenu, ID_CIRCLE_POLAR, MF_UNCHECKED);
		CheckMenuItem(hMenu, ID_CIRCLE_BRESENHAM, MF_UNCHECKED);
		CheckMenuItem(hMenu, ID_RECT, MF_UNCHECKED);
		CheckMenuItem(hMenu, ID_TRI, MF_UNCHECKED);
		CheckMenuItem(hMenu, ID_HEART, MF_UNCHECKED);
		CheckMenuItem(hMenu, ID_STAR, MF_UNCHECKED);
		return;
	case ID_CIRCLE_POLAR:
		mode = MODE_CIRCLE;
		circle.mode = 1;

		CheckMenuItem(hMenu, ID_CIRCLE_COORD, MF_UNCHECKED);
		CheckMenuItem(hMenu, ID_CIRCLE_POLAR, MF_CHECKED);
		CheckMenuItem(hMenu, ID_CIRCLE_BRESENHAM, MF_UNCHECKED);
		CheckMenuItem(hMenu, ID_RECT, MF_UNCHECKED);
		CheckMenuItem(hMenu, ID_TRI, MF_UNCHECKED);
		CheckMenuItem(hMenu, ID_HEART, MF_UNCHECKED);
		CheckMenuItem(hMenu, ID_STAR, MF_UNCHECKED);
		return;
	case ID_CIRCLE_BRESENHAM:
		mode = MODE_CIRCLE;
		circle.mode = 2;

		CheckMenuItem(hMenu, ID_CIRCLE_COORD, MF_UNCHECKED);
		CheckMenuItem(hMenu, ID_CIRCLE_POLAR, MF_UNCHECKED);
		CheckMenuItem(hMenu, ID_CIRCLE_BRESENHAM, MF_CHECKED);
		CheckMenuItem(hMenu, ID_RECT, MF_UNCHECKED);
		CheckMenuItem(hMenu, ID_TRI, MF_UNCHECKED);
		CheckMenuItem(hMenu, ID_HEART, MF_UNCHECKED);
		CheckMenuItem(hMenu, ID_STAR, MF_UNCHECKED);
		return;
	case ID_RECT:
		mode = MODE_RECT;

		CheckMenuItem(hMenu, ID_CIRCLE_COORD, MF_UNCHECKED);
		CheckMenuItem(hMenu, ID_CIRCLE_POLAR, MF_UNCHECKED);
		CheckMenuItem(hMenu, ID_CIRCLE_BRESENHAM, MF_UNCHECKED);
		CheckMenuItem(hMenu, ID_RECT, MF_CHECKED);
		CheckMenuItem(hMenu, ID_TRI, MF_UNCHECKED);
		CheckMenuItem(hMenu, ID_HEART, MF_UNCHECKED);
		CheckMenuItem(hMenu, ID_STAR, MF_UNCHECKED);
		return;
	case ID_TRI:
		mode = MODE_TRIANGLE;
		triangle.count = 0;

		CheckMenuItem(hMenu, ID_CIRCLE_COORD, MF_UNCHECKED);
		CheckMenuItem(hMenu, ID_CIRCLE_POLAR, MF_UNCHECKED);
		CheckMenuItem(hMenu, ID_CIRCLE_BRESENHAM, MF_UNCHECKED);
		CheckMenuItem(hMenu, ID_RECT, MF_UNCHECKED);
		CheckMenuItem(hMenu, ID_TRI, MF_CHECKED);
		CheckMenuItem(hMenu, ID_HEART, MF_UNCHECKED);
		CheckMenuItem(hMenu, ID_STAR, MF_UNCHECKED);
		return;
		/* �ؽ�Ʈ */
	case ID_CHAR:
		mode = MODE_CHAR;
		return;
		/* ���찳 */
	case ID_ERASER:
		mode = MODE_ERASER;
		return;
		/* �� */
	case ID_COLOR_BOX:
		ZeroMemory(&colorChooser, sizeof(colorChooser));
		colorChooser.lStructSize = sizeof(colorChooser);
		colorChooser.hwndOwner = hWnd;
		colorChooser.lpCustColors = (LPDWORD)arrayCustomColor;
		colorChooser.rgbResult = rgbCurrent;
		colorChooser.Flags = CC_FULLOPEN | CC_RGBINIT;

		if (ChooseColor(&colorChooser) == TRUE)
		{
			if(MessageBox(NULL, TEXT("�÷��� �����ų �귯���� �����ϼ���. \n �� - ���� �ƴϿ� - ������") , TEXT("�귯�� ����"), MB_YESNO | MB_ICONQUESTION) == IDYES)
			{
				colorMode[0] = Color(colorChooser.rgbResult);
			}
			else
			{
				colorMode[1] = Color(colorChooser.rgbResult);
			}
		}
		return;
	case ID_COLOR_RAND:
		randColor = !randColor;
		if (randColor)
			CheckMenuItem(hMenu, ID_COLOR_RAND, MF_CHECKED);
		else
			CheckMenuItem(hMenu, ID_COLOR_RAND, MF_UNCHECKED);
		return;
	/* ��ü ����� */
	case ID_ALL_CLEAR:
		glClear(GL_COLOR_BUFFER_BIT);
		WriteImageDatas();
		return;		
	/* ��ȯ ���� */
	case ID_FS_DITHERING:
		FS_dither(imageData, pixelWidth * 3, pixelHeight);
		//Ordered_dither(imageData, pixelWidth * 3, pixelHeight);
		return;
	case ID_RGB_BGR:
		RGB_BGR_Trans(imageData, pixelWidth * 3, pixelHeight);
		return;
	case ID_HEART:
		mode = MODE_HEART;

		CheckMenuItem(hMenu, ID_CIRCLE_COORD, MF_UNCHECKED);
		CheckMenuItem(hMenu, ID_CIRCLE_POLAR, MF_UNCHECKED);
		CheckMenuItem(hMenu, ID_CIRCLE_BRESENHAM, MF_UNCHECKED);
		CheckMenuItem(hMenu, ID_RECT, MF_UNCHECKED);
		CheckMenuItem(hMenu, ID_TRI, MF_UNCHECKED);
		CheckMenuItem(hMenu, ID_HEART, MF_CHECKED);
		CheckMenuItem(hMenu, ID_STAR, MF_UNCHECKED);
		return;
	case ID_STAR:
		mode = MODE_STAR;
		CheckMenuItem(hMenu, ID_CIRCLE_COORD, MF_UNCHECKED);
		CheckMenuItem(hMenu, ID_CIRCLE_POLAR, MF_UNCHECKED);
		CheckMenuItem(hMenu, ID_CIRCLE_BRESENHAM, MF_UNCHECKED);
		CheckMenuItem(hMenu, ID_RECT, MF_UNCHECKED);
		CheckMenuItem(hMenu, ID_TRI, MF_UNCHECKED);
		CheckMenuItem(hMenu, ID_HEART, MF_UNCHECKED);
		CheckMenuItem(hMenu, ID_STAR, MF_CHECKED);
		return;
	}
}

GLvoid KillGLWindow(GLvoid) // ���α׷��� ����Ǳ� �ٷ� ���� �����
{
	if (fullscreen)
	{
		ChangeDisplaySettings(NULL, 0); // ��üȭ���̸� ����ũž���� ���ư�
		ShowCursor(TRUE); // ���콺 ������ ǥ��
	}

	if (hRC)// ������ ���ؽ�Ʈ�� ������ �ִ��� �˻�
	{
		if (!wglMakeCurrent(NULL, NULL)) // DC�� RC ���ؽ�Ʈ�� ������ �� �ִ��� Ȯ��
		{
			MessageBox(NULL, TEXT("DC�� RC�� ������ ������"), TEXT("����"), MB_OK | MB_ICONINFORMATION);
		}

		if (!wglDeleteContext(hRC)) // RC�� ����� �ִ��� Ȯ��
		{
			MessageBox(NULL, TEXT("��ġ ���ؽ�Ʈ ������ ������"), TEXT("���ῡ��"), MB_OK | MB_ICONINFORMATION);
			hDC = NULL; // DC�� NULL�� ����
		}

		if (hWnd && !DestroyWindow(hWnd)) // â�� �ı��� �� �ִ��� Ȯ��
		{
			MessageBox(NULL, TEXT("hWnd ������ ������"), TEXT("���ῡ��"), MB_OK | MB_ICONINFORMATION);
			hWnd = NULL; // hWnd�� NULL�� ����
		}

		if (!UnregisterClass(TEXT("MyClass"), hInstance)) // ��ϵ� Ŭ������ ����� �ִ��� Ȯ��
		{
			MessageBox(NULL, TEXT("�������� Ŭ������ ����� ������ �� ����"), TEXT("���ῡ��"), MB_OK | MB_ICONINFORMATION);
			hInstance = NULL; // hInstance�� NULL�� �ٲ�
		}

	}
}

BOOL CreateGLWindow(LPCWSTR title, int width, int height, int bits, bool fullScreenFlag)
{
	GLuint PixelFormat; // ��ġ�ϴ� ������ ã�� ����� ������

	WNDCLASSEX windowClass; // ������ Ŭ����

	DWORD dwExStyle; // ������ Ȯ�� ��Ÿ��
	DWORD dwStyle; // ������ ��Ÿ��

	RECT windowRect; // �簢���� �»�ܰ� / ���ϴ� ���� ������
	windowRect.left = (long)0; // ���� ���� 0���� ����
	windowRect.right = (long)width; // ������ ���� ��û�� �ʺ�� ����
	windowRect.top = (long)0; // ���� ���� 0���� ����
	windowRect.bottom = (long)height; // �Ʒ��� ���� ��û�� ���̷� ����

	fullscreen == fullScreenFlag;

	// ������ Ŭ���� ����ü�� ä���
	windowClass.cbSize = sizeof(WNDCLASSEX);
	windowClass.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC; // ������ ��Ÿ��
	windowClass.lpfnWndProc = (WNDPROC)WndProc; // �޽��� ó�� �Լ�
	windowClass.cbClsExtra = 0;
	windowClass.cbWndExtra = 0;
	windowClass.hInstance = hInstance; // �� ������ Ŭ������ ����ϴ� ���α׷� ��ȣ
	windowClass.hIcon = LoadIcon(NULL, IDI_APPLICATION); // �ּ�ȭ ������
	windowClass.hCursor = LoadCursor(NULL, IDC_ARROW); // ���콺 Ŀ��
	windowClass.hbrBackground = NULL; // ������ ����
	windowClass.lpszMenuName = MAKEINTRESOURCE(MENU1); // ����� �޴�
	windowClass.lpszClassName = TEXT("MyClass"); // ������ Ŭ������ �̸�
	windowClass.hIconSm = LoadIcon(NULL, IDI_WINLOGO);

	if (!RegisterClassEx(&windowClass)) // ������ Ŭ������ ����� �õ���
	{
		MessageBox(NULL, TEXT("������ Ŭ������ ����ϴµ� ��������"), TEXT("����"), MB_OK | MB_ICONEXCLAMATION);
		return FALSE;
	}

	if (fullscreen)
	{
		DEVMODE dmScreenSettings;                    // ��ġ���
		memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));        // �޸𸮸� �ݵ�� ������
		dmScreenSettings.dmSize = sizeof(dmScreenSettings);        // Devmode ����ü�� ũ��
		dmScreenSettings.dmPelsWidth = width;            // ���õ� ȭ��ʺ�
		dmScreenSettings.dmPelsHeight = height;            // ���õ� ȭ�����
		dmScreenSettings.dmBitsPerPel = bits;                // ���õ� �ȼ��� ��Ʈ
		dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

		if (ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN) != DISP_CHANGE_SUCCESSFUL)
		{
			if (MessageBox(NULL, TEXT("������� ���� ī�尡 ��û�Ͻ� ��üȭ�� ��带 ����\n���� �ʽ��ϴ�. ��� â��带 ����ұ��?"), TEXT("MyOpenGL"), MB_YESNO | MB_ICONEXCLAMATION) == IDYES)
			{
				fullscreen = FALSE;
			}
			else
			{
				MessageBox(NULL, TEXT("���α׷��� ����� ���Դϴ�"), TEXT("����"), MB_OK | MB_ICONSTOP);
				return FALSE; // ���� �� FALSE��ȯ
			}
		}
	}
	if (fullscreen) // ������ ��üȭ�� ������� Ȯ��
	{
		dwExStyle = WS_EX_APPWINDOW; // ������ Ȯ�� ��Ÿ��
		dwStyle = WS_POPUP; // �������� ��Ÿ��
		ShowCursor(TRUE); // ���콺 �����͸� ����
	}
	else
	{
		dwExStyle = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE; // ������ Ȯ�� ��Ÿ��
		dwStyle = WS_OVERLAPPEDWINDOW; // �������� ��Ÿ��
	}

	AdjustWindowRectEx(&windowRect, dwStyle, FALSE, dwExStyle); // ��û�� ũ��� ������ ũ�⸦ �����Ѵ�

	if (!(hWnd = CreateWindowEx(
		dwExStyle, // �� ������� Ȯ�彺Ÿ��
		TEXT("MyClass"), // Ŭ���� �̸�
		title, // ������ Ÿ��Ʋ
		WS_CLIPSIBLINGS | // �ʼ� ������ ��Ÿ��
		WS_CLIPCHILDREN | // �ʼ� ������ ��Ÿ��
		dwStyle,  // ���õ� ������ ��Ÿ��
		0, 0, // â ��ġ
		windowRect.right - windowRect.left, // ������ â �ʺ� �����
		windowRect.bottom - windowRect.top, // ������ â ���̸� �����
		NULL, // �θ� ������ ����
		NULL, // �޴� ����
		hInstance,  // �ν��Ͻ�
		NULL
		)))
	{
		KillGLWindow(); // ���÷��̸� ������
		MessageBox(NULL, TEXT("������ ���� ����"), TEXT("����"), MB_OK | MB_ICONEXCLAMATION);
		return FALSE;
	}


	static PIXELFORMATDESCRIPTOR pfd = {
		sizeof(PIXELFORMATDESCRIPTOR), // ����ü�� ũ��
		1, // ���� -> �׻� 1�� ������
		PFD_DRAW_TO_WINDOW | // â ��� ����
		PFD_SUPPORT_OPENGL | // OpenGL ����
		PFD_DOUBLEBUFFER |
		PFD_TYPE_RGBA, // RGBA ���� ���
		32, // 32��Ʈ ���� ���
		0, 0, 0, 0, 0, 0, // ���� ��Ʈ���� ����
		0, // ���� ���� ����
		0, // ����Ʈ ��Ʈ ����
		0, // ���� ���� ����
		0, // ���� ��Ʈ ����
		16, // z ���۴� 16 ��Ʈ
		0, // ���ٽ� ���� ����
		0, // ���� ���� ����
		PFD_MAIN_PLANE, // ���� ����� ���
		0, // �����
		0, 0, 0 // ���̾� ����ũ���� ����
	};

	if (!(hDC = GetDC(hWnd))) // ��ġ ���ؽ�Ʈ�� ������� Ȯ��
	{
		KillGLWindow(); // ���÷��̸� ����
		MessageBox(NULL, TEXT("GL ��ġ ���ؽ�Ʈ�� ���� �� ����"), TEXT("����"), MB_OK | MB_ICONEXCLAMATION);
		return FALSE;
	}

	if (!(PixelFormat = ChoosePixelFormat(hDC, &pfd))) // ��ġ�ϴ� �ȼ� ������ ã�Ҵ��� Ȯ��
	{
		KillGLWindow(); // ���÷��̸� ����
		MessageBox(NULL, TEXT("������ pixelFormat�� ã�� �� ����"), TEXT("����"), MB_OK | MB_ICONEXCLAMATION);
		return FALSE;
	}

	if (!SetPixelFormat(hDC, PixelFormat, &pfd)) // �ȼ� ������ ������ �� �ִ� �� Ȯ��
	{
		KillGLWindow(); // ���÷��̸� ����
		MessageBox(NULL, TEXT("PixelFormat�� ������ �� �����ϴ�."), TEXT("����"), MB_OK | MB_ICONEXCLAMATION);
		return FALSE;
	}

	if (!(hRC = wglCreateContext(hDC))) // ������ ���ؽ�Ʈ�� ������ �� �ִ��� Ȯ��
	{
		KillGLWindow(); // ���÷��̸� ����
		MessageBox(NULL, TEXT("GL ������ ���ؽ��� ������ �� �����ϴ�."), TEXT("����"), MB_OK | MB_ICONEXCLAMATION);
		return FALSE;
	}

	if (!wglMakeCurrent(hDC, hRC)) // ������ ���ؽ�Ʈ�� Ȱ��ȭ�Ϸ��� �õ�
	{
		KillGLWindow(); // ���÷��̸� ����
		MessageBox(NULL, TEXT("GL ������ ���ؽ��� Ȱ��ȭ�� �� �����ϴ�."), TEXT("����"), MB_OK | MB_ICONEXCLAMATION);
		return FALSE;
	}

	ShowWindow(hWnd, SW_SHOW); // â�� �����ش�
	SetForegroundWindow(hWnd); // �ణ ���� �켱��
	SetFocus(hWnd); // Ű���� ��Ŀ���� ���� �����쿡 �����
	ReSizeGLScene(width, height); // ���ٰ� �ִ� GLȭ���� �����Ѵ�

	if (!InitGL()) // ���� ������ GLâ�� �ʱ�ȭ�Ѵ�
	{
		KillGLWindow(); // ���÷��̸� ����
		MessageBox(NULL, TEXT("�ʱ�ȭ ����"), TEXT("����"), MB_OK | MB_ICONEXCLAMATION);
		return FALSE;
	}

	hMenu = GetMenu(hWnd);

	return TRUE; // ����
}

// ������ ���ν���(�̺�Ʈ ó�� �Լ�)
LRESULT CALLBACK WndProc(
	HWND hwnd, // �� â�� �ڵ�
	UINT message, // �� â�� �޽���
	WPARAM wParam,  // �߰� �޽��� ����
	LPARAM lParam // �߰� �޽��� ����
	)
{
	PAINTSTRUCT paintStruct;
	static HGLRC hRC; // ������ ����
	static HDC hDC;	// ��ġ ����
	LPCWSTR string = TEXT("Hello, world"); // ����� �ؽ�Ʈ

	switch (message)
	{
		case WM_ACTIVATE: // ������ Ȱ��ȭ��
		{
			if (!HIWORD(wParam)) // �ּ�ȭ ���¸� üũ
			{
				active = TRUE; // ���α׷��� Ȱ��ȭ ����
			}
			else
			{
				active = FALSE; // ���α׷��� Ȱ��ȭ ���°� �ƴ�
			}
			return 0;
		}
		case WM_SYSCOMMAND: // �ý��� ��ɾ ����è
		{
			switch (wParam) // �ý��� ȣ���� üũ�Ѵ�
			{
				case SC_SCREENSAVE: // ȭ�� ��ȣ�Ⱑ �����Ϸ��� �� ���
				case SC_MONITORPOWER: // ����Ͱ� ������巹 ������ �� ���
					return 0; // �� ��츦 ��� �߻���Ű�� �ʴ´�
			}
			break;
		}
		case WM_CLOSE: // �����찡 ������ ����
		{
			KillFont();
			free(imageData); // �̹��� ���� ���� ����
			free(imagebuffer); // �̹��� ���� ����

			PostQuitMessage(0); // ���� �޽����� ����
			return 0;
		}
		case WM_KEYDOWN: // Ű�� ������ �ִ� ���
		{
			keys[wParam] = TRUE;
			KeyboardInputManager(wParam);
			return 0;
		}
		case WM_COMMAND:
		{
			MenuManager(wParam,lParam);
			return 0;
		}
		case WM_KEYUP: // Ű�� ���̻� ������ �ʴ� ���
		{
			keys[wParam] = FALSE;
			return 0;
		}
		case WM_SIZE: // OpenGL �������� ũ�Ⱑ ���ϴ� ���
		{
			ReSizeGLScene(LOWORD(lParam), HIWORD(lParam)); // LoWord = �ʺ�, HiWord = ����
			return 0;
		}
		case WM_LBUTTONDOWN: // ���콺 ���� ��ư ����
		{
			if (!mouseButtonDown_L)
			{
				if (mode == MODE_TRIANGLE && triangle.count == 1)
					triangle.PlusCount();

				if (mode == MODE_CHAR)
				{
					preview = true;
				}

				mouseStartPos.x = LOWORD(lParam);
				mouseStartPos.y = windowHeight - HIWORD(lParam);

				mouseButtonDown_L = TRUE;
				mouseButtonUp_L = FALSE;
			}
		
			return 0;
		}
		case WM_RBUTTONDOWN: // ���콺 ������ ��ư ����
		{
			if (!mouseButtonDown_R)
			{
				if (mode == MODE_TRIANGLE && triangle.count == 1)
					triangle.PlusCount();

				mouseStartPos.x = LOWORD(lParam);
				mouseStartPos.y = windowHeight - HIWORD(lParam);

				mouseButtonDown_R = TRUE;
				mouseButtonUp_R = FALSE;
			}
			
			return 0;
		}
		case WM_MOUSEMOVE: // ���콺 ������
		{
			mouseLastPos.x = LOWORD(lParam);
			mouseLastPos.y = windowHeight - HIWORD(lParam);

			if (mouseButtonDown_L)
			{
				draging_L = TRUE;
			}
			else if (mouseButtonDown_R)
			{
				draging_R = TRUE;
			}

			if (mode == MODE_TRIANGLE && triangle.count == 0)
				triangle.count = 0;

			return 0;
		}
		case WM_LBUTTONUP: // ���콺 ���� ��ư ��
		{
			if (mouseButtonDown_L)
			{
				if (mode == MODE_TRIANGLE && triangle.count == 0 && draging_L)
					triangle.PlusCount();

				mouseButtonDown_L = FALSE;
				mouseButtonUp_L = TRUE;
				draging_L = FALSE;
				preview = true;
			}

			if ((mode == MODE_LINE || mode == MODE_CIRCLE || mode == MODE_RECT || mode == MODE_HEART || mode == MODE_STAR) && !draging_L)
			{
				preview = false;
			}
			else if (mode == MODE_TRIANGLE)
			{
				if (triangle.count == 2)
					preview = false;
				else
					preview = true;
			}

			return 0;
		}
		case WM_RBUTTONUP: // ���콺 ������ ��ư ��
		{
			if (mouseButtonDown_R)
			{
				if (mode == MODE_TRIANGLE && triangle.count == 0 && draging_R)
					triangle.PlusCount();

				mouseButtonDown_R = FALSE;
				mouseButtonUp_R = TRUE;
				draging_R = FALSE;
				preview = true;
			}

			if ((mode == MODE_LINE || mode == MODE_CIRCLE || mode == MODE_RECT || mode == MODE_HEART || mode == MODE_STAR)  && !draging_R)
			{
				preview = false;
			}
			else if (mode == MODE_TRIANGLE)
			{
				if (triangle.count == 2)
					preview = false;
				else
					preview = true;
			}


			return 0;
		}
		case WM_MOUSEWHEEL:
		{
			((short)HIWORD(wParam)<0) ? objectSize-= 0.6f : objectSize+= 0.6f;

			if (objectSize > maxObjectSize)
				objectSize = maxObjectSize;
			if (objectSize < minObjectSize)
				objectSize = minObjectSize;

			return 0;
		}
		return DefWindowProc(hWnd, message, wParam, lParam); // ó������ ���� �޽����� DefWindowProc�� �����Ѵ�.
	}

	return (DefWindowProc(hwnd, message,wParam, lParam));
}

int WINAPI WinMain(
	HINSTANCE hInstance, // �ν��Ͻ�
	HINSTANCE hPrevInstance, // ���� �ν��Ͻ�
	LPSTR lpCmdLine, // ����� �Ű�����
	int nShowCmd // ������ �����ֱ� ����
	)
{
	MSG msg; // �޽���
	bool done = FALSE; // ���� ���α׷��� �Ϸ�Ǿ������� ����

	if (MessageBox(NULL, TEXT("��üȭ�� ��忡�� �����Ͻðڽ��ϱ�?"), TEXT("��üȭ�� ��忡�� ����?"), MB_YESNO | MB_ICONQUESTION) == IDNO)
	{
		fullscreen = FALSE;                        // Windowed Mode
	}

	if (!CreateGLWindow(PROGRAMNAME, windowWidth, windowHeight + 20, 16, fullscreen))
	{
		return 0;
	}

	//// ���� �޽��� ����
	while (!done)
	{
		if (PeekMessage(&msg, 0, NULL, NULL, PM_REMOVE))
		{
			if (msg.message == WM_QUIT) // WM_QUIT �޽����̸�
			{
				done = true; // �������α׷� ����
			}
			else
			{
				TranslateMessage(&msg); // �޽����� �ؼ��ϰ� �ٽ� ����
				DispatchMessage(&msg);
			}
		}
		else // �޽����� ���� ���
		{
			// ����� �׸���. ESCŰ�� DrawGLScene()���κ����� ���� �޽����� ���캻��.
			if (active) // ���α׷��� ���� ��?
			{
				if (keys[VK_ESCAPE]) // ESCŰ�� �����°�?
				{
					done = TRUE;
				}
				else
				{
					DrawGLScene();
				}
			}
		}
	}

	//while (GetMessage(&msg, nullptr, 0, 0))
	//{
	//	TranslateMessage(&msg);
	//	DispatchMessage(&msg);
	//	DrawGLScene();
	//	if (keys[VK_ESCAPE]) // ESCŰ�� �����°�?
	//	{
	//		done = TRUE;
	//	}
	//	else
	//	{
	//		DrawGLScene();
	//	}
	//}

	KillGLWindow(); // �����츦 ����
	return msg.wParam; // ���α׷� ����
}