#define COUNT_OF_STRINGS 100
#define COUNT_OF_MACRO 50

#include "structure.h"

def DEFTAB[COUNT_OF_STRINGS];          // Таблица макроопределений
char** ARGTAB = NULL;                  // Таблица аргументов
Namtab namtab[COUNT_OF_MACRO];         // Таблица, хранящая имена фактических макропараметров
PeriodArg periodArgs[COUNT_OF_MACRO];  // Таблица параметров периода макрогенерации