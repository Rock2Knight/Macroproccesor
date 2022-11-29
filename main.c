#define COUNT_OF_STRINGS 100
#define COUNT_OF_MACRO 50

#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "ctype.h"

#include "structure.h"
#include "objects.h"
#include "settings.h"
#include "macro.h"

extern const char** cmp;
const char* beginCX = "1000h";      // Начальное значение счетчика размещений
extern int countOfArgs; 

extern def DEFTAB[];                          // Таблица макроопределений
extern char** ARGTAB;                         // Таблица аргументов
extern Namtab namtab[];                       // Таблица, хранящая имена фактических макропараметров
extern PeriodArg periodArgs[];                // Таблица параметров периода макрогенерации

extern void assemble(str*); 

int main(){
    str buffer[100];                     // Строки из файла с ассемблерным кодом
    int ind = 0;                         // Индекс строки из исходного кода
    int macro_count = 0;                 // Счетчик макроопределений
    int macro_ind = 0;                   // номер макроопределения
    int arg_count = 0;                   // Количество формальных параметров
    int buf_ind = 0;                     // Индекс для таблицы коррекции макроопределений

    char isParametr = 0;                 // Флаг для обнаружения параметра
    char* macroBuffer[10];               // Таблица макроопределений для коррекции

    ARGTAB = (char**)malloc(sizeof(char*)*10);        // Выделяем память под ARGTAB

    //Инициализация ARGTAB
    for(int i=0; i<10; i++)
        ARGTAB[i] = NULL;

    //Инициализация periodArgs
    for(int i=0; i<COUNT_OF_MACRO; i++){
        periodArgs[i].name = NULL;
        periodArgs[i].value = -1;
    }

    initDeftab();                        // Обнуление DEFTAB

    FILE* source = fopen("asm_ifmacro2.asm", "r");

    // Основной цикл
    while(!feof(source)){
        fgets(buffer[ind].len, sizeof(buffer[ind].len), source);

        // Обработка пустой строки
        if(*buffer[ind].len == '\0' || *buffer[ind].len == '\000'){
            ind += 1;
            continue;
        }

        assemble(&buffer[ind]);          // Разбираем строку на мнемоники

        int isMacro = strcmp(buffer[ind].command, "macro");   //
        int isMend = strcmp(buffer[ind].command, "mend");

        // Если мы встретили обычную строку, при этом счетчик
        // макроопредлений = 0, то просто печатаем строку на экран
        if(macro_count == 0 ^ (macro_count == 0 && isMacro == 0)){
            int is_print = -1;
            int isSame = -1;

            if(namtab[0].macroName!=NULL && macro_count == 0){
                for(int i=0; i<COUNT_OF_MACRO; i++){
                    if(namtab[i].macroName==NULL)
                        break;

                    isMacro = strcmp(namtab[i].macroName, buffer[ind].command);
                    if(isMacro == 0){

                        //Проверка, что данная строка макровызова не встречалась ранее
                        if(macroBuffer[buf_ind]==NULL){
                            macroBuffer[buf_ind] = (char*)malloc(sizeof(char)*50);   // Если нет, то 
                            strcpy(macroBuffer[buf_ind], buffer[ind].len);           // записываем строку в буффер
                        }
                        else{                                            // Если встречалась, то:
                            int t = 0;

                            // Замена символов окончания строки в начале строки пробелами                                   
                            while(isprint(buffer[ind].len[t])==0){
                                buffer[ind].len[t] = ' ';
                                t += 1;
                            }

                            isSame = strcmp(buffer[ind].len, macroBuffer[buf_ind]);    // Проверка, что данная строка макровызова не встречалась ранее
                            buf_ind += 1;                                              // переход на новую строку macroBuffer
                            if(t == 0){                                                   // Если не было выполнено корректировки
                                macroBuffer[buf_ind] = (char*)malloc(sizeof(char)*50);   // добавляем строку в macroBuffer 
                                strcpy(macroBuffer[buf_ind], buffer[ind].len);
                            }
                        }

                        if(isSame == 0)                            // Если является повторением уже встреченной,
                            break;                                 // Выходим из цикла
                        macroExpand(namtab[i], buffer[ind].args);  // Подставляем тело макроса (макрорасширение)
                        is_print = 1;                              // Строка напечатана
                        break;
                    }
                }
            }

            if(isSame == 0){
                isSame = -1;     // Переходим к следующей строке файла, если
                continue;        // строка встречалась ранее
            }
            if(is_print == 1)
                continue;        // и если строка напечатана
            printf("%s", buffer[ind].len);                       // Если мы не в теле макроопределеня, просто печатаем строку
            isMacro = -1;
        }
        else{             

            // Проделываем действия по запоминанию тела макроопределения
            if(isMacro==0){
                //Запись в таблицу макроимен
                namtab[macro_count].macroName = (char*)malloc(sizeof(char)*100);
                strcpy(namtab[macro_count].macroName, buffer[ind].metka);
                namtab[macro_count].begin = macro_ind;
                macro_count += 1;

        
                // (1) Подстановка в ARGTAB формальных параметров макроса
                int i_arg = 0;
                arg_count = 0;
                ARGTAB[arg_count] = (char*)malloc(sizeof(char)*50);
                int arg_index = 0;
                while(buffer[ind].args[i_arg]!='\0' || buffer[ind].args[i_arg]!='\000'){

                    //В случае обнаружения разделителя, переходим к следующему элементу в ARGTAB
                    if(buffer[ind].args[i_arg] == ','){
                        arg_count += 1;
                        ARGTAB[arg_count] = (char*)malloc(sizeof(char)*50);
                        arg_index = 0;
                        i_arg += 1;
                        continue;
                    }
  
                    // Подставляем символ из аргумента в строку параметра ARGTAB
                    if(isalpha(buffer[ind].args[i_arg])){
                        ARGTAB[arg_count][arg_index] = buffer[ind].args[i_arg];
                        arg_index += 1;
                    }
            
                    i_arg += 1;
                }
                // Окончание (1)
            }
            else if(strcmp(buffer[ind].command, "mend")==0){        // Действия, если мы встретили признак окончания макроопределения
                macro_count -= 1;
                namtab[macro_count].end = macro_ind;
            }

            // Запись тела макроопределения в DEFTAB
            DEFTAB[macro_ind].stroka = (char*)malloc(sizeof(char)*100);
            writeToDeftab(&buffer[ind], macro_ind);
            macro_ind += 1;    // Переход к следующей sстроке в DEFTAB
        }

        isMacro = -1;
        isMend = -1;
        ind+=1;
    }

    clearNamtab(namtab, COUNT_OF_MACRO);

    // Очистка ARGTAB
    for(int i=0; i<10; i++){
        if(ARGTAB[i]!=NULL)
            free(ARGTAB[i]);
    }
    if(ARGTAB != NULL){
        free(ARGTAB);
        ARGTAB = NULL;
    }

    return 0;
}