
# Lab 3

## Exercise 1

### Q1

1. .data: a directive to declare read/write data section. static data, global variables are stored there.
2. .word: a directive for 4-byte space in static data.
3. .text: a directive to declare read-only data section. Code, constant strings are stored there.

### Q2

1. Output: 34
2. The 9th number of Fibonacci sequence

### Q3

`t3`: `0x10000010` (Step to the line `lw t3, 0(t3)`)

### Q4

Follow the following steps:

1. Step to line `fib: beq t3, x0, finish`
2. Manually change the register `t3` to `0x0000000D`
3. Run through the program
4. The output should be 233

## Exercise 2

### Q1
1. k: `t0`
2. source: `t1`, dest: `t2`
3. The assembly code:
```
loop:
	slli t3, t0, 2
	add t4, t1, t3
	lw t5, 0(t4)
	beq t5, x0, exit
	add t6, t2, t3
	sw t5, 0(t6)
	addi t0, t0, 1
	jal x0, loop
exit:
```
4. A C perspective of pointer in assembly:
    1. All pointers could be stored in a register, for their sizes are exactly 4 bytes on 32-bit system.
    2. Pointers in assembly are just like `char*`/`unsigned char*` in C. Adding one to a pointer (i.e. `p+1`) means moving one byte forward(i.e. `p+1`).
    3. Pointers in assembly don't have type, which means it can be processed as any types. For example, you can read a memory block four bytes by four bytes, if you want to interpret them as an integer array.

5. [Code](lab3_ex2_assembly.s)

## Exercise 3

[Code](factorial.s)
