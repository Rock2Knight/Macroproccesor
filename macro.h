#ifndef MACRO_H
#define MACRO_H

#include "structure.h"

extern const char** cmp;

extern int sizeOfString;                      // Длина строки
extern int countOfArgs;                       // Действительное количество параметров периода макрогенерации

extern def DEFTAB[];                          // Таблица макроопределений
extern char** ARGTAB;                         // Таблица аргументов
extern Namtab namtab[];                       // Таблица, хранящая имена фактических макропараметров
extern PeriodArg periodArgs[COUNT_OF_MACRO];  // Таблица параметров периода макрогенерации

void initDeftab();                            // Иницализация DEFTAB;
void assemble(str*);
void clearNamtab(Namtab*, int);               // Очистка таблицы макроимен
void writeToDeftab(str*, int);                // Запись строки макроопределения в DEFTAB
int trueCondition(def);                       // Проверка условия в теле макроопределения
void macroExpand(Namtab, char*);              // Макрорасширение

#endif