#include "Interface.h"
using namespace std;
void Interface::Menu()
{
	char pathFile[256];
	cout << "Добро пожаловать в дефрагментатор!" << endl << endl << "Введите полный путь файла, который необходимо дефрагментировать: ";
	cin >> pathFile;
	Defragger def;
	def.getFileMap(pathFile);
	def.getFileVolume();
	def.FileMove(pathFile);
}
