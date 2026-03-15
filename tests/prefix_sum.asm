; --- Entry Point ---
br start

; -------- DATA --------
N:      data 5          ; Number of elements
i:      data 0          ; Loop counter
sum:    data 0          ; Running total
addr:   data 0          ; Temporary address holder
arr:    data 10         ; Initial: 10, After: 10
        data 20         ; Initial: 20, After: 30
        data 5          ; Initial: 5,  After: 35
        data 15         ; Initial: 15, After: 50
        data 2          ; Initial: 2,  After: 52

; -------- CODE --------
start:
; i = 0
ldc 0
ldc i
stnl 0

; sum = 0
ldc 0
ldc sum
stnl 0

loop:
; Check if i == N
ldc N
ldnl 0
ldc i
ldnl 0
sub             ; A = N - i
brz end         ; If A == 0, we are done

; 1. Calculate address of arr[i] and store it
ldc arr
ldc i
ldnl 0
add             ; A = address of arr[i]
ldc addr
stnl 0          ; addr = &arr[i]

; 2. sum = sum + arr[i]
ldc sum
ldnl 0          ; A = current sum
ldc addr
ldnl 0          ; A = &arr[i], B = sum
ldnl 0          ; A = value of arr[i], B = sum
add             ; A = sum + arr[i]
ldc sum
stnl 0          ; Update sum variable

; 3. arr[i] = sum
ldc sum
ldnl 0          ; A = value of sum
ldc addr
ldnl 0          ; A = &arr[i], B = sum
stnl 0          ; memory[&arr[i]] = sum

; 4. i++
ldc i
ldnl 0
adc 1
ldc i
stnl 0

br loop

end:
HALT