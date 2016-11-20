#ifndef __FILE_HANDLE_H_
#define __FILE_HANDLE_H_

#include <stdlib.h>
#include <assert.h>
#include <windows.h>
#include <stdio.h>
#include <list>
#include <algorithm>
#include <Commdlg.h>

using namespace std;

const int MAX_PATH_LENGTH = 40; // 파일 경로 최대 길이

static wchar_t lpstrFile[MAX_PATH_LENGTH]; // 파일 경로 문자열 버퍼

typedef unsigned char byte;

GLubyte* imageBuffer = nullptr; // 이미지 버퍼
static GLuint texture[2];

namespace BMP
{
	bool LoadBmp(wchar_t* filename, GLubyte** _image);
	bool SaveBmp(wchar_t* filename, GLubyte* _image, int width, int height);

	bool LoadBmp(wchar_t* filename, GLubyte** _image)
	{
		FILE* fp;

		// 비트맵 정보 구조체
		BITMAPFILEHEADER BMFH; // BitMap File Header
		BITMAPINFOHEADER BMH; // BitMap Info Header

		fopen_s(&fp, (char*)filename, "rb");
		if (nullptr == fp)
		{
			OutputDebugStringA("Open ERROR");
			return false;
		}

		// 지정한 크기만큼 파일에서 읽어옴, 그리고 비트맵 파일 헤더에 넣어준다
		fread(&BMFH, sizeof(BITMAPFILEHEADER), 1, fp);
		if (BMFH.bfType != 0x4d42) // 비트맵 파일이 아니면 종료한다.
		{
			fclose(fp);
			OutputDebugStringA("BMP 파일이 아닙니다.");
			return false;
		}

		// 인포헤더에 있는 크기의 정보만큼 읽음
		fread(&BMH, sizeof(BITMAPINFOHEADER), 1, fp);
		
		// 24비트이고 압축 안되었는지 확인
		if (BMH.biBitCount != 24 || BMH.biCompression != BI_RGB)
		{
			fclose(fp);
			return false;
		}

		int width = BMH.biWidth;
		int height = BMH.biHeight - 1;
		int bytePerScanLine = (width * 3 + 3) & ~ 3; // 패딩
		int size = BMFH.bfSize;

		*_image = (byte*)malloc(size);

		fread(*_image, size, 1, fp); // 파일의 정보를 모두 읽어옴
		
		fclose(fp);

		return true;
	}

	bool SaveBmp(wchar_t* filename, GLubyte* _image, int width, int height)
	{
		// DIB의 형식을 정의
		BITMAPINFO dib_define;
		dib_define.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
		dib_define.bmiHeader.biWidth = width;
		dib_define.bmiHeader.biHeight = height;
		dib_define.bmiHeader.biPlanes = 1;
		dib_define.bmiHeader.biBitCount = 24;
		dib_define.bmiHeader.biCompression = BI_RGB;
		dib_define.bmiHeader.biSizeImage = (((width * 24 + 31) & ~31) >> 3) * height;
		dib_define.bmiHeader.biXPelsPerMeter = 0;
		dib_define.bmiHeader.biYPelsPerMeter = 0;
		dib_define.bmiHeader.biClrImportant = 0;
		dib_define.bmiHeader.biClrUsed = 0;

		// DIB 파일의 헤더 내용을 구성한다.
		BITMAPFILEHEADER dib_format_layout;
		ZeroMemory(&dib_format_layout, sizeof(BITMAPFILEHEADER));
		dib_format_layout.bfType = *(WORD*)"BM";
		dib_format_layout.bfSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);// +dib_define.bmiHeader.biSizeImage;
		dib_format_layout.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);

		// 파일 생성.
		FILE* fp = nullptr;
		fopen_s(&fp, (char*)filename, "wb");
		if (nullptr == fp)
		{
			OutputDebugStringA("Open ERROR");
			return false;
		}

		// 생성 후 헤더 및 데이터 쓰기.
		fwrite(&dib_format_layout, 1, sizeof(BITMAPFILEHEADER), fp);
		fwrite(&dib_define, 1, sizeof(BITMAPINFOHEADER), fp);
		fwrite(_image, 1, dib_define.bmiHeader.biSizeImage, fp);
		fclose(fp);

		return true;
	}
}

#endif // !__FILE_HANDLE_H_
