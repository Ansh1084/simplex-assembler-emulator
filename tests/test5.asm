; test_loop_labels.asm

ldc 3          ; loop counter
loop: target:  adc -1
brlz end
br loop
end: HALT