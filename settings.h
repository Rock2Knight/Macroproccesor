// Служебные функции
#ifndef SETTINGS_H
#define SETTINGS_H

#include "stdio.h"
#include "string.h"
#include "stdlib.h"

#include "structure.h"
#include "objects.h"

extern def DEFTAB[];                          // Таблица макроопределений
extern char** ARGTAB;                         // Таблица аргументов
extern Namtab namtab[];                       // Таблица, хранящая имена фактических макропараметров
extern PeriodArg periodArgs[];                // Таблица параметров периода макрогенерации

void assemble(str*);                // Функция разбора строки программы на ассемблере на составляющие
void conditionStrings(str*, int);             // Проставялем флаги условных строк в DEFTAB
void printSting(int, int);          // Печать строки макроопределения
int writeToArgtab(char*);           // Запись фактических параметров в ARGTAB

#endif