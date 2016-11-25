#ifndef __FILE_HANDLE_H_
#define __FILE_HANDLE_H_

#include <stdlib.h>
#include <assert.h>
#include <windows.h>
#include <stdio.h>
#include <list>
#include <algorithm>
#include <Commdlg.h>
#include "Debug.h"

namespace BMP
{

	const int BITMAP_ID = 0x4D42;

	unsigned char* LoadBMP(char* filename, BITMAPINFOHEADER *bitmapInfoHeader);
	int SaveBMP(char* filename, int width, int height, unsigned char* imageData);

	unsigned char* LoadBMP(char* filename, BITMAPINFOHEADER *bitmapInfoHeader)
	{
		FILE *filePtr; // file pointer
		BITMAPFILEHEADER bitmapFileHeader; // BMP header
		unsigned char* bitmapImage; // BMP image data
		int imageIdx = 0; // image index
		unsigned char tempRGB; // temp for changing R, B

							   // Read file as binary
		filePtr = fopen(filename, "rb");
		if (filePtr == nullptr)
			return nullptr;

		// Read BMP header
		fread(&bitmapFileHeader, sizeof(BITMAPFILEHEADER), 1, filePtr);

		if (bitmapFileHeader.bfType != BITMAP_ID)
		{
			fclose(filePtr);
			return nullptr;
		}

		// Read BMP info
		fread(bitmapInfoHeader, sizeof(BITMAPINFOHEADER), 1, filePtr);

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

	int SaveBMP(const char* filename, int width, int height, unsigned char* imageData)
	{
		FILE* filePtr; // file Pointer
		BITMAPFILEHEADER bitmapFileHeader; // BMP file header
		BITMAPINFOHEADER bitmapInfoHeader; // BMP info header
		int imageIdx; // RGB -> BGR index
		unsigned char tempRGB;

		// Open file as binary writing
		filePtr = fopen(filename, "wb");
		if (!filePtr)
			return 0;

		// Fill BMP header
		bitmapFileHeader.bfSize = sizeof(BITMAPFILEHEADER);
		bitmapFileHeader.bfType = BITMAP_ID;
		bitmapFileHeader.bfReserved1 = 0;
		bitmapFileHeader.bfReserved2 = 0;
		bitmapFileHeader.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);

		// Fill BMP Info header
		bitmapInfoHeader.biSize = sizeof(BITMAPINFOHEADER);
		bitmapInfoHeader.biPlanes = 1;
		bitmapInfoHeader.biBitCount = 24; // 24bits
		bitmapInfoHeader.biCompression = BI_RGB; // no compressing
		bitmapInfoHeader.biSizeImage = width * abs(height) * 3; // w *h * (RGB bytes)
		bitmapInfoHeader.biXPelsPerMeter = 0;
		bitmapInfoHeader.biYPelsPerMeter = 0;
		bitmapInfoHeader.biClrUsed = 0;
		bitmapInfoHeader.biClrImportant = 0;
		bitmapInfoHeader.biWidth = width;
		bitmapInfoHeader.biHeight = height;

		for (imageIdx = 0; imageIdx < bitmapInfoHeader.biSizeImage; imageIdx += 3)
		{
			tempRGB = imageData[imageIdx];
			imageData[imageIdx] = imageData[imageIdx + 2];
			imageData[imageIdx + 2] = tempRGB;
		}

		// Save BMP file header 
		fwrite(&bitmapFileHeader, 1, sizeof(BITMAPFILEHEADER), filePtr);

		// Save BMP Info header
		fwrite(&bitmapInfoHeader, 1, sizeof(BITMAPINFOHEADER), filePtr);

		// Save Image Data
		fwrite(imageData, 1, bitmapInfoHeader.biSizeImage, filePtr);

		fclose(filePtr);

		for (imageIdx = 0; imageIdx < bitmapInfoHeader.biSizeImage; imageIdx += 3)
		{
			tempRGB = imageData[imageIdx];
			imageData[imageIdx] = imageData[imageIdx + 2];
			imageData[imageIdx + 2] = tempRGB;
		}

		return 1;
	}

	void SaveScreenshot(int winWidth, int winHeight, unsigned char* _imagebuffer)
	{
		glReadPixels(0, 0, winWidth * 1, winHeight * 1, GL_RGB, GL_UNSIGNED_BYTE, _imagebuffer);

		SaveBMP("ScreenShot.bmp", winWidth, winHeight, _imagebuffer);
	}
}

#endif // !__FILE_HANDLE_H_
