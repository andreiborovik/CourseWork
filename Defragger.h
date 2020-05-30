#pragma once
#include <cstdio>
#include <Windows.h>
class Defragger
{
	PRETRIEVAL_POINTERS_BUFFER out_buf;
public:
	Defragger();
	~Defragger();
	PRETRIEVAL_POINTERS_BUFFER getFileMap(LPCTSTR file);
	PVOLUME_BITMAP_BUFFER getFileVolume();
};

