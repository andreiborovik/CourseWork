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

void Defragger::getFileMap(LPCTSTR file)
{
	DWORD sectorsInCluster;
	DWORD freeClusters;
	DWORD bytesInSector;
	DWORD totalClusters;
	DWORD last = 0;
	STARTING_VCN_INPUT_BUFFER input_buf;
	input_buf.StartingVcn.QuadPart = 0;
	DWORD sizeFile;
	ULONG out;
	PRETRIEVAL_POINTERS_BUFFER out_buf;
	DWORD out_bufSize;
	HANDLE myFile = CreateFile(file,  // открываемое устройство
		0,                                   // нет доступа к устройству
		FILE_SHARE_READ |                    // режим совместного использования
		FILE_SHARE_WRITE|
		FILE_SHARE_DELETE,
		NULL,                                // атрибуты безопасности по умолчанию
		OPEN_EXISTING,                       // расположение
		0,                                   // атрибуты файла
		NULL);
	cout << GetLastError() << endl;
	GetDiskFreeSpace("G:\ ",				 //имя тома
		&sectorsInCluster,					 //число секторов в кластере
		&bytesInSector,						 //число байт в секторе
		&freeClusters,						 //число свободных кластеров
		&totalClusters);					 //общее число кластеров
	cout << GetLastError() << endl;
	sizeFile = GetFileSize(myFile, NULL);
	out_bufSize = sizeof(RETRIEVAL_POINTERS_BUFFER) + (sizeFile/(sectorsInCluster*bytesInSector)) * sizeof(out_buf->Extents);
	out_buf = new RETRIEVAL_POINTERS_BUFFER[out_bufSize];
	DeviceIoControl(myFile, FSCTL_GET_RETRIEVAL_POINTERS, &input_buf, sizeof(input_buf), out_buf, out_bufSize, &last, NULL); //возвращает карту кластеров, занятых файлом
	cout << GetLastError();
}
