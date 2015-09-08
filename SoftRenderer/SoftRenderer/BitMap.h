#pragma once

#ifndef __BitMap_H__
#define __BitMap_H__

#include "ColourValue.h"

class BitMap
{
public:

	BitMap();
	~BitMap();

protected:
	BITMAPFILEHEADER fileHeader;
	BITMAPINFOHEADER infoHeader;
	uint8 *dataBuf;
	int width_p, height_p;
	int bitCount;
	ColourValue GetColour(int x, int y);
public:
	inline bool HasRead() const{
		return (dataBuf != NULL);
	}
	bool Read(const char *fileName);
	void Close();

	ColourValue GetColour(float u, float v);
};

#endif