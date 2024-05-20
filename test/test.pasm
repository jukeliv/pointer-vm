# Truth-Machine (https://esolangs.org/wiki/Truth-machine)

# Read user input
mov 0x01, 0x00
sys

popb 0x02

# Check if it is '1'
mov '1', 0x00
eq 0x04, 0x02, 0x00

if 0x04
hlt # If not, just exit

# If it is '1', repeat infinitely
$loop

# print '1'
null 0x00
pushb 0x02
sys

jmp $loop