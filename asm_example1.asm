      begin 1000h
st:   ldx   b
sum1: macro &paul,&curt,&chester  ;Начало макроопределения
      lda   &curt
      add   &chester
      sta   &paul
      mend            ;Конец макроопределения
      
      sum1  a,b,c      ;Макровызовы
      sum1  aa,bb,cc

a:    resw  2
b:    dw    3
c:    dw    4
aa:   resw  5
bb:   dw    4
cc:   dw    6
      end   st