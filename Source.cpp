#include <cstdio>
#include "Defragger.h"
#include <Windows.h>
#include <iostream>
#include "Interface.h"
using namespace std;

int main()
{
	setlocale(LC_ALL, "Russian");
	Interface inter;
	inter.Menu();
	system("pause");
	return 0;
}