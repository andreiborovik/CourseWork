#include "Interface.h"
using namespace std;
void Interface::Menu()
{
	char pathFile[256];
	cout << "����� ���������� � ��������������!" << endl << endl << "������� ������ ���� �����, ������� ���������� �����������������: ";
	cin >> pathFile;
	Defragger def;
	def.getFileMap(pathFile);
	def.getFileVolume();
	def.FileMove(pathFile);
}
