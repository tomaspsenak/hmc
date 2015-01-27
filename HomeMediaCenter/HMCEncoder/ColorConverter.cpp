#include "StdAfx.h"
#include "ColorConverter.h"

void RGBtoYUY2(BITMAPINFOHEADER & bmi, BYTE * srcBuffer, BYTE * dstBuffer)
{
	BYTE R, B, G, V;
	BYTE * pSrcBuffer;
	DWORD lineSize = DIBWIDTHBYTES(bmi);
	BOOL is32bit = (bmi.biBitCount == 32);

	for (int i = (bmi.biHeight - 1); i >= 0; i--)
	{
		pSrcBuffer = srcBuffer + (lineSize * i);

		for (int j = 0; j < bmi.biWidth; j++)
		{
			B = *pSrcBuffer++;
			G = *pSrcBuffer++;
			R = *pSrcBuffer++;
			if (is32bit) pSrcBuffer++;

			*dstBuffer++ = (BYTE)((0.299 * R) + (0.587 * G) + (0.114 * B)); //Y

			if (j % 2 == 0)
			{
				*dstBuffer++ = (BYTE)((-0.14317 * R) + (-0.28886 * G) + (0.436 * B) + 128); //U
				V = (BYTE)((0.615 * R) + (-0.51499 * G) + (-0.10001 * B) + 128); //V temp
			}
			else
			{
				*dstBuffer++ = V; //V
			}
		}
	}
}

void RGBtoYV12(BITMAPINFOHEADER & bmi, BYTE * srcBuffer, BYTE * dstBuffer)
{
	BYTE R, B, G;
	BYTE * pSrcBuffer;
	BYTE * pDataY = dstBuffer;
	BYTE * pDataV = pDataY + (bmi.biWidth * bmi.biHeight);
	BYTE * pDataU = pDataV + ((bmi.biWidth * bmi.biHeight) >> 2);
	DWORD lineSize = DIBWIDTHBYTES(bmi);
	BOOL is32bit = (bmi.biBitCount == 32);

	for (int i = (bmi.biHeight - 1); i >= 0; i--)
	{
		pSrcBuffer = srcBuffer + (lineSize * i);

		for (int j = 0; j < bmi.biWidth; j++)
		{
			B = *pSrcBuffer++;
			G = *pSrcBuffer++;
			R = *pSrcBuffer++;
			if (is32bit) pSrcBuffer++;

			*pDataY++ = (BYTE)((0.299 * R) + (0.587 * G) + (0.114 * B)); //Y

			if (i % 2 == 0 && j % 2 == 0)
			{
				*pDataV++ = (BYTE)((0.615 * R) + (-0.51499 * G) + (-0.10001 * B) + 128); //V
				*pDataU++ = (BYTE)((-0.14317 * R) + (-0.28886 * G) + (0.436 * B) + 128); //U
			}
		}
	}
}

void RGBtoRGB(BITMAPINFOHEADER & bmi, BYTE * srcBuffer, BYTE * dstBuffer, BITMAPINFOHEADER & dstBmi)
{
	BYTE R, B, G;
	BYTE * pSrcBuffer, * pDstBuffer;
	DWORD lineSize = DIBWIDTHBYTES(bmi);
	DWORD dstLineSize = DIBWIDTHBYTES(dstBmi);
	BOOL is32bit = (bmi.biBitCount == 32);
	BOOL isDst32bit = (dstBmi.biBitCount == 32);

	for (int i = 0; i < bmi.biHeight; i++)
	{
		pSrcBuffer = srcBuffer + (lineSize * i);
		pDstBuffer = dstBuffer + (dstLineSize * i);

		for (int j = 0; j < bmi.biWidth; j++)
		{
			B = *pSrcBuffer++;
			G = *pSrcBuffer++;
			R = *pSrcBuffer++;
			if (is32bit) pSrcBuffer++;

			*pDstBuffer++ = B;
			*pDstBuffer++ = G;
			*pDstBuffer++ = R;
			if (isDst32bit) *pDstBuffer++ = 0;
		}
	}
}