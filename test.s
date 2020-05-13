.org $600
lda $d01f
and #$07
cmp #$07
bne exit
rts
exit: brk

