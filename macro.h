#ifndef MACRO_H
#define MACRO_H

#include "structure.h"

extern int sizeOfString;                  // Длина строки

extern char* DEFTAB[];      // Таблица макроопределений
extern char* ARGTAB;                 // Таблица аргументов
extern Namtab namtab[];       // Таблица, хранящая имена фактических макропараметров

void assemble(str*);
void clearNamtab(Namtab*, int);   // Очистка таблицы макроимен
void writeToDeftab(char*, int);   // Запись строки макроопределения в DEFTAB
void macroExpand(Namtab, char*);  // Макрорасширение

#endif