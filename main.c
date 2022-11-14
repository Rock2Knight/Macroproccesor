#define COUNT_OF_STRINGS 100
#define COUNT_OF_MACRO 50

#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "ctype.h"

#include "structure.h"
#include "macro.h"

const char* beginCX = "1000h";      // Начальное значение счетчика размещений

extern char* DEFTAB[];        // Таблица макроопределений
extern char* ARGTAB;          // Таблица аргументов
extern Namtab namtab[];       // Таблица, хранящая имена фактических макропараметров

// Нужно допилить замену формальных параметров на их коды в ARGTAB

int main(){
    str buffer[100];                     // Строки из файла с ассемблерным кодом
    int ind = 0;                         // Индекс строки из исходного кода
    int macro_count = 0;                 // Счетчик макроопределений
    int macro_ind = 0;                   // номер макроопределения
    int arg_count = 0;                   // Количество формальных параметров
    int call_count = 0;

    char isParametr = 0;                 // Флаг для обнаружения параметра
    char* macroBuffer[10];               // Строки макроопределений для коррекции

    FILE* source = fopen("asm_example1.asm", "r");


    while(!feof(source)){
        fgets(buffer[ind].len, sizeof(buffer[ind].len), source);

        // Обработка пустой строки
        if(*buffer[ind].len == '\0' || *buffer[ind].len == '\000'){
            ind += 1;
            continue;
        }

        assemble(&buffer[ind]);

        // Если мы встретили обычную строку, при этом счетчик
        // макроопредлений на 0, то просто печатаем строку на экран
        int isMacro = strcmp(buffer[ind].command, "macro");
        int isMend = strcmp(buffer[ind].command, "mend");

        if(macro_count == 0 ^ (macro_count == 0 && isMacro == 0)){
            int is_print = -1;
            int isSame = -1;

            if(namtab[0].macroName!=NULL && macro_count == 0){
                for(int i=0; i<COUNT_OF_MACRO; i++){
                    if(namtab[i].macroName==NULL)
                        break;

                    isMacro = strcmp(namtab[i].macroName, buffer[ind].command);
                    if(isMacro == 0){
                        //int isSame = -1;

                        //Проверка, что данная строка макровызова не встречалась ранее
                        if(macroBuffer[call_count]==NULL){
                            macroBuffer[call_count] = (char*)malloc(sizeof(char)*50);
                            strcpy(macroBuffer[call_count], buffer[ind].len);
                        }
                        else{
                            int t = 0;
                            while(isprint(buffer[ind].len[t])==0){
                                buffer[ind].len[t] = ' ';
                                t += 1;
                            }

                            isSame = strcmp(buffer[ind].len, macroBuffer[call_count]);
                            call_count += 1;
                            if(t == 0){
                                macroBuffer[call_count] = (char*)malloc(sizeof(char)*50);
                                strcpy(macroBuffer[call_count], buffer[ind].len);
                            }
                        }

                        if(isSame == 0)
                            break;
                        macroExpand(namtab[i], buffer[ind].args);  //Подставляем тело макроса (макрорасширение)
                        is_print = 1;
                        break;
                    }
                }
            }

            if(isSame == 0){
                isSame = -1;
                continue;
            }
            if(is_print == 1)
                continue;
            printf("%s", buffer[ind].len);
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
                ARGTAB = (char*)malloc(sizeof(char)*50);
                arg_count = 0;
                while(buffer[ind].args[i_arg]!='\0' || buffer[ind].args[i_arg]!='\000'){
  
                    if(isalpha(buffer[ind].args[i_arg])){
                        ARGTAB[arg_count] = buffer[ind].args[i_arg];
                        arg_count += 1;
                    }
                    i_arg += 1;
                }
                // Окончание (1)
            }
            else if(strcmp(buffer[ind].command, "mend")==0){
                macro_count -= 1;
                namtab[macro_count].end = macro_ind;
            }

            // Запись тела макроопределения в DEFTAB
            DEFTAB[macro_ind] = (char*)malloc(sizeof(char)*100);
            writeToDeftab(buffer[ind].len, macro_ind);
            macro_ind += 1;    // Переход к следующей строке в DEFTAB
        }

        isMacro = -1;
        isMend = -1;
        ind+=1;
    }

    clearNamtab(namtab, COUNT_OF_MACRO);
    free(ARGTAB);
    ARGTAB = NULL;

    printf("\nHello, World!\n");
}