// Структуры для работы макропроцессора
#ifndef OBJECTS_H
#define OBJECTS_H

#include "structure.h"

extern def DEFTAB[];                          // Таблица макроопределений
extern char** ARGTAB;                         // Таблица аргументов
extern Namtab namtab[];                       // Таблица, хранящая имена фактических макропараметров
extern PeriodArg periodArgs[COUNT_OF_MACRO];  // Таблица параметров периода макрогенерации

#endif