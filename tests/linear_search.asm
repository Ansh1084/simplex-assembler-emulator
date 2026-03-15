;  Entry Point 
br start

;  DATA 
N:      data 5          ; Number of elements
target: data 45         ; The value we are looking for
result: data -1         ; Final index (-1 means not found)
i:      data 0          ; Loop counter
arr:    data 96
        data 67
        data 45
        data 38
        data 46

; CODE 
start:
; i = 0
ldc 0
ldc i
stnl 0

loop:
; Check if i == N (End of array)
ldc N
ldnl 0
ldc i
ldnl 0
sub             ; A = N - i
brz end         ; If A == 0, end loop (Result stays -1)

; 1. Load arr[i]
ldc arr
ldc i
ldnl 0
add             ; A = address of arr[i]
ldnl 0          ; A = value of arr[i]

; 2. Compare with target
ldc target
ldnl 0          ; A = target, B = arr[i]
sub             ; A = B - A (arr[i] - target)
brz found       ; If A == 0, they are equal!

; 3. i++
ldc i
ldnl 0
adc 1
ldc i
stnl 0

br loop

found:
; Store current i into result
ldc i
ldnl 0          ; A = i
ldc result
stnl 0          ; result = i
HALT            ; Stop early because we found it

end:
HALT            ; Stop after checking everything