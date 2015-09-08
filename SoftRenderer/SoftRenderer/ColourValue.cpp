#include "stdafx.h"
#include "ColourValue.h"

const ColourValue ColourValue::ZERO = ColourValue(0.0f, 0.0f, 0.0f, 0.0f);
const ColourValue ColourValue::Black = ColourValue(0.0f, 0.0f, 0.0f);
const ColourValue ColourValue::White = ColourValue(1.0f, 1.0f, 1.0f);
const ColourValue ColourValue::Red = ColourValue(1.0f, 0.0f, 0.0f);
const ColourValue ColourValue::Green = ColourValue(0.0f, 1.0f, 0.0f);
const ColourValue ColourValue::Blue = ColourValue(0.0f, 0.0f, 1.0f);
const ColourValue ColourValue::Gray = ColourValue(0.4f, 0.4f, 0.4f);


ARGB ColourValue::getAsARGB(void) const
{
	uint8 val8;
	uint32 val32 = 0;

	// Convert to 32bit pattern
	// (ARGB = 8888)

	// Alpha
	val8 = static_cast<uint8>(a * 255);
	val32 = val8 << 24;

	// Red
	val8 = static_cast<uint8>(r * 255);
	val32 += val8 << 16;

	// Green
	val8 = static_cast<uint8>(g * 255);
	val32 += val8 << 8;

	// Blue
	val8 = static_cast<uint8>(b * 255);
	val32 += val8;


	return val32;
}

RGBA ColourValue::getAsRGBA(void) const
{
	uint8 val8;
	uint32 val32 = 0;

	// Convert to 32bit pattern
	// (RGBA = 8888)

	// Red
	val8 = static_cast<uint8>(r * 255);
	val32 += val8 << 24;

	// Green
	val8 = static_cast<uint8>(g * 255);
	val32 += val8 << 16;

	// Blue
	val8 = static_cast<uint8>(b * 255);
	val32 += val8 << 8;

	// Alpha
	val8 = static_cast<uint8>(a * 255);
	val32 = val8;

	return val32;
}

void ColourValue::setAsARGB(const ARGB val)
{
	uint32 val32 = val;

	// Convert from 32bit pattern
	// (ARGB = 8888)

	// Alpha
	a = ((val32 >> 24) & 0xFF) / 255.0f;

	// Red
	r = ((val32 >> 16) & 0xFF) / 255.0f;

	// Green
	g = ((val32 >> 8) & 0xFF) / 255.0f;

	// Blue
	b = (val32 & 0xFF) / 255.0f;
}

void ColourValue::setAsRGBA(const RGBA val)
{
	uint32 val32 = val;

	// Convert from 32bit pattern
	// (RGBA = 8888)

	// Red
	r = ((val32 >> 24) & 0xFF) / 255.0f;

	// Green
	g = ((val32 >> 16) & 0xFF) / 255.0f;

	// Blue
	b = ((val32 >> 8) & 0xFF) / 255.0f;

	// Alpha
	a = (val32 & 0xFF) / 255.0f;
}
