// Описание макропроцессора c поддержкой условной макрогенерации для машины Бека
#define COUNT_OF_STRINGS 100
#define COUNT_OF_MACRO 50

#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "ctype.h"

#include "structure.h"
#include "objects.h"
#include "settings.h"

const char* cmp[2] = {"eq", "ne"};
int ipa = 0;                           // Индекс переменных периода макрогенерации

extern def DEFTAB[];                          // Таблица макроопределений
extern char** ARGTAB;                         // Таблица аргументов
extern Namtab namtab[];                       // Таблица, хранящая имена фактических макропараметров
extern PeriodArg periodArgs[];                // Таблица параметров периода макрогенерации

extern void assemble(str*);
extern void conditionStrings(str*, int);             // Проставялем флаги условных строк в DEFTAB
extern void printSting(int, int); 

// Инициализация DEFTAB
void initDeftab(){
	for(int i=0; i<COUNT_OF_STRINGS; i++){
		DEFTAB[i].stroka = NULL;
		DEFTAB[i].cond = -1;
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
        if(buffer->len[j] == ';'){			// Если встретили комментарий
			DEFTAB[macro_ind].stroka[j_macro] = '\n';
			break;
		}                                 

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

			DEFTAB[macro_ind].stroka[j_macro] = buffer->len[j];
			isParametr = 1;
			ipa += 1;
			j_macro += 1;
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
				if(isNotParam == 0 && (buffer->len[j+arg_index] == ' ' || buffer->len[j+arg_index] == ',' || buffer->len[j+arg_index] == '\n' || buffer->len[j+arg_index] == '\'')){
					DEFTAB[macro_ind].stroka[j_macro] = (char)(arg_count) + '0';
					j += arg_index;
					isWrite = 1;
					break;
				}
				else if(arg_count != 9)
					isNotParam = 0;
				else
					isNotParam = 2;
			}
			
			// Действия, если параметр не окзался в ARGTAB (ищем соответствие среди параметров периода макрогенерации)
			if(isNotParam = 2){
				int indOfPArg = 0;       // Индекс параметра макроопределения
				int k;               // Индекс строки в параметре макроопределения

				for(indOfPArg=0; indOfPArg<COUNT_OF_MACRO; indOfPArg++){
					if(periodArgs[indOfPArg].name == NULL)
						break;
					k = 0;
					
					while(periodArgs[indOfPArg].name[k] != '\0' && periodArgs[indOfPArg].name[k] != '\000'){
						if(periodArgs[indOfPArg].name[k] != buffer->len[j+k]){
							isNotParam = 3;                                                  // в строке ассемблера
							break;
						}
						k += 1;
					}

					// Записываем код параметра в DEFTAB
					if(isNotParam == 2){
						DEFTAB[macro_ind].stroka[j_macro] = 'x';
						j_macro += 1;
						DEFTAB[macro_ind].stroka[j_macro] = (char)(indOfPArg) + '0';
						j += k;
						isWrite = 1;
						break;
					}
					else
						isNotParam = 2;
				}
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

	conditionStrings(buffer, macro_ind);   // Проставялем флаги условных строк в DEFTAB
}

// Проверка условия в теле макроопределения
int trueCondition(def def_str){
	int i = 0;                         // Номер символа в строке DEFTAB
	int substr_cond = 0;               // Сигнал, что мы читаем условие
	int isParametr = 0;    			   // Флаг макропараметра
	int index = -1;                    // Индекс параметра, проверяемого в условии
	int keyCmp = -1;                   // номер команды сравнения
	int twind = 0;                     // Индекс timeWord
	int paramIf = 0;                   // флаг условия
	int paramInd = 0;                  // Индекс для param
	int isPeriod = 0;                  // Периодическая переменная или параметр макроса
	int val = -1;                      // Значение переменной периода условной макрогенерации

	int result = -1;                   // Результат работы функции

	char* param = (char*)malloc(sizeof(char)*10);      // Буффер для условия в строке
	char* timeWord = (char*)malloc(sizeof(char)*10);   // Буффер для команды сравнения

	// Цикл обхода строки DEFTAB
	while(def_str.stroka[i]!='\0' && def_str.stroka[i]!='\000'){
		if(def_str.stroka[i] == '(' && def_str.cond == 2)          // Проставляем сигнал условия, если наткнулись на '('
			substr_cond = 1;
		else if(def_str.stroka[i] == ')')                    // Обнуляем сигнал условия, если наткнулись на ')'
			substr_cond = 0;
		else if(substr_cond == 1 && def_str.stroka[i]=='&')       // Если нашли '&', то проставляем флаг макропараметра = 1
			isParametr = 1;                                       
		else if(isParametr==1 && isdigit(def_str.stroka[i])){     // Если наткнулись на номер параметра в ARGTAB
		
			for(int k=0; k<10; k++){                       // Ищем нужный параметр из ARGTAB
				if(ARGTAB[k] == NULL)
					break;

				char key = (char)(k)+'0';
				if(key == def_str.stroka[i]){
					index = k;
					break;
				}
			}
			isParametr=2;                                       // isParametr = 2, так проверяем условие дальше
		}
		else if(isParametr==1 && def_str.stroka[i]=='x'){
			index = (int)(def_str.stroka[i+1]-'0');
			isPeriod = 1;
			i += 2;
			isParametr = 2;
			continue;
		}
		else if(isParametr==2 && isalpha(def_str.stroka[i])){    // дальше просто записываем символы команды проверки условия
			timeWord[twind] = def_str.stroka[i];
			twind += 1;
		}
		else if(isParametr==2 && def_str.stroka[i]==' '){        // Ищем нужную команду сравнения
			for(int j=0; j<2; j++){
				if(strcmp(timeWord, cmp[j])==0){
					keyCmp = j;
					break;
				}
			}
		}
		else if(isParametr==2 && def_str.stroka[i]=='\'' && paramIf==0)
			paramIf = 1;                                                    // Условие прочитано
		else if(substr_cond==0 && paramIf==2){
			if(isPeriod == 0){
				// Результат проверки условия
				if(keyCmp == 0 && strcmp(ARGTAB[index], param)==0){             // Если строки равны и у нас проверка на равенство
					result = 1;
					break;
				}
				else if(keyCmp == 1 && strcmp(ARGTAB[index], param)!=0){        // Если строки не равны и у нас проверка на отличие
					result = 1;
					break;
				}
				else{                             // В любом другом случае ставим false
					result = 0;
					break;
				}
			}
			else if(isPeriod == 1){
				if(periodArgs[index].value==val && keyCmp == 0){
					result = 1;
					break;
				}
				else if(periodArgs[index].value!=val && keyCmp == 1){
					result = 1;
					break;
				}
				else{
					result = 0;
					break;
				}
			}
		}
		else if(substr_cond==1 && paramIf == 0 && isdigit(def_str.stroka[i]) && isPeriod == 1){
			val = (int)(def_str.stroka[i]-'0');
			paramIf = 2;
		}
		else if(substr_cond==1 && paramIf == 1){
			if(isprint(def_str.stroka[i]) && def_str.stroka[i]!='\''){
				param[paramInd] = def_str.stroka[i];
				paramInd += 1;
			}
			else if(def_str.stroka[i]=='\''){
				if(param != NULL)
					free(param);
				param = (char*)malloc(sizeof(char)*10);
				paramIf = 2;
			}
		}
		i += 1;
	}

	if(param != NULL){
		free(param);
		param = NULL;
	}

	if(timeWord != NULL){
		free(timeWord);
		timeWord = NULL;
	}

	return result;
}

void macroExpand(Namtab note, char* args){
	char isCond = -1;                            // Флаг условия
	int arg_count = 0;
	int arg_index = 0;
	int i_arg = 0;
	int isPrint = 0;                             // Печатать ли строку DEFTAB
	int isNext = 0;                              // Признак перехода к следующей строке DEFTAB

	if(ARGTAB[arg_count] != NULL){
		free(ARGTAB[arg_count]);
		ARGTAB[arg_count] = (char*)malloc(sizeof(char)*50);
	}

	// Обнуление переменных периода макрогенерации
	for(int i=0; i<ipa; i++){
		if(periodArgs[i].value != -1)
			periodArgs[i].value = -1;
	}

	arg_count = writeToArgtab(args);           // Запись фактических параметров в ARGTAB

	// Подстановка строк макроопределения в код
	for(int i=note.begin+1; i<note.end; i++){
		isNext = 0;

		if(DEFTAB[i].cond == 2){                        // Если встретили IF
			isCond = trueCondition(DEFTAB[i]);          // Проверка условия в теле макроопределния
			continue;
		}
		else if(DEFTAB[i].cond == 3)                    // Если встретили ELSE
			continue;
		else if(DEFTAB[i].cond == 4){					// Если встретили ENDIF
			isCond = -1;
			continue;
		}
		else if(DEFTAB[i].cond == isCond)               // условие строки совпадает с текущим условием, то выводим строку в выходной поток
			isPrint = 1;
		else
			isPrint = 0;

		if(isPrint == 1){
			if(DEFTAB[i].stroka[0] == '&'){
				int index = (int)(DEFTAB[i].stroka[2]-'0');

				int k=3, k1=0;
				int isCom = 0;
				char* tWord = (char*)malloc(sizeof(char)*10);

				while(DEFTAB[i].stroka[k]!='\0' || DEFTAB[i].stroka[k]!='\0'){
				
					if(isalpha(DEFTAB[i].stroka[k])){
						if(isCom == 0){
							isCom = 1;
							tWord[k1] = DEFTAB[i].stroka[k];
							k1 += 1;
						}
						else if(isCom == 1){
							tWord[k1] = DEFTAB[i].stroka[k];
							k1 += 1;
						}
					}
					else if(DEFTAB[i].stroka[k] == ' ' && isCom == 1){
						isCom = 2;
						tWord[k1] = '\0';
					}
					else if(isdigit(DEFTAB[i].stroka[k]) && isCom == 2){
						if(strcmp(tWord, "set")==0){
							// Утсановка значения условной переменной в зависимости от значения аргумента в коде ассемблера
							if(DEFTAB[i].stroka[k] == '1')                       
								periodArgs[index].value = 1;
							else if(DEFTAB[i].stroka[k] == '0')
								periodArgs[index].value = 0;
							isNext = 1;
							break;
						}
					}
					k += 1;
				}

				if(tWord != NULL){
					free(tWord);
					tWord = NULL;
				}

				if(isNext == 1)
					continue;
			}
			else
				printSting(i, arg_count);
		}
	}

	printf("\n");
}