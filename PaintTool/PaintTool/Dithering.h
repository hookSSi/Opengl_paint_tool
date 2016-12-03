#ifndef  __DITHERING_H_
#define __DITHERING_H_
#include<math.h>

float fs_coeffs[4] = {7.0 / 16.0, 3.0 / 16.0, 5.0 / 16.0, 1.0 / 16.0};

int threshold_map_2x2[2][2] = { {1, 3 }, {8, 0} };

float Find_Closest_Color(unsigned char oldPixel) { return (oldPixel + 0.5); }

void FS_dither(unsigned char* imageData, int width, int height)
{
	float oldPixel = 0;
	float newPixel = 0;
	float quant_error = 0;

	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < width; j ++)
		{
			if (j % 3 == 0)
			{
				oldPixel = imageData[i * width + j];

				if (oldPixel < 128)
					newPixel = 50;
				else
					newPixel = Find_Closest_Color(oldPixel);

				imageData[i * width + j] = newPixel;
				quant_error = oldPixel - newPixel;

				if (j < width - 1)
					imageData[i * width + (j + 1)] += (fs_coeffs[0] * quant_error);
				if ((j > 0) && (i < height - 1))
					imageData[(i + 1) * width + (j - 1)] += (fs_coeffs[1] * quant_error);
				if (i < height - 1)
					imageData[(i + 1) * width + j] += (fs_coeffs[2] * quant_error);
				if ((j < width - 1) && (i < height - 1))
					imageData[(i + 1) * width + (j + 1)] += (fs_coeffs[3] * quant_error);
			
				imageData[i * width + j + 1] = imageData[i * width + j];
				imageData[i * width + j + 2] = imageData[i * width + j];
			}
		}
	}
}

void Ordered_dither(unsigned char* imageData, int width, int height)
{
	float oldPixel = 0;
	float newPixel = 0;

	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < width; j++)
		{

			oldPixel = imageData[i * width + j] + (imageData[i * width + j] * threshold_map_2x2[i % 2][j % 2]);
			newPixel = Find_Closest_Color(oldPixel);
			imageData[i * width + j] = newPixel;

		}
	}
}


#endif // ! __DITHERING_H_
