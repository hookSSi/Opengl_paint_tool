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

const int MAX_PATH_LENGTH = 40; // ���� ��� �ִ� ����

static wchar_t lpstrFile[MAX_PATH_LENGTH]; // ���� ��� ���ڿ� ����

typedef unsigned char byte;

GLubyte* imageBuffer = nullptr; // �̹��� ����
static GLuint texture[2];

namespace BMP
{
	bool LoadBmp(wchar_t* filename, GLubyte** _image);
	bool SaveBmp(wchar_t* filename, GLubyte* _image, int width, int height);

	bool LoadBmp(wchar_t* filename, GLubyte** _image)
	{
		FILE* fp;

		// ��Ʈ�� ���� ����ü
		BITMAPFILEHEADER BMFH; // BitMap File Header
		BITMAPINFOHEADER BMH; // BitMap Info Header

		fopen_s(&fp, (char*)filename, "rb");
		if (nullptr == fp)
		{
			OutputDebugStringA("Open ERROR");
			return false;
		}

		// ������ ũ�⸸ŭ ���Ͽ��� �о��, �׸��� ��Ʈ�� ���� ����� �־��ش�
		fread(&BMFH, sizeof(BITMAPFILEHEADER), 1, fp);
		if (BMFH.bfType != 0x4d42) // ��Ʈ�� ������ �ƴϸ� �����Ѵ�.
		{
			fclose(fp);
			OutputDebugStringA("BMP ������ �ƴմϴ�.");
			return false;
		}

		// ��������� �ִ� ũ���� ������ŭ ����
		fread(&BMH, sizeof(BITMAPINFOHEADER), 1, fp);
		
		// 24��Ʈ�̰� ���� �ȵǾ����� Ȯ��
		if (BMH.biBitCount != 24 || BMH.biCompression != BI_RGB)
		{
			fclose(fp);
			return false;
		}

		int width = BMH.biWidth;
		int height = BMH.biHeight - 1;
		int bytePerScanLine = (width * 3 + 3) & ~ 3; // �е�
		int size = BMFH.bfSize;

		*_image = (byte*)malloc(size);

		fread(*_image, size, 1, fp); // ������ ������ ��� �о��
		
		fclose(fp);

		return true;
	}

	bool SaveBmp(wchar_t* filename, GLubyte* _image, int width, int height)
	{
		// DIB�� ������ ����
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

		// DIB ������ ��� ������ �����Ѵ�.
		BITMAPFILEHEADER dib_format_layout;
		ZeroMemory(&dib_format_layout, sizeof(BITMAPFILEHEADER));
		dib_format_layout.bfType = *(WORD*)"BM";
		dib_format_layout.bfSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);// +dib_define.bmiHeader.biSizeImage;
		dib_format_layout.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);

		// ���� ����.
		FILE* fp = nullptr;
		fopen_s(&fp, (char*)filename, "wb");
		if (nullptr == fp)
		{
			OutputDebugStringA("Open ERROR");
			return false;
		}

		// ���� �� ��� �� ������ ����.
		fwrite(&dib_format_layout, 1, sizeof(BITMAPFILEHEADER), fp);
		fwrite(&dib_define, 1, sizeof(BITMAPINFOHEADER), fp);
		fwrite(_image, 1, dib_define.bmiHeader.biSizeImage, fp);
		fclose(fp);

		return true;
	}
}

#endif // !__FILE_HANDLE_H_
