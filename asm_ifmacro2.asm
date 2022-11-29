        begin  1000h

rdbuff: macro  &indev,&bufadr,&reclth,&eor,&maxlth
        if     (&eor ne '')
&eorck: set    1
        endif
        clear  x                ;Очистка счетчика цикла
        clear  a                ;Очистка сумматора
        if     (&eorck eq 1)
        ldch   =x'&eor'         ;Установка сивола eor
        rmo    a,s
        endif

        if     (&maxlth eq '')
        +ldt   4096            ;Максимальная длина записи 4096
        else
        +ldt   &maxlth         ;Установка максимальной длины записи
        endif

loop:   td     =x'&indev'       ;Проверка готовности устройства
        jeq    loop             ;Цикл ожидания готовности
        rd     =x'&indev'       ;Чтение сивола в регистр А
        if     (&eorck eq 1)    
        compr  a,s              ;Проверка на конец записи
        jeq    exit             ;Выход из цикла, если конец записи
        endif
        stch   &bufadr,x
        tixr   t                ;Цикл до достижения
        jlt    loop             ;максимальной длины записи

exit:   stx    &reclth          ;сохранение длины записи
        mend

        rdbuff 0e,buffer,length,,80