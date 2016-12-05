#define WIN32_LEAN_AND_MEAN // 불필요한 윈도우즈 기능들 제외
#define _CRT_SECURE_NO_WARNINGS 

// 윈도우 헤더파일
#include<Windows.h>
// 리소스 파일
#include "resource.h"
// C++ 라이브러리
#include<iostream>
// OpenGL 헤더파일
#include <GL/glut.h>
// OpenGL 유틸리티
#include<gl/glu.h>
// OpenGL 보조 함수들
#include<gl/glaux.h>
// stirng 헤더
#include<string>
#include <commctrl.h>

#include "FileHandle.h"
#include "Layer.h"
#include "Debug.h"
#include "Drawing.h"
#include "Util.h"
#include "Transformation.h"

const LPCWSTR PROGRAMNAME = TEXT("PaintHook"); // 프로그램 이름

HGLRC hRC = NULL; // 랜더링 컨텍스트
HDC hDC = NULL; // GDI 장치 컨텍스트
HWND hWnd = NULL; // 윈도우 핸들
HINSTANCE hInstance; // 응용프로그램 인스턴스
HMENU hMenu = NULL; // 메뉴 인스턴스

bool keys[256]; // 키보드 루틴에 사용하는 배열
bool active = TRUE; // 윈도우 활성화 플래그, 디폴트값은 TRUE
bool fullscreen = TRUE; // 전체화면 플래그, 디폴트값은 TRUE


BITMAPINFOHEADER bitmapInfoHeader; // BMP Info Header

const int MAX_PATH_LENGTH = 40; // 파일 경로 최대 길이

static wchar_t lpstrFile1[MAX_PATH_LENGTH]; // 파일 경로 문자열 버퍼
static wchar_t lpstrFile2[MAX_PATH_LENGTH];

unsigned char* imagebuffer; // 이미지 읽고 쓰기 버퍼
unsigned char* imageData; // 이미지 파일
bool isOpen = false;

static int windowWidth = 640; // 윈도우 너비
static int windowHeight = 480; // 윈도우 높이

static int pixelWidth = windowWidth;
static int pixelHeight = windowHeight;



/* 마우스 입력 관려 변수*/

static const Vector2 CENTER(windowWidth / 2, windowHeight / 2);

static Vector2 tempPos(CENTER);
static Vector2 mouseStartPos(CENTER);
static Vector2 mouseLastPos(CENTER);


static bool mouseButtonDown_L = false; // 왼쪽 버튼 UP
static bool mouseButtonDown_R = false; // 오른쪽 버튼 UP
static bool mouseButtonUp_L = true; // 왼쪽 버튼 DOWN
static bool mouseButtonUp_R = true; // 오른쪽 버튼 DOWN
static bool draging_R = false; // 드래그중?
static bool draging_L = false;
static bool preview = true; // 미리보기 활성화?
static bool change = true; // 이미지 변화?


OPENFILENAME OFN; // 열기
OPENFILENAME SFN; // 저장

LRESULT CALLBACK WndProc(
	HWND hwnd, // 이 창의 핸들
	UINT message, // 이 창의 메시지
	WPARAM wParam,  // 추가 메시지 정보
	LPARAM lParam // 추가 메시지 정보
	);

int DrawGLScene(GLvoid); // 출력 함수

void ReadImageDatas(); // 이미지 파일 읽기
void WriteImageDatas(); // 이미지 파일 쓰기
void ObjectManager(Object &object, Color color, float size); // 오브젝트 설정 함수
void DrawingManager(); // 그리기 함수
void PreviewManager(); // 미리보기 함수
void DrawingRoutine(); // 그리기 루틴


const int maxObjectSize = 30;
const int minObjectSize = 1;

const int MODE_POINT = 0; // 점
const int MODE_LINE = 1; // 선
const int MODE_CIRCLE = 2; // 원
const int MODE_RECT = 3; // 사각형
const int MODE_TRIANGLE = 4; // 삼각형
const int MODE_ERASER = 5; // 지우개
const int MODE_CHAR = 6; // 텍스트
const int MODE_HEART = 7; // 기타 도형
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
static Drawing::Line line1; // 삼각형 용 라인
static Drawing::Heart heart;
static Drawing::Star star;

//static Drawing::TEXT text;

/* 색관련 변수 */
CHOOSECOLOR colorChooser;
static COLORREF arrayCustomColor[16];
static DWORD rgbCurrent;

const static Color WHITE = Color(1, 1, 1);
const static Color BLACK = Color(0, 0, 0);

static Color colorMode[2] = { BLACK,WHITE }; // 왼쪽, 오른쪽

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

GLvoid ReSizeGLScene(GLsizei width, GLsizei height) // GL 윈도우를 초기화하고 크기를 조정한다.
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0.0, (GLdouble)width, 0.0, (GLdouble)height, -1.0, 1.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	/* adjust viewport and  clear */	

	glViewport(0, 0, width, height);
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f); // 배경색 설정
	glClear(GL_COLOR_BUFFER_BIT);
	ReadImageDatas();
	DrawGLScene();
	glFlush();

	/* set global size for use by drawing routine */

	windowWidth = width;
	windowHeight = height;

	change = true;
}

/* 문자열 출력 관련 함수 */
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
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f); // 배경색 설정

	BuildFont();

	return TRUE; // 초기화가 무사히 끝났음
}

int DrawGLScene(GLvoid) // 모든 드로잉을 처리하는 곳
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

	return TRUE; // 무사히 마침
}

void ObjectManager(Object &object, Color color, float size) // 그릴려는 오브젝트 설정
{
	object.color = color;
	object.transform.scale = Vector3(size,size,0);
}

void PreviewManager()
{
	glClear(GL_COLOR_BUFFER_BIT); // 미리보기 지우기
	ReadImageDatas(); // 이미지 불러오기

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
			ObjectManager(line, colorMode[0], objectSize); // 오브젝트 설정

			line.transform.position = (Vector3)mouseStartPos;
			line.Draw(mouseLastPos.x, mouseLastPos.y);
		}
		else if (draging_R)
		{
			ObjectManager(line, colorMode[1], objectSize); // 오브젝트 설정

			line.transform.position = (Vector3)mouseStartPos;
			line.Draw(mouseLastPos.x, mouseLastPos.y);
		}
		else
		{
			ObjectManager(point, colorMode[0], 1); // 오브젝트 설정			
			point.transform.position = (Vector3)mouseLastPos;
			point.Draw();
		}
		break;
	}
	case MODE_CIRCLE:
	{
		if (draging_L)
		{
			ObjectManager(circle, colorMode[0], objectSize); // 오브젝트 설정

			circle.transform.position = (Vector3)mouseStartPos;
			circle.Draw(mouseLastPos.x, mouseLastPos.y, fill_Circle);
		}
		else if (draging_R)
		{
			ObjectManager(circle, colorMode[1], objectSize); // 오브젝트 설정

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
			ObjectManager(rect, colorMode[0], objectSize); // 오브젝트 설정

			rect.transform.position = (Vector3)mouseStartPos;
			rect.Draw(mouseLastPos.x, mouseLastPos.y, fill_Rect);
		}
		else if (draging_R)
		{
			ObjectManager(rect, colorMode[1], objectSize); // 오브젝트 설정

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
			ObjectManager(heart, colorMode[0], objectSize); // 오브젝트 설정

			heart.transform.position = (Vector3)mouseStartPos;
			heart.Draw(mouseLastPos.x, mouseLastPos.y, fill_ETC);
		}
		else if (draging_R)
		{
			ObjectManager(heart, colorMode[1], objectSize); // 오브젝트 설정

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
			ObjectManager(star, colorMode[0], objectSize); // 오브젝트 설정

			star.transform.position = (Vector3)mouseStartPos;
			star.Draw(mouseLastPos.x, mouseLastPos.y, fill_ETC);
		}
		else if (draging_R)
		{
			ObjectManager(star, colorMode[1], objectSize); // 오브젝트 설정

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
		/* 점 */
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
	/* 선 */
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
	/* 원 */
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
	/* 사각형 */
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
	/* 삼각형 */
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
	/* 텍스트 */
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
	/* 지우개 */
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
	/* 기타 도형들 */
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

// 키보드 입력 매니저 함수
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
		if (ch == 229) // 한글 입력 제외
			return;
		text.AddChar(ch);
	}
}

// 메뉴 매니저 함수
void MenuManager(WPARAM &wParam, LPARAM &lParam)
{
	switch (LOWORD(wParam))
	{
		/* 파일 */
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
			imagebuffer = BMP::LoadBMP(Util::ConvertWCtoC(OFN.lpstrFile), &bitmapInfoHeader); // 읽어서 ImageBuffer에 저장
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
			BMP::SaveBMP(fileName.data(), pixelWidth, pixelHeight, imageData); // 이미지 파일 저장
		}
		return;
	case MENU1_EXIT:
		PostQuitMessage(0); // 종료 메시지를 보냄
		return;
		/* 점 */
	case ID_POINT:
		mode = MODE_POINT;
		return;
		/* 선 */
	case ID_LINE:
		mode = MODE_LINE;
		return;
		/* 도형 */
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
		/* 텍스트 */
	case ID_CHAR:
		mode = MODE_CHAR;
		return;
		/* 지우개 */
	case ID_ERASER:
		mode = MODE_ERASER;
		return;
		/* 색 */
	case ID_COLOR_BOX:
		ZeroMemory(&colorChooser, sizeof(colorChooser));
		colorChooser.lStructSize = sizeof(colorChooser);
		colorChooser.hwndOwner = hWnd;
		colorChooser.lpCustColors = (LPDWORD)arrayCustomColor;
		colorChooser.rgbResult = rgbCurrent;
		colorChooser.Flags = CC_FULLOPEN | CC_RGBINIT;

		if (ChooseColor(&colorChooser) == TRUE)
		{
			if(MessageBox(NULL, TEXT("컬러를 적용시킬 브러쉬를 선택하세요. \n 예 - 왼쪽 아니요 - 오른쪽") , TEXT("브러쉬 선택"), MB_YESNO | MB_ICONQUESTION) == IDYES)
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
	/* 전체 지우기 */
	case ID_ALL_CLEAR:
		glClear(GL_COLOR_BUFFER_BIT);
		WriteImageDatas();
		return;		
	/* 변환 관련 */
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

GLvoid KillGLWindow(GLvoid) // 프로그램이 종료되기 바로 직전 실행됨
{
	if (fullscreen)
	{
		ChangeDisplaySettings(NULL, 0); // 전체화면이면 데스크탑으로 돌아감
		ShowCursor(TRUE); // 마우스 포인터 표시
	}

	if (hRC)// 랜더링 컨텍스트를 가지고 있는지 검사
	{
		if (!wglMakeCurrent(NULL, NULL)) // DC와 RC 컨텍스트를 해제할 수 있는지 확인
		{
			MessageBox(NULL, TEXT("DC와 RC의 해제가 실패함"), TEXT("종료"), MB_OK | MB_ICONINFORMATION);
		}

		if (!wglDeleteContext(hRC)) // RC를 지울수 있는지 확인
		{
			MessageBox(NULL, TEXT("장치 컨텍스트 해제에 실패함"), TEXT("종료에러"), MB_OK | MB_ICONINFORMATION);
			hDC = NULL; // DC를 NULL로 만듬
		}

		if (hWnd && !DestroyWindow(hWnd)) // 창을 파괴할 수 있는지 확인
		{
			MessageBox(NULL, TEXT("hWnd 해제에 실패함"), TEXT("종료에러"), MB_OK | MB_ICONINFORMATION);
			hWnd = NULL; // hWnd를 NULL로 만듬
		}

		if (!UnregisterClass(TEXT("MyClass"), hInstance)) // 등록된 클래스를 지울수 있는지 확인
		{
			MessageBox(NULL, TEXT("윈도우즈 클래스의 등록을 해제할 수 없음"), TEXT("종료에러"), MB_OK | MB_ICONINFORMATION);
			hInstance = NULL; // hInstance를 NULL로 바꿈
		}

	}
}

BOOL CreateGLWindow(LPCWSTR title, int width, int height, int bits, bool fullScreenFlag)
{
	GLuint PixelFormat; // 일치하는 포맷을 찾은 결과를 가진다

	WNDCLASSEX windowClass; // 윈도우 클래스

	DWORD dwExStyle; // 윈도우 확장 스타일
	DWORD dwStyle; // 윈도우 스타일

	RECT windowRect; // 사각형의 좌상단과 / 우하단 값을 저장함
	windowRect.left = (long)0; // 왼쪽 값을 0으로 지정
	windowRect.right = (long)width; // 오른쪽 값을 요청된 너비로 지정
	windowRect.top = (long)0; // 위쪽 값을 0으로 지정
	windowRect.bottom = (long)height; // 아래쪽 값을 요청된 높이로 지정

	fullscreen == fullScreenFlag;

	// 윈도우 클래스 구조체를 채운다
	windowClass.cbSize = sizeof(WNDCLASSEX);
	windowClass.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC; // 윈도우 스타일
	windowClass.lpfnWndProc = (WNDPROC)WndProc; // 메시지 처리 함수
	windowClass.cbClsExtra = 0;
	windowClass.cbWndExtra = 0;
	windowClass.hInstance = hInstance; // 이 윈도우 클래스를 사용하는 프로그램 번호
	windowClass.hIcon = LoadIcon(NULL, IDI_APPLICATION); // 최소화 아이콘
	windowClass.hCursor = LoadCursor(NULL, IDC_ARROW); // 마우스 커서
	windowClass.hbrBackground = NULL; // 윈도우 배경색
	windowClass.lpszMenuName = MAKEINTRESOURCE(MENU1); // 사용할 메뉴
	windowClass.lpszClassName = TEXT("MyClass"); // 윈도우 클래스의 이름
	windowClass.hIconSm = LoadIcon(NULL, IDI_WINLOGO);

	if (!RegisterClassEx(&windowClass)) // 윈도우 클래스의 등록을 시도한
	{
		MessageBox(NULL, TEXT("윈도우 클래스를 등록하는데 실패했음"), TEXT("오류"), MB_OK | MB_ICONEXCLAMATION);
		return FALSE;
	}

	if (fullscreen)
	{
		DEVMODE dmScreenSettings;                    // 장치모드
		memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));        // 메모리를 반드시 지우자
		dmScreenSettings.dmSize = sizeof(dmScreenSettings);        // Devmode 구조체의 크기
		dmScreenSettings.dmPelsWidth = width;            // 선택된 화면너비
		dmScreenSettings.dmPelsHeight = height;            // 선택된 화면높이
		dmScreenSettings.dmBitsPerPel = bits;                // 선택된 픽셀당 비트
		dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

		if (ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN) != DISP_CHANGE_SUCCESSFUL)
		{
			if (MessageBox(NULL, TEXT("사용중인 비디오 카드가 요청하신 전체화면 모드를 지원\n하지 않습니다. 대신 창모드를 사용할까요?"), TEXT("MyOpenGL"), MB_YESNO | MB_ICONEXCLAMATION) == IDYES)
			{
				fullscreen = FALSE;
			}
			else
			{
				MessageBox(NULL, TEXT("프로그램이 종료될 것입니다"), TEXT("오류"), MB_OK | MB_ICONSTOP);
				return FALSE; // 종료 및 FALSE반환
			}
		}
	}
	if (fullscreen) // 여전히 전체화면 모드인지 확인
	{
		dwExStyle = WS_EX_APPWINDOW; // 윈도우 확장 스타일
		dwStyle = WS_POPUP; // 윈도우즈 스타일
		ShowCursor(TRUE); // 마우스 포인터를 숨김
	}
	else
	{
		dwExStyle = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE; // 윈도우 확장 스타일
		dwStyle = WS_OVERLAPPEDWINDOW; // 윈도우즈 스타일
	}

	AdjustWindowRectEx(&windowRect, dwStyle, FALSE, dwExStyle); // 요청된 크기로 윈도우 크기를 조정한다

	if (!(hWnd = CreateWindowEx(
		dwExStyle, // 본 윈도우용 확장스타일
		TEXT("MyClass"), // 클래스 이름
		title, // 윈도우 타이틀
		WS_CLIPSIBLINGS | // 필수 윈도우 스타일
		WS_CLIPCHILDREN | // 필수 윈도우 스타일
		dwStyle,  // 선택된 윈도우 스타일
		0, 0, // 창 위치
		windowRect.right - windowRect.left, // 조정된 창 너비를 계산함
		windowRect.bottom - windowRect.top, // 조정된 창 높이를 계산함
		NULL, // 부모 윈도우 없음
		NULL, // 메뉴 없음
		hInstance,  // 인스턴스
		NULL
		)))
	{
		KillGLWindow(); // 디스플레이를 리셋함
		MessageBox(NULL, TEXT("윈도우 생성 오류"), TEXT("오류"), MB_OK | MB_ICONEXCLAMATION);
		return FALSE;
	}


	static PIXELFORMATDESCRIPTOR pfd = {
		sizeof(PIXELFORMATDESCRIPTOR), // 구조체의 크기
		1, // 버전 -> 항상 1로 고정됨
		PFD_DRAW_TO_WINDOW | // 창 모드 지원
		PFD_SUPPORT_OPENGL | // OpenGL 지원
		PFD_DOUBLEBUFFER |
		PFD_TYPE_RGBA, // RGBA 색상 모드
		32, // 32비트 색상 모드
		0, 0, 0, 0, 0, 0, // 색상 비트들은 무시
		0, // 알파 버퍼 없음
		0, // 시프트 비트 무시
		0, // 누적 버퍼 없음
		0, // 누적 비트 무시
		16, // z 버퍼는 16 비트
		0, // 스텐실 버퍼 없음
		0, // 보조 버퍼 없음
		PFD_MAIN_PLANE, // 메인 드로잉 평면
		0, // 예약됨
		0, 0, 0 // 레이어 마스크들을 무시
	};

	if (!(hDC = GetDC(hWnd))) // 장치 컨텍스트를 얻었는지 확인
	{
		KillGLWindow(); // 디스플레이를 리셋
		MessageBox(NULL, TEXT("GL 장치 컨텍스트를 만들 수 없음"), TEXT("오류"), MB_OK | MB_ICONEXCLAMATION);
		return FALSE;
	}

	if (!(PixelFormat = ChoosePixelFormat(hDC, &pfd))) // 일치하는 픽셀 포맷을 찾았는지 확인
	{
		KillGLWindow(); // 디스플레이를 리셋
		MessageBox(NULL, TEXT("적절한 pixelFormat을 찾을 수 없음"), TEXT("오류"), MB_OK | MB_ICONEXCLAMATION);
		return FALSE;
	}

	if (!SetPixelFormat(hDC, PixelFormat, &pfd)) // 픽셀 포맷을 설정할 수 있는 지 확인
	{
		KillGLWindow(); // 디스플레이를 리셋
		MessageBox(NULL, TEXT("PixelFormat을 설정할 수 없습니다."), TEXT("오류"), MB_OK | MB_ICONEXCLAMATION);
		return FALSE;
	}

	if (!(hRC = wglCreateContext(hDC))) // 렌더링 컨텍스트를 가져올 수 있는지 확인
	{
		KillGLWindow(); // 디스플레이를 리셋
		MessageBox(NULL, TEXT("GL 렌더링 컨텍스를 생성할 수 없습니다."), TEXT("오류"), MB_OK | MB_ICONEXCLAMATION);
		return FALSE;
	}

	if (!wglMakeCurrent(hDC, hRC)) // 렌더링 컨텍스트를 활성화하려고 시도
	{
		KillGLWindow(); // 디스플레이를 리셋
		MessageBox(NULL, TEXT("GL 렌더링 컨텍스를 활성화할 수 없습니다."), TEXT("오류"), MB_OK | MB_ICONEXCLAMATION);
		return FALSE;
	}

	ShowWindow(hWnd, SW_SHOW); // 창을 보여준다
	SetForegroundWindow(hWnd); // 약간 높은 우선권
	SetFocus(hWnd); // 키보드 포커스를 현재 윈도우에 맞춘다
	ReSizeGLScene(width, height); // 원근감 있는 GL화면을 설정한다

	if (!InitGL()) // 새로 생성된 GL창을 초기화한다
	{
		KillGLWindow(); // 디스플레이를 리셋
		MessageBox(NULL, TEXT("초기화 실패"), TEXT("오류"), MB_OK | MB_ICONEXCLAMATION);
		return FALSE;
	}

	hMenu = GetMenu(hWnd);

	return TRUE; // 성공
}

// 윈도우 프로시저(이벤트 처리 함수)
LRESULT CALLBACK WndProc(
	HWND hwnd, // 이 창의 핸들
	UINT message, // 이 창의 메시지
	WPARAM wParam,  // 추가 메시지 정보
	LPARAM lParam // 추가 메시지 정보
	)
{
	PAINTSTRUCT paintStruct;
	static HGLRC hRC; // 랜더링 문맥
	static HDC hDC;	// 장치 문맥
	LPCWSTR string = TEXT("Hello, world"); // 출력할 텍스트

	switch (message)
	{
		case WM_ACTIVATE: // 윈도우 활성화중
		{
			if (!HIWORD(wParam)) // 최소화 상태를 체크
			{
				active = TRUE; // 프로그램이 활성화 상태
			}
			else
			{
				active = FALSE; // 프로그램이 활성화 상태가 아님
			}
			return 0;
		}
		case WM_SYSCOMMAND: // 시스템 명령어를 가로챔
		{
			switch (wParam) // 시스템 호출을 체크한다
			{
				case SC_SCREENSAVE: // 화면 보호기가 시작하려고 할 경우
				case SC_MONITORPOWER: // 모니터가 절전모드레 들어가려고 할 경우
					return 0; // 두 경우를 모두 발생시키지 않는다
			}
			break;
		}
		case WM_CLOSE: // 윈도우가 닫히는 중임
		{
			KillFont();
			free(imageData); // 이미지 파일 버퍼 지움
			free(imagebuffer); // 이미지 버퍼 지움

			PostQuitMessage(0); // 종료 메시지를 보냄
			return 0;
		}
		case WM_KEYDOWN: // 키가 눌리고 있는 경우
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
		case WM_KEYUP: // 키가 더이상 눌리지 않는 경우
		{
			keys[wParam] = FALSE;
			return 0;
		}
		case WM_SIZE: // OpenGL 윈도우의 크기가 변하는 경우
		{
			ReSizeGLScene(LOWORD(lParam), HIWORD(lParam)); // LoWord = 너비, HiWord = 높이
			return 0;
		}
		case WM_LBUTTONDOWN: // 마우스 왼쪽 버튼 누름
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
		case WM_RBUTTONDOWN: // 마우스 오른쪽 버튼 누름
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
		case WM_MOUSEMOVE: // 마우스 움직임
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
		case WM_LBUTTONUP: // 마우스 왼쪽 버튼 뗌
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
		case WM_RBUTTONUP: // 마우스 오른쪽 버튼 뗌
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
		return DefWindowProc(hWnd, message, wParam, lParam); // 처리되지 않은 메시지는 DefWindowProc로 전달한다.
	}

	return (DefWindowProc(hwnd, message,wParam, lParam));
}

int WINAPI WinMain(
	HINSTANCE hInstance, // 인스턴스
	HINSTANCE hPrevInstance, // 이전 인스턴스
	LPSTR lpCmdLine, // 명령행 매개변수
	int nShowCmd // 윈도우 보여주기 상태
	)
{
	MSG msg; // 메시지
	bool done = FALSE; // 응용 프로그램이 완료되었는지의 여부

	if (MessageBox(NULL, TEXT("전체화면 모드에서 실행하시겠습니까?"), TEXT("전체화면 모드에서 시작?"), MB_YESNO | MB_ICONQUESTION) == IDNO)
	{
		fullscreen = FALSE;                        // Windowed Mode
	}

	if (!CreateGLWindow(PROGRAMNAME, windowWidth, windowHeight + 20, 16, fullscreen))
	{
		return 0;
	}

	//// 메인 메시지 루프
	while (!done)
	{
		if (PeekMessage(&msg, 0, NULL, NULL, PM_REMOVE))
		{
			if (msg.message == WM_QUIT) // WM_QUIT 메시지이면
			{
				done = true; // 응용프로그램 종료
			}
			else
			{
				TranslateMessage(&msg); // 메시지를 해석하고 다시 전달
				DispatchMessage(&msg);
			}
		}
		else // 메시지가 없는 경우
		{
			// 장면을 그린다. ESC키와 DrawGLScene()으로부터의 종료 메시지를 살펴본다.
			if (active) // 프로그램이 동작 중?
			{
				if (keys[VK_ESCAPE]) // ESC키를 눌렀는가?
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
	//	if (keys[VK_ESCAPE]) // ESC키를 눌렀는가?
	//	{
	//		done = TRUE;
	//	}
	//	else
	//	{
	//		DrawGLScene();
	//	}
	//}

	KillGLWindow(); // 윈도우를 죽임
	return msg.wParam; // 프로그램 종료
}