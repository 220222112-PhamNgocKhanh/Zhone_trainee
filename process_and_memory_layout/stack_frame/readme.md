## This foldrer I will explain the process inside stack when we run the `stack_frame` programe

Firstly, we need to build the program by using the command: 

- `gcc -o stack_frame.s -S stack_frame.c`
- `gcc -o stack_frame.s -S stack_frame.c`.

 Now we have both the execute file and assembly file.

Secondly, I will setup the gdb tool to see all register when the program run. Use `gdb ./stack_frame` to run the program in debug mode. Next, run all command in gdb terminal: 
- `b main`: This command will set a breakpoint in main function
- `display/x $rcx`: Display the rcx register 

 - `display/x $rdx` : Display the rdx register 

- `display/x $rsi` : Display the rsi register 

- `display/x $rdi`: Display the rdi register 

 - `display/x $rbp` : Display the rbp register 

- `display/x $rsp` : Display the rsp register 

- `display/x $r8`: Display the r8 register 

 - `display/x $r9`  : Display the r9 register 

- `display/i $pc` : Display the program instruction

Then run the `run` command. Use `si` to continue to the next instruction.

Now, stay focused on the assembly file and the values of all registers.

In the first step, after running the program, all registers have trash values, while `rbp` and `rsp` have the same address value. Continue running the `si` command. You can see that the `rsp` address decreases when data is pushed onto the stack.


After running the instructions: 
```
	pushq	$8
	movl	$7, %r9d
	movl	$6, %r8d
	movl	$5, %ecx
	movl	$4, %edx
	movl	$3, %esi
	movl	$2, %edi
```

That mean 6 first arguments stored in registers, the 7th argument stored in the stack. You also see the value of 6 register will change: 
```
1: /x $rcx = 0x5
2: /x $rdx = 0x4
3: /x $rsi = 0x3
4: /x $rdi = 0x2
5: /x $rbp = 0x7fffffffde50
6: /x $rsp = 0x7fffffffde38
7: /x $r8 = 0x6
8: /x $r9 = 0x7
```
Can you also see that the value of the `rsp` register does not change? This means that the first six arguments are not stored on the stack.

Now, when calling the `call calculate` command, it means that we start the `calculate` function's stack frame.

Now we see the following instructions:

```
pushq   %rbp
movq    %rsp, %rbp
```

The `pushq %rbp` instruction creates a new data area on the stack for the new `rbp` register. It then stores the old `rbp` address as its value.

The `movq %rsp, %rbp` instruction changes the value of `rbp` to the current `rsp` value. This means that `rbp` now points to the new address created by the previous instruction.

```
1: /x $rcx = 0x5
2: /x $rdx = 0x4
3: /x $rsi = 0x3
4: /x $rdi = 0x2
5: /x $rbp = 0x7fffffffde28
6: /x $rsp = 0x7fffffffde28
7: /x $r8 = 0x6
8: /x $r9 = 0x7
9: x/i $pc
```

The `rbp` of calculate function is `0x7fffffffde28`. After then, we can see the instructions

```
	subq	$40, %rsp
	movl	%edi, -20(%rbp)
	movl	%esi, -24(%rbp)
	movl	%edx, -28(%rbp)
	movl	%ecx, -32(%rbp)
	movl	%r8d, -36(%rbp)
	movl	%r9d, -40(%rbp)
```

As we can see, all value of 6 registers will be passed again to the stack, however, the location is not same with the 7th argument. They stored in the location same local variable.

The stack frame of `calculate` function quite completed. With others function, the process same so i will not write again. 


Now continue to momment when the leaf function completed, that mean when run the instruction: `pop $rbp`, the value of `rbp` register will receive the value at current `rbg` address. 


We can see the example: 

Before calling the `multiply_add` function: 

```
5: /x $rbp = 0x7fffffffde28
6: /x $rsp = 0x7fffffffde00
```

After the `multiply_add1` function complete: 
```
=> 0x555555555172 <multiply_add+41>:	pop    %rbp
(gdb) si
0x0000555555555173 in multiply_add ()
1: /x $rcx = 0x3
2: /x $rdx = 0x6
3: /x $rsi = 0x3
4: /x $rdi = 0x2
5: /x $rbp = 0x7fffffffde28
6: /x $rsp = 0x7fffffffddf8
7: /x $r8 = 0x6
8: /x $r9 = 0x7
9: x/i $pc
=> 0x555555555173 <multiply_add+42>:	ret    
(gdb) si
0x00005555555551a6 in calculate ()
1: /x $rcx = 0x3
2: /x $rdx = 0x6
3: /x $rsi = 0x3
4: /x $rdi = 0x2
5: /x $rbp = 0x7fffffffde28
6: /x $rsp = 0x7fffffffde00
7: /x $r8 = 0x6
8: /x $r9 = 0x7
9: x/i $pc
```

So we can see, after the `pop` and `ret` instructions, the `rbp` and `rsp` comeback the address of the `calculate` function stack frame.