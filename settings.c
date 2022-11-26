#define COUNT_OF_STRINGS 100
#define COUNT_OF_MACRO 50

#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "ctype.h"

#include "structure.h"
#include "objects.h"

int sizeOfString = 0;                  // Длина строки
int k1, k2, k3, k4, k5;                // Индексы для метки, команды, аргументов и комментария
char Metka, Command, Args, Comment;    // Флаги для метки, команды, аргументов и комментария

extern def DEFTAB[];                          // Таблица макроопределений
extern char** ARGTAB;                         // Таблица аргументов
extern Namtab namtab[];                       // Таблица, хранящая имена фактических макропараметров
extern PeriodArg periodArgs[];                // Таблица параметров периода макрогенерации

// Функция разбора строки программы на ассемблере на составляющие
void assemble(str* buffer)
{
	k1 = 0;
	k2 = 0;
	k3 = 0;
	k4 = 0;
	k5 = 0;

	Metka = 0;
	Command = 0;
	Args = 0;
	Comment = 0;

	sizeOfString = sizeof(buffer->len);

    // Парсинг строки ассемблерного кода
	for(int j = 0; j < sizeOfString || buffer->len[j]!='\n' || buffer->len[j]!='\0'; j++)
	{
		if(buffer->len[j]=='\0' || buffer->len[j]=='\000')
			break;

        if(j == 0 && (isalpha(buffer->len[j]) || buffer->len[j] == '&')){
			Metka = 1;
		}
		if(Metka==1 && buffer->len[j]==':')
			Metka = 0;
		if(j == 8 && (isalpha(buffer->len[j]) || buffer->len[j]=='+')){
			Metka = 0;
			Command = 1;
		}
		if(Command==1 && buffer->len[j]==' ')
			Command = 0;
		if(j == 15 && (isalpha(buffer->len[j]) || isdigit(buffer->len[j]) || buffer->len[j] == '&')){
			Command = 0;
			Args = 1;
		}
		if(Args==1 && buffer->len[j]==' ')
			Args = 0;
		if(buffer->len[j]==';')
			Comment = 1;
		if(Comment==1 && buffer->len[j]!=';')
			Comment = 2;
		
		if(buffer->len[j]!='\n')
		{
			if(Metka==1){buffer->metka[k1]=buffer->len[j]; k1+=1;}
			else if(Command==1){buffer->command[k2]=buffer->len[j]; k2+=1;}
			else if(Args==1){buffer->args[k3]=buffer->len[j]; k3+=1;}
			else if(Comment==2){buffer->comment[k4]=buffer->len[j]; k4+=1;}
		}
		else
			break;
	}
}

// Проставялем флаги условных строк в DEFTAB
void conditionStrings(str* buffer, int macro_ind){
	if(macro_ind != 0){
		if(DEFTAB[macro_ind-1].cond == 2 || DEFTAB[macro_ind-1].cond == 1){
			if(strcmp(buffer->command, "else") == 0)
				DEFTAB[macro_ind].cond = 3;
			else if(strcmp(buffer->command, "endif") == 0)
				DEFTAB[macro_ind].cond = 4;
			else
				DEFTAB[macro_ind].cond = 1;
		}
		else if(DEFTAB[macro_ind-1].cond == 3 || DEFTAB[macro_ind-1].cond == 1){
			if(strcmp(buffer->command, "endif") == 0)
				DEFTAB[macro_ind].cond = 4;
			else
				DEFTAB[macro_ind].cond = 0;
		}
	}
}

// Печать строки макроопределения
void printSting(int def_ind, int arg_count){
	int j=0;
	int isParametr = 0;
	while(DEFTAB[def_ind].stroka[j]!='\0' || DEFTAB[def_ind].stroka[j]!='\000'){
		if(DEFTAB[def_ind].stroka[j]=='&')
			isParametr = 1;
		else if(isParametr==1 && isdigit(DEFTAB[def_ind].stroka[j])){
			int index = -1;
			for(int k=0; k<arg_count; k++){
				char key = (char)(k)+'0';
				if(key == DEFTAB[def_ind].stroka[j]){
					index = k;
					break;
				}
			}
			printf("%s", ARGTAB[index]);
			isParametr=0;
		}
		else
			printf("%c", DEFTAB[def_ind].stroka[j]);
		j+=1;
	}
}

// Запись фактических параметров в ARGTAB
int writeToArgtab(char* args){
    int arg_count = 0;
    int arg_index = 0;
    int i_arg = 0;
    
    while(args[i_arg]!='\0' || args[i_arg]!='\000'){

		//В случае обнаружения разделителя, переходим к следующему элементу в ARGTAB
        if(args[i_arg] == ','){
            arg_count += 1;
			free(ARGTAB[arg_count]);
            ARGTAB[arg_count] = (char*)malloc(sizeof(char)*50);
        	arg_index = 0;
            i_arg += 1;
            continue;
        }
  
        if(isalpha(args[i_arg]) || isdigit(args[i_arg])){
            ARGTAB[arg_count][arg_index] = args[i_arg];
            arg_index += 1;
        }
            
        i_arg += 1;
	}

	arg_count += 1;
    return arg_count;
}