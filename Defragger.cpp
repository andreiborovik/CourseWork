#include "Defragger.h"
#include <iostream>
#include <cstdlib>
#include <Windows.h>

using namespace std;

Defragger::Defragger()
{
}


Defragger::~Defragger()
{
}

void Defragger::getFileMap(const char* file)
{
	DWORD sectorsInCluster;
	DWORD freeClusters;
	DWORD bytesInSector;
	DWORD totalClusters;
	DWORD last = 0;
	STARTING_VCN_INPUT_BUFFER buf1;
	buf1.StartingVcn.QuadPart = 0;
	ULONG sizeFile;
	ULONG out;
	PRETRIEVAL_POINTERS_BUFFER OutBuf;
	HANDLE myFile = CreateFile("\\\\.\\G:",  // открываемое устройство
		0,                                   // нет доступа к устройству
		FILE_SHARE_READ |                    // режим совместного использования
		FILE_SHARE_WRITE|
		FILE_SHARE_DELETE,
		NULL,                                // атрибуты безопасности по умолчанию
		OPEN_EXISTING,                       // расположение
		0,                                   // атрибуты файла
		NULL);
	GetDiskFreeSpace(file,				 //имя тома
		&sectorsInCluster,					 //число секторов в кластере
		&bytesInSector,						 //число байт в секторе
		&freeClusters,						 //число свободных кластеров
		&totalClusters);					 //общее число кластеров
	cout << sectorsInCluster << endl << bytesInSector << endl << freeClusters << endl << totalClusters << endl;
	out = sizeof(RETRIEVAL_POINTERS_BUFFER) + ((sectorsInCluster*bytesInSector*totalClusters) * sizeof(OutBuf->Extents));
	OutBuf = new RETRIEVAL_POINTERS_BUFFER[out];
	DeviceIoControl(myFile, FSCTL_GET_RETRIEVAL_POINTERS, &buf1, sizeof(buf1), OutBuf, out, &last, NULL);
	pOutSize = pOutSize + (RPB->ExtentCount - RPB->StartingVcn.QuadPart) + sizeof(RETRIEVAL_POINTERS_BUFFER);
	RPB = (PRETRIEVAL_POINTERS_BUFFER)malloc(pOutSize);
	RPB->StartingVcn.QuadPart = 0;
	DeviceIoControl(myFile, FSCTL_GET_RETRIEVAL_POINTERS, &buf1, sizeof(buf1), RPB, pOutSize, &last, NULL);
	cout << GetLastError();
}
