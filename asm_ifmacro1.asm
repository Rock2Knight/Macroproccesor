        begin  1000h

rdbuff: macro  &indev,&bufadr,&reclth,&eor,&maxlth
        if     (&eor ne '')
&eorck: set    1
        endif
        clear  x                                     
        clear  a                                     
        if     (&eorck eq 1)
        ldch   =x'&eor'                              
        rmo    a,s
        endif

        if     (maxlth eq '')
        +ldt   #4096            
        else
        +ldt   #&maxlth         
        endif

loop:   td     =x'&indev'       
        jeq    loop             
        rd     =x'&indev'       
        if     (&eorck eq 1)    
        compr  a,s              
        jeq    exit             
        endif
        stch   &bufadr,x
        tixr   t                
        jlt    loop             

exit:   stx    &reclth          
        mend

        rdbuff f3,buf,recl,04,2048