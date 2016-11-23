#ifndef __FILE_HANDLE_H_
#define __FILE_HANDLE_H_

#include <stdlib.h>
#include <assert.h>
#include <windows.h>
#include <stdio.h>
#include <list>
#include <algorithm>
#include <Commdlg.h>

namespace BMP
{
	const int BITMAP_ID = 0x4D42;

	const int MAX_PATH_LENGTH = 40; // 파일 경로 최대 길이

	static wchar_t lpstrFile[MAX_PATH_LENGTH]; // 파일 경로 문자열 버퍼

	unsigned char* LoadBMP(char* filename, BITMAPINFOHEADER *bitmapInfoHeader);

	unsigned char* LoadBMP(char* filename, BITMAPINFOHEADER *bitmapInfoHeader)
	{
		FILE *filePtr; // file pointer
		BITMAPFILEHEADER bitmapFileHeader; // BMP header
		unsigned char* bitmapImage; // BMP image data
		int imageIdx = 0; // image index
		unsigned char tempRGB; // temp for changing R, B

							   // Read file as binary
		filePtr = fopen(filename, "rb"); // Open file for read
		if (filePtr == nullptr)
			return nullptr;

		// Read BMP header
		fread(&bitmapFileHeader, sizeof(BITMAPCOREHEADER), 1, filePtr);

		if (bitmapFileHeader.bfType != BITMAP_ID)
		{
			fclose(filePtr);
			return nullptr;
		}

		// Read BMP info
		fread(bitmapInfoHeader, sizeof(BITMAPCOREHEADER), 1, filePtr);

		fseek(filePtr, bitmapFileHeader.bfOffBits, SEEK_SET);

		// Create Image data memory
		bitmapImage = (unsigned char*)malloc(bitmapInfoHeader->biSizeImage);

		if (!bitmapImage)
		{
			free(bitmapImage);
			fclose(filePtr);
			return nullptr;
		}

		// Read BMP data
		fread(bitmapImage, 1, bitmapInfoHeader->biSizeImage, filePtr);

		if (bitmapImage == nullptr)
		{
			fclose(filePtr);
			return nullptr;
		}

		// BMP is BGR so change R to B
		for (imageIdx = 0; imageIdx < bitmapInfoHeader->biSizeImage; imageIdx += 3)
		{
			tempRGB = bitmapImage[imageIdx];
			bitmapImage[imageIdx] = bitmapImage[imageIdx + 2];
			bitmapImage[imageIdx + 2] = tempRGB;
		}
		
		fclose(filePtr);
		return bitmapImage;
	}
}

#endif // !__FILE_HANDLE_H_
