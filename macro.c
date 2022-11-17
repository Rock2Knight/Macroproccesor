// Описание макропроцессора c поддержкой условной макрогенерации для машины Бека
#define COUNT_OF_STRINGS 100
#define COUNT_OF_MACRO 50

#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "ctype.h"

#include "structure.h"

int sizeOfString = 0;                  // Длина строки
int k1, k2, k3, k4, k5;                // Индексы для метки, команды, аргументов и комментария
int ipa = 0;                           // Индекс переменных периода макрогенерации
char Metka, Command, Args, Comment;    // Флаги для метки, команды, аргументов и комментария

def DEFTAB[COUNT_OF_STRINGS];          // Таблица макроопределений
char** ARGTAB = NULL;                  // Таблица аргументов
Namtab namtab[COUNT_OF_MACRO];         // Таблица, хранящая имена фактических макропараметров
PeriodArg periodArgs[COUNT_OF_MACRO];  // Таблица параметров периода макрогенерации

void initDeftab(){
	for(int i=0; i<COUNT_OF_STRINGS; i++){
		DEFTAB[i].stroka = NULL;
		DEFTAB[i].cond = -1;
	}
}

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


// Очистка таблицы макроимен
void clearNamtab(Namtab* NAMTAB, int size){
    for(int i=0; i<size; i++){
        if(NAMTAB[i].macroName != NULL){
            free(NAMTAB[i].macroName);
            NAMTAB[i].macroName = NULL;
        }
    }
}

// Запись строки макроопределения в DEFTAB
void writeToDeftab(str* buffer, int macro_ind){
	int isParametr = 0;                           // Флаг макропараметра
	int j = 0;                                    // Номер символа в строке кода ассемблера
	int j_macro = 0;                              // Номер символа в строке DEFTAB
	int arg_count = 0;                            // Номер параметра в ARGTAB
	int arg_index = 0;                            // Индекс символа в параметре ARGTAB
	int isWrite = 0;                              // Флаг записи номера параметра в DEFTAB

	// Проставялем флаги строк условия в DEFTAB
	if(strcmp(buffer->command, "if") == 0)
		DEFTAB[macro_ind].cond = 2;
	else if(strcmp(buffer->command, "else")==0)
		DEFTAB[macro_ind].cond = 3;
	else if(strcmp(buffer->command, "endif")==0)
		DEFTAB[macro_ind].cond = 4;

	// Цикл обхода строки кода ассемблера
    while(buffer->len[j] != '\0' || buffer->len[j] != '\000'){
        if(buffer->len[j] == ';')                                 // Если встретили комментарий
            break;

		// Действия в случае если мы наткнулись на условную переменную
		if(buffer->len[j] == '&' && j == 0){
			periodArgs[ipa].name = (char*)malloc(sizeof(char)*50);
			int lenOfMetka = strlen(buffer->metka);
			strncpy(periodArgs[ipa].name, buffer->metka+1, lenOfMetka);     // Копируем часть метки, удаляя символ '&'

			// Утсановка значения условной переменной в зависимости от значения аргумента в коде ассемблера
			if(buffer->args[0] == '1')                       
				periodArgs[ipa].value = 1;
			else if(buffer->args[0] == '0')
				periodArgs[ipa].value = 0;

			ipa += 1;
			j += 1;
			continue;
		}
		
        if(buffer->len[j] == '&' && j!=0)        // При нахождении символа формального параметра ставим его флаг True
            isParametr = 1;
        else if(isParametr == 1 && isalpha(buffer->len[j])){  // Если мы нашли формальный параметр, ищем его ндекс в ARGTAB
            
			int isNotParam = 0;                              // Флаг соответствия параметра из строки кода ассемблера параметру в ARGTAB
			for(arg_count=0; arg_count<10; arg_count++){
				if(ARGTAB[arg_count]==NULL)                  // Параметра ARGTAB нет !!
					break;

				arg_index = 0;

				while(ARGTAB[arg_count][arg_index] != '\0' || ARGTAB[arg_count][arg_index] != '\000'){  // Считываем параметр ARGTAB
                	if(ARGTAB[arg_count][arg_index] != buffer->len[j+arg_index]){                           // и проверяем его на соответсвие параметру
						isNotParam = 1;                                                  // в строке ассемблера
						break;                             
					}
					arg_index += 1;
				}

				// Записываем код параметра в DEFTAB
				if(isNotParam == 0){
					DEFTAB[macro_ind].stroka[j_macro] = (char)(arg_count) + '0';
					j += arg_index;
					isWrite = 1;
					break;
				}
				else
					isNotParam = 0;
			}

            isParametr = 0;
			j_macro += 1;

			// Делаем итерацию, если в строке тела макроопрделения не найден макропараметр
			if(isWrite == 0)
            	j += 1;
			else
				isWrite = 0;
            continue;
        }

        DEFTAB[macro_ind].stroka[j_macro] = buffer->len[j];
        j += 1;
		j_macro+=1;
    }

	// Проставялем флаги условных строк в DEFTAB
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

void macroExpand(Namtab note, char* args){
	int arg_count = 0;
	int arg_index = 0;
	int i_arg = 0;
	int isParametr = 0;

	if(ARGTAB[arg_count] != NULL){
		free(ARGTAB[arg_count]);
		ARGTAB[arg_count] = (char*)malloc(sizeof(char)*50);
	}

	// Запись фактических параметров в ARGTAB
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
  
        if(isalpha(args[i_arg])){
            ARGTAB[arg_count][arg_index] = args[i_arg];
            arg_index += 1;
        }
            
        i_arg += 1;
	}

	arg_count += 1;

	for(int i=note.begin+1; i<note.end; i++){
		int j=0;
		while(DEFTAB[i].stroka[j]!='\0' || DEFTAB[i].stroka[j]!='\000'){
			if(DEFTAB[i].stroka[j]=='&')
				isParametr = 1;
			else if(isParametr==1 && isdigit(DEFTAB[i].stroka[j])){
				int index = -1;
				for(int k=0; k<arg_count; k++){
					char key = (char)(k)+'0';
					if(key == DEFTAB[i].stroka[j]){
						index = k;
						break;
					}
				}
				printf("%s", ARGTAB[index]);
				isParametr=0;
			}
			else
				printf("%c", DEFTAB[i].stroka[j]);
			j+=1;
		}
	}

	printf("\n");
}