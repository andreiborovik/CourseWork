#pragma once
#include <cstdio>
#include <Windows.h>
class Defragger
{
	int StartLCN = 0; //начальный кластер для перемещения файла
	int FreeCluster = 0; //количество свободных кластеров для перемещения
	int UseCluster = 0; //количество занятых кластеров файлом
	int PrevVCN = 0;
	PRETRIEVAL_POINTERS_BUFFER out_buf = 0;
public:
	Defragger();
	~Defragger();
	void getFileMap(LPCTSTR file);
	PVOLUME_BITMAP_BUFFER getFileVolume();
	void FileMove(LPCSTR FileName);
};

