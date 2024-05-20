$label

# Print 'E' to the stdout
null 0x00
mov 'E', 0x02
pushb 0x02

sys

jmp $label