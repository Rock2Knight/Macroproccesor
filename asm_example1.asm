      begin 1000h
st:   ldx   b
sum1: macro &x,&y,&z  ;Начало макроопределения
      lda   &y
      add   &z
      sta   &x
      mend            ;Конец макроопределения
      
      sum1  a,b,c      ;Макровызовы
      sum1  d,e,f

a:    resw  2
b:    dw    3
c:    dw    4
d:    resw  5
e:    dw    4
f:    dw    6
      end   st