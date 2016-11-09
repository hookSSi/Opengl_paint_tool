#define WIN32_LEAN_AND_MEAN // 불필요한 윈도우즈 기능들 제외

// 윈도우 헤더파일
#include<Windows.h> 
// C++ 라이브러리
#include<iostream>
// OpenGL 헤더파일
#include <GL/glut.h> 
// OpenGL 유틸리티
#include<gl/glu.h> 
#include<gl/glut.h>
// OpenGL 보조 함수들
#include<gl/glaux.h>

#include "DrawingCircle.h"
#include "Debug.h"

HGLRC hRC = NULL; // 랜더링 컨텍스트
HDC hDC = NULL; // GDI 장치 컨텍스트
HWND hWnd = NULL; // 윈도우 핸들
HINSTANCE hInstance; // 응용프로그램 인스턴스

bool keys[256]; // 키보드 루틴에 사용하는 배열
bool active = TRUE; // 윈도우 활성화 플래그, 디폴트값은 TRUE
bool fullscreen = TRUE; // 전체화면 플래그, 디폴트값은 TRUE

Circle circle(20); // 원 클래스 인스턴스
int mode = 1; // 모드
float time = 0; // 걸린시간
wchar_t buffer[256]; // 문자열 버퍼

LRESULT CALLBACK WndProc(
	HWND hwnd, // 이 창의 핸들 
	UINT message, // 이 창의 메시지 
	WPARAM wParam,  // 추가 메시지 정보
	LPARAM lParam // 추가 메시지 정보
	);

GLvoid ReSizeGLScene(GLsizei width, GLsizei height) // GL 윈도우를 초기화하고 크기를 조정한다.
{
	if (height == 0) // 0으로 나누기 방지
	{
		height = 1;
	}

	glViewport(0, 0, width, height); // 현 뷰포트를 리셋

	glMatrixMode(GL_PROJECTION); // 투영 행렬을 선택
	glLoadIdentity(); // 투영행렬을 리셋한다

	gluPerspective(45.0f, (GLfloat)width / (GLfloat)height, 0.1f, 100.0f); // 창의 비율을 계산

	glMatrixMode(GL_MODELVIEW); // 모델뷰 행렬을 선택
	glLoadIdentity(); // 모델뷰 행렬을 리셋한다
}

int InitGL(GLvoid)
{
	glShadeModel(GL_SMOOTH); // 부드러운 쉐이딩을 설정한다

	glClearColor(1.0f, 1.0f, 1.0f, 1.0f); // 검정색 배경

	glClearDepth(1.0f); // 깊이버퍼 설정
	glEnable(GL_DEPTH_TEST); // 깊이테스트를 킴
	glDepthFunc(GL_LEQUAL); // 수행할 깊이테스트의 종류

	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST); // 정말로 근사한 원근 계산

	return TRUE; // 초기화가 무사히 끝났음
}

int DrawGLScene(GLvoid) // 모든 드로잉을 처리하는 곳
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // 화면과 깊이버퍼를 비움
	/*
	여기에 드로잉 코드를 넣는 걸로...
	*/
	
	time = circle.Draw(0, 0, mode);

	glLoadIdentity(); // 현재 모델뷰 행렬을 리셋
	return TRUE; // 무사히 마침
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
	windowClass.lpszMenuName = NULL; // 사용할 메뉴
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
		ShowCursor(FALSE); // 마우스 포인터를 숨김
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
		PFD_DOUBLEBUFFER | // 더블 버퍼링 지원
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
			PostQuitMessage(0); // 종료 메시지를 보냄
			return 0;
		}
		case WM_KEYDOWN: // 키가 눌리고 있는 경우
		{
			keys[wParam] = TRUE;
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

		return DefWindowProc(hWnd, message, wParam, lParam); // 처리되지 않은 메시지는 DefWindowProc로 전달한다.
	}

	return (DefWindowProc(hwnd, message,wParam, lParam));
}

BOOL KeyboardInputManager()
{
	if (keys[VK_F1])                    // F1이 눌렸는지 검사
	{
		keys[VK_F1] = FALSE;                // 그렇다면 FALSE로 설정
		KillGLWindow();                    // 현재 창을 죽인다.
		fullscreen = !fullscreen;                // 전체화면/창모드 전환
		// OpenGL 창을 다시 만든다
		if (!CreateGLWindow(TEXT("OpenGL Framework"), 640, 480, 16, fullscreen))
		{
			return 0;                // 창이 만들어지지 않았다면 종료한다
		}
	}
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

	if (!CreateGLWindow(TEXT("OpenGL Framework"), 640, 480, 16, fullscreen))
	{
		return 0;
	}

	// 메인 메시지 루프
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
				else if (keys['1'] || keys['2'] || keys['3'])// 화면 업데이트
				{
					if (keys['1'])
					{
						mode = 0;
					}
					else if (keys['2'])
					{
						mode = 1;
					}
					else if (keys['3'])
					{
						mode = 2;
					}

					DrawGLScene(); // 장면을 그린다
					SwapBuffers(hDC); // 버퍼를 스와핑한다 (더블 버퍼링)

					swprintf_s(buffer, 256, L"%f초", time);
					MessageBox(NULL, buffer, TEXT("수행시간 확인"), MB_OK);
					keys['1'] = false; keys['2'] = false; keys['3'] = false;
				}

				KeyboardInputManager();
			}
		}	
	}

	KillGLWindow(); // 윈도우를 죽임
	return msg.wParam; // 프로그램 종료
}