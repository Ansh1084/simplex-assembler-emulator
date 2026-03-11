        ldc 5
LOOP:   adc -1
        brlz END
        br LOOP
END:    HALT