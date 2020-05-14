.org $600    ; start address for check-exit-condition routine
lda $d01f    ; load console button status
and #$07     ; start-select-option
cmp #$07
bne exit     ; if any of the console buttons are pressed, then jump to exit
rts          ; return
exit: brk    ; break to debugger

