#include <cstdio>
#include "Defragger.h"
#include <Windows.h>
#include <iostream>
using namespace std;

int main()
{
	Defragger def;
	def.getFileMap("G:\\temp.txt");
	
	return 0;
}