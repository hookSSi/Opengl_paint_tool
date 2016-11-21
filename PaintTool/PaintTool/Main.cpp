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

#include "FileHandle.h"
#include "Layer.h"
#include "Debug.h"
#include "Drawing.h"

const LPCWSTR PROGRAMNAME = TEXT("PaintHook"); // ���α׷� �̸�

HGLRC hRC = NULL; // ������ ���ؽ�Ʈ
HDC hDC = NULL; // GDI ��ġ ���ؽ�Ʈ
HWND hWnd = NULL; // ������ �ڵ�
HINSTANCE hInstance; // �������α׷� �ν��Ͻ�

bool keys[256]; // Ű���� ��ƾ�� ����ϴ� �迭
bool active = TRUE; // ������ Ȱ��ȭ �÷���, ����Ʈ���� TRUE
bool fullscreen = TRUE; // ��üȭ�� �÷���, ����Ʈ���� TRUE

wchar_t buffer[256]; // ���ڿ� ����

LRESULT CALLBACK WndProc(
	HWND hwnd, // �� â�� �ڵ�
	UINT message, // �� â�� �޽���
	WPARAM wParam,  // �߰� �޽��� ����
	LPARAM lParam // �߰� �޽��� ����
	);

GLvoid ReSizeGLScene(GLsizei width, GLsizei height) // GL �����츦 �ʱ�ȭ�ϰ� ũ�⸦ �����Ѵ�.
{
	if (height == 0) // 0���� ������ ����
	{
		height = 1;
	}

	glViewport(0, 0, width, height); // ���� ����Ʈ�� ����

	glMatrixMode(GL_PROJECTION); // ���� ����� ����
	glLoadIdentity(); // ��������� �����Ѵ�

	gluPerspective(45.0f, (GLfloat)width / (GLfloat)height, 0.1f, 100.0f); // â�� ������ ���

	glMatrixMode(GL_MODELVIEW); // �𵨺� ����� ����
	glLoadIdentity(); // �𵨺� ����� �����Ѵ�
}

int InitGL(GLvoid)
{
	glShadeModel(GL_SMOOTH); // �ε巯�� ���̵��� �����Ѵ�

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f); // ���� ����

	glClearDepth(1.0f); // ���̹��� ����
	glEnable(GL_DEPTH_TEST); // �����׽�Ʈ�� Ŵ
	glDepthFunc(GL_LEQUAL); // ������ �����׽�Ʈ�� ����

	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST); // ������ �ٻ��� ���� ���

	return TRUE; // �ʱ�ȭ�� ������ ������
}

int DrawGLScene(GLvoid) // ��� ������� ó���ϴ� ��
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // ȭ��� ���̹��۸� ���
	glLoadIdentity();
	
	gluLookAt(0.0, 0.0, 10.0,
		0.0, 0.0, 0.0,
		0.0, 1.0, 0.0);

	/*
	���⿡ ����� �ڵ带 �ִ� �ɷ�...
	*/

	Drawing::Rectangle rectangle(Vector2(-3,-1));

	rectangle.endPoint = Vector2(5, 10);

	rectangle.color = Color(1, 1,1, 1);
	rectangle.Draw();

	//Debug::Circle();

	return TRUE; // ������ ��ħ
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
		PFD_DOUBLEBUFFER | // ���� ���۸� ����
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
		case WM_SYSCOMMAND: // �ý��� ���ɾ ����è
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
			switch (LOWORD(wParam))
			{
			case MENU1_OPEN:
				OPENFILENAME OFN;
				memset(&OFN, 0, sizeof(OPENFILENAME));
				OFN.lStructSize = sizeof(OPENFILENAME);
				OFN.hwndOwner = hWnd;
				OFN.lpstrFilter = TEXT("BMP files(*.bmp)\0*.bmp\0PNG(*.png) files\0*.png\0All Files (*.*)\0*.*\0");;
				OFN.lpstrFile = lpstrFile;
				OFN.nMaxFile = 256;
				OFN.lpstrInitialDir = TEXT("c:\\");
				if (GetOpenFileName(&OFN) != 0)
				{
					wsprintf(buffer, TEXT("%s ������ �����߽��ϴ�."), OFN.lpstrFile);
					MessageBox(hWnd, buffer, TEXT("���� ���� ����"), MB_OK);
					BMP::LoadBmp(OFN.lpstrFile, &imageBuffer);
				}
				return 0;
			case MENU1_SAVE:
				if (BMP::SaveBmp(TEXT("../ Save_image.bmp"), imageBuffer, 1920, 1080))
					Debug::Log(TEXT("����"));
				else
					Debug::Log(TEXT("���� ����"));
				return 0;
			case MENU1_SAVE_ANOTHER_NAME:
				Debug::Log(TEXT("�ٸ� �̸����� ����"));
				return 0;
			case MENU1_EXIT:
				PostQuitMessage(0); // ���� �޽����� ����
				return 0;
			}
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

		return DefWindowProc(hWnd, message, wParam, lParam); // ó������ ���� �޽����� DefWindowProc�� �����Ѵ�.
	}

	return (DefWindowProc(hwnd, message,wParam, lParam));
}

int WINAPI WinMain(
	HINSTANCE hInstance, // �ν��Ͻ�
	HINSTANCE hPrevInstance, // ���� �ν��Ͻ�
	LPSTR lpCmdLine, // ������ �Ű�����
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
					SwapBuffers(hDC);
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

	KillGLWindow(); // �����츦 ����
	return msg.wParam; // ���α׷� ����
}