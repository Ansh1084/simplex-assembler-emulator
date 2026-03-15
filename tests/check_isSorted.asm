; --- Entry Point ---
br start

; -------- DATA --------
N:      data 5          ; Size of array
i:      data 0          ; Loop counter
is_srt: data 0          ; Initially 0
arr:    data 10         ; arr[0]
        data 20         ; arr[1]
        data 30         ; arr[2]
        data 40         ; arr[3]
        data 50         ; arr[4]

; -------- CODE --------
start:
; Handle edge case: N < 2 is always sorted
ldc N
ldnl 0
adc -2
brlz set_sorted ; If N is 0 or 1, skip to set result to 1

; i=0
ldc 0
ldc i
stnl 0

loop:
; Check if i==N-1
ldc N
ldnl 0
adc -1          ; A=N-1
ldc i
ldnl 0          ; A=i, B=N-1
sub
brz set_sorted  ; Loop finished without errors -> array is sorted

; 1. Load arr[i]
ldc arr
ldc i
ldnl 0
add
ldnl 0          ; A=arr[i]
ldc 0
adc 0           ; Move A to B (B=arr[i])

; 2. Load arr[i+1]
ldc arr
ldc i
ldnl 0
adc 1           ; A=i+1
add             ; A=address of arr[i+1]
ldnl 0          ; A=arr[i+1], B=arr[i]

; 3. Compare (arr[i]-arr[i+1])
sub             ; A=arr[i]-arr[i+1]
brlz continue   ; If arr[i]<arr[i+1], it's okay
brz continue    ; If arr[i]==arr[i+1], it's okay

; --- Not Sorted Case ---
; Result stays 0, just exit
HALT

continue:
; i++
ldc i
ldnl 0
adc 1
ldc i
stnl 0
br loop

set_sorted:
; If we reach here, the array is sorted
ldc 1
ldc is_srt
stnl 0
HALT