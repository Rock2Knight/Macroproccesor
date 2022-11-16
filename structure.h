#ifndef STRUCTURE_H
#define STRUCTURE_H

#include "stdio.h"
#include "string.h"
#include "stdlib.h"

//Структура для строки ассемблерного кода
typedef struct str
{
	char len[100];
	char metka[100];
	char command[100];
	char args[100];
	char comment[100];
}str;

// Описание структуры таблицы указателей на DEFTAB
typedef struct Namtab{
    char* macroName;
    int begin;
    int end;
}Namtab;

typedef struct PeriodArg{
	char* name;
	int value;
}PeriodArg;

#endif