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
#include<gl/glut.h>
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

const LPCWSTR PROGRAMNAME = TEXT("PaintHook"); // ���α׷� �̸�

HGLRC hRC = NULL; // ������ ���ؽ�Ʈ
HDC hDC = NULL; // GDI ��ġ ���ؽ�Ʈ
HWND hWnd = NULL; // ������ �ڵ�
HINSTANCE hInstance; // �������α׷� �ν��Ͻ�

bool keys[256]; // Ű���� ��ƾ�� ����ϴ� �迭
bool active = TRUE; // ������ Ȱ��ȭ �÷���, ����Ʈ���� TRUE
bool fullscreen = TRUE; // ��üȭ�� �÷���, ����Ʈ���� TRUE


BITMAPINFOHEADER bitmapInfoHeader; // BMP Info Header

const int MAX_PATH_LENGTH = 40; // ���� ��� �ִ� ����

static wchar_t lpstrFile1[MAX_PATH_LENGTH]; // ���� ��� ���ڿ� ����
static wchar_t lpstrFile2[MAX_PATH_LENGTH];

unsigned char* imagebuffer; // �̹��� �а� ���� ����
bool isOpen = false;
unsigned char* imageData; // �̹��� ����

static int pixelWidth = 640;
static int pixelHeight = 460;
static int windowWidth = 640; // ������ �ʺ�
static int windowHeight = 480; // ������ ����


/* ���콺 �Է� ���� ����*/

static Vector2 tempPos(320, 240);
static Vector2 mouseStartPos(320,240);
static Vector2 mouseLastPos(320,240);
static const Vector2 CENTER(320, 240);

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

const int MODE_POINT = 0; // ��
const int MODE_LINE = 1; // ��
const int MODE_CIRCLE = 2; // ��
const int MODE_RECT = 3; // �簢��
const int MODE_TRIANGLE = 4; // �ﰢ��
const int MODE_ERASER = 5; // ���찳

static int mode = 0;
static bool fill = false;
static float objectSize = 5;

static Drawing::Point point;
static Drawing::Line line;

/* ������ ���� */
CHOOSECOLOR colorChooser;
static COLORREF arrayCustomColor[16];
static DWORD rgbCurrent;

const static Color WHITE = Color(1, 1, 1);
const static Color BLACK = Color(0, 0, 0);

static Color colorMode[2] = { BLACK,WHITE };


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

int InitGL(GLvoid)
{
	ReSizeGLScene(windowWidth, windowHeight);
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f); // ���� ����

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

	DrawingManager();

	if (change)
	{
		WriteImageDatas();
		change = false;
	}
	else if (preview)
	{
		PreviewManager();
		SwapBuffers(hDC);
	}

	SwapBuffers(hDC);

	DrawingManager();

	SwapBuffers(hDC);

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
		}
		break;
		case MODE_LINE:
		{
			ObjectManager(point, colorMode[0], 1); // ������Ʈ ����			

			if (mouseButtonDown_L)
			{
				ObjectManager(line, colorMode[0], objectSize); // ������Ʈ ����

				line.transform.position = (Vector3)mouseStartPos;
				tempPos = mouseLastPos;
				line.Draw(mouseLastPos.x, mouseLastPos.y);
			}
			else if (mouseButtonDown_R)
			{
				ObjectManager(line, colorMode[1], objectSize); // ������Ʈ ����

				line.transform.position = (Vector3)mouseStartPos;
				tempPos = mouseLastPos;
				line.Draw(mouseLastPos.x, mouseLastPos.y);
			}
			else
			{
				point.transform.position = (Vector3)mouseLastPos;
				point.Draw();
			}
		}
		break;
		case MODE_ERASER:
		{
			ObjectManager(point, WHITE, objectSize);

			point.transform.position = (Vector3)mouseLastPos;
			tempPos = mouseLastPos;
			point.Draw();
		}
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
		if (mouseButtonDown_L)
		{
			ObjectManager(point, colorMode[0], objectSize);
			point.transform.position = (Vector3)mouseLastPos;
			point.Draw();
		}
		else if (mouseButtonDown_R)
		{
			ObjectManager(point, colorMode[1], objectSize);
			point.transform.position = (Vector3)mouseLastPos;
			point.Draw();
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
		}
	}
	break;
	/* �� */
	case MODE_CIRCLE:
	{

	}
	break;
	/* ���찳 */
	case MODE_ERASER:
	{
		ObjectManager(point, WHITE, objectSize);

		if (mouseButtonDown_L || mouseButtonDown_R)
		{
			point.transform.position = (Vector3)mouseLastPos;
			point.Draw();
		}
	}
	break;
	}
}

void MenuManager(WPARAM &wParam, LPARAM &lParam)
{
	switch (LOWORD(wParam))
	{
		/* ���� */
	case MENU1_NEWFILE:
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
		break;
		/* �� */
	case ID_LINE:
		mode = MODE_LINE;
		break;
		/* ���� */
	case ID_CIRCLE_COORD:
		mode = MODE_CIRCLE;
		break;
		/* ���찳 */
	case ID_ERASER:
		mode = MODE_ERASER;
		break;
		/* �� */
	case ID_COLOR_BOX:
	{
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
	}
	break;
	/* ��ü ����� */
	case ID_ALL_CLEAR:
	{
		glClear(GL_COLOR_BUFFER_BIT);
		WriteImageDatas();
	}
		break;
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
			PostQuitMessage(0); // ���� �޽����� ����
			return 0;
		}
		case WM_KEYDOWN: // Ű�� ������ �ִ� ���
		{
			keys[wParam] = TRUE;

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
			mouseStartPos.x = LOWORD(lParam);
			mouseStartPos.y = windowHeight - HIWORD(lParam);
			
			mouseButtonDown_L = TRUE;
			return 0;
		}
		case WM_RBUTTONDOWN: // ���콺 ������ ��ư ����
		{
			mouseStartPos.x = LOWORD(lParam);
			mouseStartPos.y = windowHeight - HIWORD(lParam);

			mouseButtonDown_R = TRUE;
			return 0;
		}
		case WM_MOUSEMOVE: // ���콺 ������
		{
			mouseLastPos.x = LOWORD(lParam);
			mouseLastPos.y = windowHeight - HIWORD(lParam);

			if (mode == MODE_POINT)
			{
				if (mouseButtonDown_L) // ���콺 ��ư ����
				{
					preview = false;
					draging_L = TRUE;
				}
				else if (mouseButtonDown_R)
				{
					preview = false;
					draging_R = TRUE;
				}
				else
				{
					preview = true;
				}
			}
			else if (mode == MODE_LINE)
			{
				preview = true;
			}
			else if (mode == MODE_ERASER)
			{
				if (mouseButtonDown_L) // ���콺 ��ư ����
				{
					preview = false;
					draging_L = TRUE;
				}
				else if (mouseButtonDown_R)
				{
					preview = false;
					draging_R = TRUE;
				}
				else
				{
					preview = true;
				}
			}

			return 0;
		}
		case WM_LBUTTONUP: // ���콺 ���� ��ư ��
		{
			mouseLastPos.x = LOWORD(lParam);
			mouseLastPos.y = windowHeight - HIWORD(lParam);

			mouseStartPos.x = LOWORD(lParam);
			mouseStartPos.y = windowHeight - HIWORD(lParam);

			mouseButtonDown_L = FALSE;
			draging_L = FALSE;

			if (mode == MODE_POINT)
			{
				preview = false;
			}
			else if (mode == MODE_LINE)
			{
				preview = false;
			}
			else if (mode == MODE_ERASER)
			{
				preview = false;
			}

			if (!preview)
				change = TRUE;

			return 0;
		}
		case WM_RBUTTONUP: // ���콺 ������ ��ư ��
		{
			mouseLastPos.x = LOWORD(lParam);
			mouseLastPos.y = windowHeight - HIWORD(lParam);

			mouseStartPos.x = LOWORD(lParam);
			mouseStartPos.y = windowHeight - HIWORD(lParam);

			mouseButtonDown_R = FALSE;
			draging_R = FALSE;


			if (mode == MODE_POINT)
			{
				preview = false;
			}
			else if (mode == MODE_LINE)
			{
				preview = false;
			}
			else if (mode == MODE_ERASER)
			{
				preview = false;
			}

			if (!preview)
				change = TRUE;
			return 0;
		}
		case WM_MOUSEWHEEL:
		{
			((short)HIWORD(wParam)<0) ? objectSize-= 0.3f : objectSize+= 0.3f;

			if (objectSize > 10)
				objectSize = 10;
			if (objectSize < 1)
				objectSize = 1;

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

	if (!CreateGLWindow(PROGRAMNAME, 640, 480, 16, fullscreen))
	{
		return 0;
	}

	// ���� �޽��� ����
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

				if (keys[VK_F1])                    // F1�� ���ȴ��� �˻�
				{
					keys[VK_F1] = FALSE;                // �׷��ٸ� FALSE�� ����
					KillGLWindow();                    // ���� â�� ���δ�.
					fullscreen = !fullscreen;                // ��üȭ��/â��� ��ȯ
															 // OpenGL â�� �ٽ� �����
					if (!CreateGLWindow(PROGRAMNAME, 640, 480, 16, fullscreen))
					{
						return 0;                // â�� ��������� �ʾҴٸ� �����Ѵ�
					}
				}

				if (keys[VK_SPACE])
				{
					keys[VK_SPACE] = FALSE;
				}
			}
		}
	}
	free(imageData);
	KillGLWindow(); // �����츦 ����
	return msg.wParam; // ���α׷� ����
}