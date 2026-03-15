; --- Entry Point ---
br start        ; Jump over data to the start of the code

; -------- DATA --------
N:      data 5
i:      data 0
j:      data 0
temp1:  data 0
temp2:  data 0
addr1:  data 0    ; Renamed: addr1 instead of addr_1
addr2:  data 0    ; Renamed: addr2 instead of addr_2
arr:    data 94
        data 67
        data 45
        data 32
        data 7

; -------- CODE --------
start:

; i = N-1
ldc N
ldnl 0
adc -1
ldc i
stnl 0

outerloop:
ldc i
ldnl 0
brz end
brlz end

; j = 0
ldc 0
ldc j
stnl 0

innerloop:
ldc i
ldnl 0
ldc j
ldnl 0
sub             ; A = i - j
brz endinner
brlz endinner

; temp1 = arr[j]
ldc arr
ldc j
ldnl 0
add
ldnl 0          ; A = value of arr[j]
ldc temp1
stnl 0          ; Store val in temp1

; temp2 = arr[j+1]
ldc arr
ldc j
ldnl 0
add
adc 1
ldnl 0          ; A = value of arr[j+1]
ldc temp2
stnl 0          ; Store val in temp2

; compare (if temp2 < temp1 then swap)
ldc temp2
ldnl 0          ; A = val2
ldc temp1
ldnl 0          ; A = val1, B = val2
sub             ; A = B - A (val2 - val1)
brlz swap
br noswap

swap:
    ; 1. Calculate & store address of arr[j]
    ldc arr
    ldc j
    ldnl 0
    add             ; A = address of arr[j]
    ldc addr1
    stnl 0          ; addr1 = &arr[j]

    ; 2. Calculate & store address of arr[j+1]
    ldc arr
    ldc j
    ldnl 0
    add
    adc 1           ; A = address of arr[j+1]
    ldc addr2
    stnl 0          ; addr2 = &arr[j+1]

    ; 3. Perform the actual swap
    ; arr[j] = temp2
    ldc temp2
    ldnl 0          ; A = value of temp2
    ldc addr1
    ldnl 0          ; A = &arr[j], B = value of temp2
    stnl 0          ; memory[&arr[j]] = value of temp2

    ; arr[j+1] = temp1
    ldc temp1
    ldnl 0          ; A = value of temp1
    ldc addr2
    ldnl 0          ; A = &arr[j+1], B = value of temp1
    stnl 0          ; memory[&arr[j+1]] = value of temp1

noswap:
; j++
ldc j
ldnl 0
adc 1
ldc j
stnl 0

br innerloop

endinner:
; i--
ldc i
ldnl 0
adc -1
ldc i
stnl 0

br outerloop

end:
HALT