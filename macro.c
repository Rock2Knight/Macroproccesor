// Описание простого макропроцессора
#define COUNT_OF_STRINGS 100
#define COUNT_OF_MACRO 50

#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "ctype.h"

#include "structure.h"

int sizeOfString = 0;                  // Длина строки
int k1, k2, k3, k4, k5;                // Индексы для метки, команды, аргументов и комментария
char Metka, Command, Args, Comment;    // Флаги для метки, команды, аргументов и комментария

char* DEFTAB[COUNT_OF_STRINGS];      // Таблица макроопределений
char** ARGTAB = NULL;                 // Таблица аргументов
Namtab namtab[COUNT_OF_MACRO];       // Таблица, хранящая имена фактических макропараметров

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

        if(j == 0 && isalpha(buffer->len[j])){
			Metka = 1;
		}
		if(Metka==1 && buffer->len[j]==':')
			Metka = 0;
		if(j == 6 && isalpha(buffer->len[j])){
			Metka = 0;
			Command = 1;
		}
		if(Command==1 && buffer->len[j]==' ')
			Command = 0;
		if(j == 12 && (isalpha(buffer->len[j]) || isdigit(buffer->len[j]) || buffer->len[j] == '&')){
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
void writeToDeftab(char* buf_str, int macro_ind){
	int isParametr = 0;
	int j = 0;
	int j_macro = 0;
	int arg_count = 0;
	int arg_index = 0;
	int isWrite = 0;

    while(buf_str[j]!='\0' || buf_str[j]!='\000'){
        if(buf_str[j]==';')
            break;

        if(buf_str[j] == '&')
            isParametr = 1;
        else if(isParametr == 1 && isalpha(buf_str[j])){  // Если мы нашли формальный параметр, ищем его ндекс в ARGTAB
            
			int isParam = 0;
			for(arg_count=0; arg_count<10; arg_count++){
				if(ARGTAB[arg_count]==NULL)
					break;
				arg_index = 0;
				while(ARGTAB[arg_count][arg_index] != '\0' || ARGTAB[arg_count][arg_index] != '\000'){
                	if(ARGTAB[arg_count][arg_index] != buf_str[j+arg_index]){
						isParam = 1;
						break;
					}
					arg_index += 1;
				}

				if(isParam == 0){
					DEFTAB[macro_ind][j_macro] = (char)(arg_count) + '0';
					j += arg_index;
					isWrite = 1;
					break;
				}
				else
					isParam = 0;
			}

            isParametr = 0;
			j_macro += 1;
			if(isWrite == 0)
            	j += 1;
			else
				isWrite = 0;
            continue;
        }
        DEFTAB[macro_ind][j_macro] = buf_str[j];
        j += 1;
		j_macro+=1;
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
		while(DEFTAB[i][j]!='\0' || DEFTAB[i][j]!='\000'){
			if(DEFTAB[i][j]=='&')
				isParametr = 1;
			else if(isParametr==1 && isdigit(DEFTAB[i][j])){
				int index = -1;
				for(int k=0; k<arg_count; k++){
					char key = (char)(k)+'0';
					if(key == DEFTAB[i][j]){
						index = k;
						break;
					}
				}
				printf("%s", ARGTAB[index]);
				isParametr=0;
			}
			else
				printf("%c", DEFTAB[i][j]);
			j+=1;
		}
	}

	printf("\n");
}