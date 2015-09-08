#include "stdafx.h"
#include "BitMap.h"
#include <stdio.h>  
#include <iostream>  
#include <fstream>  


BitMap::BitMap()
{
	dataBuf = NULL;
}


BitMap::~BitMap()
{

}

//read bitmap info from a file  
bool BitMap::Read(const char* fileName)
{
	FILE *_f;
	fopen_s(&_f, fileName, "rb");//open file  
	if (_f == NULL) return false;

	fread(&fileHeader, sizeof(BITMAPFILEHEADER), 1, _f);//read BITMAPFILEHEADER  

	fread(&infoHeader, sizeof(BITMAPINFOHEADER), 1, _f);//read BITMAPINFOHEADER  
	width_p = infoHeader.biWidth;
	height_p = infoHeader.biHeight;
	bitCount = infoHeader.biBitCount;

	dataBuf = new uint8[width_p*height_p*bitCount/8];//read image data  
	fread(dataBuf, 1, width_p*height_p*bitCount/8, _f);

	fclose(_f);//close file  
	return true;
}

void BitMap::Close()
{
	if (dataBuf != NULL)
	{
		delete[] dataBuf;
		dataBuf = NULL;
	}
}

ColourValue BitMap::GetColour(int x, int y)
{
	if (dataBuf != NULL && x >= 0 && x < width_p && y >= 0 && y < height_p)
	{
		int index = (x + y*width_p) * 3;
		uint8 red = dataBuf[index + 2];
		uint8 green = dataBuf[index + 1];
		uint8 blue = dataBuf[index];
		return ColourValue(red / 255.0f, green / 255.0f, blue / 255.0f);
	}
	return ColourValue::ZERO;
}

ColourValue BitMap::GetColour(float u, float v)
{
	return GetColour((int)(u*(width_p - 1)), (int)(v*(height_p - 1)));
}