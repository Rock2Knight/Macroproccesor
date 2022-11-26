#ifndef MACRO_H
#define MACRO_H

#include "structure.h"
#include "objects.h"
#include "settings.h"

extern const char** cmp;

extern int sizeOfString;                      // Длина строки
extern int countOfArgs;                       // Действительное количество параметров периода макрогенерации

extern def DEFTAB[];                          // Таблица макроопределений
extern char** ARGTAB;                         // Таблица аргументов
extern Namtab namtab[];                       // Таблица, хранящая имена фактических макропараметров
extern PeriodArg periodArgs[];                // Таблица параметров периода макрогенерации

extern void assemble(str*);                   

void initDeftab();                            // Иницализация DEFTAB;
void clearNamtab(Namtab*, int);               // Очистка таблицы макроимен
void writeToDeftab(str*, int);                // Запись строки макроопределения в DEFTAB
int trueCondition(def);                       // Проверка условия в теле макроопределения
void macroExpand(Namtab, char*);              // Макрорасширение

#endif