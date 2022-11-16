#ifndef MACRO_H
#define MACRO_H

#include "structure.h"

extern int sizeOfString;                  // Длина строки
extern int countOfArgs;                   // Действительное количество параметров периода макрогенерации

extern char* DEFTAB[];      // Таблица макроопределений
extern char** ARGTAB;                 // Таблица аргументов
extern Namtab namtab[];       // Таблица, хранящая имена фактических макропараметров
extern PeriodArg periodArgs[COUNT_OF_MACRO];  // Таблица параметров периода макрогенерации

void assemble(str*);
void clearNamtab(Namtab*, int);   // Очистка таблицы макроимен
void writeToDeftab(char*, char*, char*, char*, int);   // Запись строки макроопределения в DEFTAB
void macroExpand(Namtab, char*);  // Макрорасширение

#endif