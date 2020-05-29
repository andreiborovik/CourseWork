#pragma once
#include <cstdio>
#include <Windows.h>
class Defragger
{
public:
	Defragger();
	~Defragger();
	void getFileMap(LPCTSTR file);
};

