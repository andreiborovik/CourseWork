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
	DWORD out_bufSize;
	HANDLE myFile = CreateFile(file,         // открываемый файл
		FILE_READ_ATTRIBUTES,                // нет доступа к устройству
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
	out_bufSize = sizeof(RETRIEVAL_POINTERS_BUFFER) + (sizeFile/(sectorsInCluster*bytesInSector)) * sizeof(this->out_buf->Extents);
	this->out_buf = new RETRIEVAL_POINTERS_BUFFER[out_bufSize];
	DeviceIoControl(myFile, FSCTL_GET_RETRIEVAL_POINTERS, &input_buf, sizeof(input_buf), this->out_buf, out_bufSize, &last, NULL); //возвращает карту кластеров, занятых файлом
	this->UseCluster = (sizeFile + (sectorsInCluster * bytesInSector) - 1) / (sectorsInCluster * bytesInSector);
	this->PrevVCN = out_buf->StartingVcn.QuadPart;
	cout << GetLastError();
	CloseHandle(myFile);
}

PVOLUME_BITMAP_BUFFER Defragger::getFileVolume()
{
	int error = 0;
	LONGLONG sizeBuf;
	int returnLCN;
	HANDLE drive;
	STARTING_VCN_INPUT_BUFFER input_buf;
	VOLUME_BITMAP_BUFFER buf;
	PVOLUME_BITMAP_BUFFER out_buf = &buf;
	DWORD bytes;
	int flag;
	int nMask = 1;
	drive = CreateFile("\\\\.\\G:", 
		GENERIC_READ,
		FILE_SHARE_READ |                    // режим совместного использования
		FILE_SHARE_WRITE,
		NULL,                                // атрибуты безопасности по умолчанию
		OPEN_EXISTING,                       // расположение
		0,                                   // атрибуты файла
		NULL);
	input_buf.StartingVcn.QuadPart = 0;
	error = DeviceIoControl(drive, FSCTL_GET_VOLUME_BITMAP, &input_buf, sizeof(input_buf), out_buf, sizeof(VOLUME_BITMAP_BUFFER), &bytes, NULL);
	if (!error && GetLastError() == ERROR_MORE_DATA)
	{
		input_buf.StartingVcn.QuadPart = 0;
		sizeBuf = out_buf->BitmapSize.QuadPart - out_buf->StartingLcn.QuadPart + sizeof(VOLUME_BITMAP_BUFFER);
		out_buf = (PVOLUME_BITMAP_BUFFER)new char[sizeBuf];
		out_buf->StartingLcn.QuadPart = 0;
		cout << error << " " << GetLastError() << endl;
		error = DeviceIoControl(drive, FSCTL_GET_VOLUME_BITMAP, &input_buf, sizeof(input_buf), out_buf, sizeBuf, &bytes, NULL);
		if (error) //вычисление стартового кластера, куда будет перемещён файл
		{
			for (LONGLONG i = 0; i < out_buf->BitmapSize.QuadPart; i++)
			{
				if (out_buf->Buffer[i] == 255)
				{
					this->StartLCN += 8;
					this->FreeCluster = 0;
					continue;
				}
				while (nMask != 256)
				{
					if (!FreeCluster) returnLCN = this->StartLCN;
					flag = out_buf->Buffer[i] & nMask;
					if (!flag) this->FreeCluster++;
					else this->FreeCluster = 0;
					nMask *= 2;
					this->StartLCN++;
				}
				if (this->FreeCluster >= this->UseCluster) 
				{
					this->StartLCN = returnLCN;
					break;
				}
				nMask = 1;
			}
			CloseHandle(drive);
			return out_buf;
		}
		else
			cout << GetLastError();
			CloseHandle(drive);
			return 0;
	}
}

void Defragger::FileMove(LPCSTR FileName)
{
	MOVE_FILE_DATA input_buf;
	DWORD bytes;
	HANDLE File = CreateFile(FileName, FILE_READ_ATTRIBUTES, FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, NULL, OPEN_EXISTING, 0, NULL);
	HANDLE Drive = CreateFile("\\\\.\\G:", GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);
	input_buf.FileHandle = File;
	input_buf.StartingLcn.QuadPart = this->StartLCN;
	for (int i = 0; i < this->out_buf->ExtentCount; i++)
	{
		input_buf.StartingVcn.QuadPart = this->PrevVCN;
		input_buf.ClusterCount = this->out_buf->Extents[i].NextVcn.QuadPart - this->PrevVCN;
		DeviceIoControl(File, FSCTL_MOVE_FILE, &input_buf, sizeof(input_buf), NULL, NULL, &bytes, NULL);
		input_buf.StartingLcn.QuadPart += input_buf.ClusterCount;
		PrevVCN = this->out_buf->Extents[i].NextVcn.QuadPart;
	}
	CloseHandle(File);
	CloseHandle(Drive);
}
