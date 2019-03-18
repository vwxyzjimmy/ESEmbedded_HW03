HW03
===
This is the hw03 sample. Please follow the steps below.

# Build the Sample Program

1. Fork this repo to your own github account.

2. Clone the repo that you just forked.

3. Under the hw03 dir, use:

	* `make` to build.

	* `make clean` to clean the ouput files.

4. Extract `gnu-mcu-eclipse-qemu.zip` into hw03 dir. Under the path of hw03, start emulation with `make qemu`.

	See [Lecture 02 ─ Emulation with QEMU] for more details.

5. The sample is a minimal program for ARM Cortex-M4 devices, which enters `while(1);` after reset. Use gdb to get more details.

	See [ESEmbedded_HW02_Example] for knowing how to do the observation and how to use markdown for taking notes.

# Build Your Own Program

1. Edit main.c.

2. Make and run like the steps above.

3. Please avoid using hardware dependent C Standard library functions like `printf`, `malloc`, etc.

# HW03 Requirements

1. How do C functions pass and return parameters? Please describe the related standard used by the Application Binary Interface (ABI) for the ARM architecture.

2. Modify main.c to observe what you found.

3. You have to state how you designed the observation (code), and how you performed it.

	Just like how you did in HW02.

3. If there are any official data that define the rules, you can also use them as references.

4. Push your repo to your github. (Use .gitignore to exclude the output files like object files or executable files and the qemu bin folder)

[Lecture 02 ─ Emulation with QEMU]: http://www.nc.es.ncku.edu.tw/course/embedded/02/#Emulation-with-QEMU
[ESEmbedded_HW02_Example]: https://github.com/vwxyzjimmy/ESEmbedded_HW02_Example

--------------------

- [ ] **If you volunteer to give the presentation next week, check this.**

--------------------

**★★★ Please take your note here ★★★**

HW03
===
## 1. 實驗題目
How do C functions pass and return parameters? Please describe the related standard used by the Application Binary Interface (ABI) for the ARM architecture.
## 2. 實驗步驟
1. Design a function pass some parameters.
`pass_int_function` pass five integer parameters and return one integer,
`pass_float` pass a floating parameter and return that.
`pass_uint64_t` pass a 64bit parameter and return that.
Using objdump to observe the mechanism that how the parameter pass to function and how the parameter return.

`main.c`
```c
#include <stdio.h>
int int_val1 = 1, int_val2 = 2, int_val3 = 3, int_val4 = 4, int_val5 = 5;
float float_val = 22.22;
uint64_t t = 33;
int pass_int_function(int int_val1, int int_val2, int int_val3, int int_val4, int int_val5){
	return (int_val1 + int_val2 + int_val3 + int_val4 + int_val5);
}

float pass_float(float float_val){
	return (float_val);
}
uint64_t pass_uint64_t(uint64_t t){
	return (t);
}
void reset_handler(void)
{
	int temp1;
	float temp2;
	uint64_t temp3;
	temp1 = pass_int_function(int_val1, int_val2, int_val3, int_val4, int_val5);
	temp2 = pass_float(float_val);
	temp3 = pass_uint64_t(t);
	while (1)
		;
}
```

2. The passing parameters `int_val1` to `int_val5`, `float_val`, `t` are saved in `.data` section and we can notice that their address are saved in the end of `.mytext` section from `0xa4` to `0xbc`.

`.data`
```assembly
Disassembly of section .data:

000000c0 <int_val1>:
  c0:	00000001 	andeq	r0, r0, r1

000000c4 <int_val2>:
  c4:	00000002 	andeq	r0, r0, r2

000000c8 <int_val3>:
  c8:	00000003 	andeq	r0, r0, r3

000000cc <int_val4>:
  cc:	00000004 	andeq	r0, r0, r4

000000d0 <int_val5>:
  d0:	00000005 	andeq	r0, r0, r5

000000d4 <float_val>:
  d4:	41b1c28f 			; <UNDEFINED> instruction: 0x41b1c28f

000000d8 <t>:
  d8:	00000021 	andeq	r0, r0, r1, lsr #32
  dc:	00000000 	andeq	r0, r0, r0
```
3. From `0x66` to `0x7e`, the program load the parameters' address into `r3` by `pc` with offset `#60`, and load the value into `r0`、`r1`、`r2`、`r4`、`r3`, then store `r3` into the end of stack and move `r4` to `r3`, so the five parameters are respectively saved in `r0`、`r1`、`r2`、`r3`、end of stack.

`reset_handler`
```assembly
  66:	4b0f      	ldr	r3, [pc, #60]	; (a4 <reset_handler+0x44>)
  68:	6818      	ldr	r0, [r3, #0]
  6a:	4b0f      	ldr	r3, [pc, #60]	; (a8 <reset_handler+0x48>)
  6c:	6819      	ldr	r1, [r3, #0]
  6e:	4b0f      	ldr	r3, [pc, #60]	; (ac <reset_handler+0x4c>)
  70:	681a      	ldr	r2, [r3, #0]
  72:	4b0f      	ldr	r3, [pc, #60]	; (b0 <reset_handler+0x50>)
  74:	681c      	ldr	r4, [r3, #0]
  76:	4b0f      	ldr	r3, [pc, #60]	; (b4 <reset_handler+0x54>)
  78:	681b      	ldr	r3, [r3, #0]
  7a:	9300      	str	r3, [sp, #0]
  7c:	4623      	mov	r3, r4
  7e:	f7ff ffc3 	bl	8 <pass_int_function>
  82:	60f8      	str	r0, [r7, #12]
  84:	4b0c      	ldr	r3, [pc, #48]	; (b8 <reset_handler+0x58>)
  86:	681b      	ldr	r3, [r3, #0]
  88:	4618      	mov	r0, r3
  8a:	f7ff ffd2 	bl	32 <pass_float>
  8e:	60b8      	str	r0, [r7, #8]
  90:	4b0a      	ldr	r3, [pc, #40]	; (bc <reset_handler+0x5c>)
  92:	e9d3 3400 	ldrd	r3, r4, [r3]
  96:	4618      	mov	r0, r3
  98:	4621      	mov	r1, r4
  9a:	f7ff ffd4 	bl	46 <pass_uint64_t>
  9e:	e9c7 0100 	strd	r0, r1, [r7]
  a2:	e7fe      	b.n	a2 <reset_handler+0x42>
  a4:	000000c0 	andeq	r0, r0, r0, asr #1
  a8:	000000c4 	andeq	r0, r0, r4, asr #1
  ac:	000000c8 	andeq	r0, r0, r8, asr #1
  b0:	000000cc 	andeq	r0, r0, ip, asr #1
  b4:	000000d0 	ldrdeq	r0, [r0], -r0	; <UNPREDICTABLE>
  b8:	000000d4 	ldrdeq	r0, [r0], -r4
  bc:	000000d8 	ldrdeq	r0, [r0], -r8
```

4. In `pass_int_function`, the program stroe `r0` to `r3` (four parameters) into stack and load them from stack when operating the parameters.
In `0x24`,the fifth parameter is store in stack before `pass_int_function`, so the program load that from stack directly.
After sum five parameters using `r3` and `r2`,the program move the answer `r3` to `r0` and recover the stack pointer then branch back to `reset_handler`.
In `reset_handler` `0x82`，the return value `r0` is saved to stack(local variable) so  the function return the value by `r0`.
From objdump,we can notice that the parameters are passed by `r0`-`r3` and stack,the return value is passed by `r0`.

`pass_int_function`
```assembly
00000008 <pass_int_function>:
   8:	b480      	push	{r7}
   a:	b085      	sub	sp, #20
   c:	af00      	add	r7, sp, #0
   e:	60f8      	str	r0, [r7, #12]
  10:	60b9      	str	r1, [r7, #8]
  12:	607a      	str	r2, [r7, #4]
  14:	603b      	str	r3, [r7, #0]
  16:	68fa      	ldr	r2, [r7, #12]
  18:	68bb      	ldr	r3, [r7, #8]
  1a:	441a      	add	r2, r3
  1c:	687b      	ldr	r3, [r7, #4]
  1e:	441a      	add	r2, r3
  20:	683b      	ldr	r3, [r7, #0]
  22:	441a      	add	r2, r3
  24:	69bb      	ldr	r3, [r7, #24]
  26:	4413      	add	r3, r2
  28:	4618      	mov	r0, r3
  2a:	3714      	adds	r7, #20
  2c:	46bd      	mov	sp, r7
  2e:	bc80      	pop	{r7}
  30:	4770      	bx	lr
```

5. If we pass a floating parameter, it excutiing in the same mechanism but the value is \<UNPREDICTABLE> in `.data` section.

`reset_handler`
```assembly
  84:	4b0c      	ldr	r3, [pc, #48]	; (b8 <reset_handler+0x58>)
  86:	681b      	ldr	r3, [r3, #0]
  88:	4618      	mov	r0, r3
  8a:	f7ff ffd2 	bl	32 <pass_float>
  8e:	60b8      	str	r0, [r7, #8]
```
`pass_float`
```assembly
00000032 <pass_float>:
  32:	b480      	push	{r7}
  34:	b083      	sub	sp, #12
  36:	af00      	add	r7, sp, #0
  38:	6078      	str	r0, [r7, #4]
  3a:	687b      	ldr	r3, [r7, #4]
  3c:	4618      	mov	r0, r3
  3e:	370c      	adds	r7, #12
  40:	46bd      	mov	sp, r7
  42:	bc80      	pop	{r7}
  44:	4770      	bx	lr
```
`.data`
```assembly
000000d4 <float_val>:
  d4:	41b1c28f 			; <UNDEFINED> instruction: 0x41b1c28f
```
6. If we pass a 64bit parameter, we can notice that the program split it into two register `r0`、`r1`.

`reset_handler`
```assembly
  90:	4b0a      	ldr	r3, [pc, #40]	; (bc <reset_handler+0x5c>)
  92:	e9d3 3400 	ldrd	r3, r4, [r3]
  96:	4618      	mov	r0, r3
  98:	4621      	mov	r1, r4
  9a:	f7ff ffd4 	bl	46 <pass_uint64_t>
  9e:	e9c7 0100 	strd	r0, r1, [r7]
```
In the function `pass_uint64_t`,the program get the 64bit parameter by saving into stack and return that by split the 64bit value into `r0`、`r1`.

`pass_uint64_t`
```assembly
00000046 <pass_uint64_t>:
  46:	b490      	push	{r4, r7}
  48:	b082      	sub	sp, #8
  4a:	af00      	add	r7, sp, #0
  4c:	e9c7 0100 	strd	r0, r1, [r7]
  50:	e9d7 3400 	ldrd	r3, r4, [r7]
  54:	4618      	mov	r0, r3
  56:	4621      	mov	r1, r4
  58:	3708      	adds	r7, #8
  5a:	46bd      	mov	sp, r7
  5c:	bc90      	pop	{r4, r7}
  5e:	4770      	bx	lr
```
---
* The full objdump

`objudmp`
```assembly
Disassembly of section .mytext:

00000000 <pass_int_function-0x8>:
   0:	20000100 	andcs	r0, r0, r0, lsl #2
   4:	00000061 	andeq	r0, r0, r1, rrx

00000008 <pass_int_function>:
   8:	b480      	push	{r7}
   a:	b085      	sub	sp, #20
   c:	af00      	add	r7, sp, #0
   e:	60f8      	str	r0, [r7, #12]
  10:	60b9      	str	r1, [r7, #8]
  12:	607a      	str	r2, [r7, #4]
  14:	603b      	str	r3, [r7, #0]
  16:	68fa      	ldr	r2, [r7, #12]
  18:	68bb      	ldr	r3, [r7, #8]
  1a:	441a      	add	r2, r3
  1c:	687b      	ldr	r3, [r7, #4]
  1e:	441a      	add	r2, r3
  20:	683b      	ldr	r3, [r7, #0]
  22:	441a      	add	r2, r3
  24:	69bb      	ldr	r3, [r7, #24]
  26:	4413      	add	r3, r2
  28:	4618      	mov	r0, r3
  2a:	3714      	adds	r7, #20
  2c:	46bd      	mov	sp, r7
  2e:	bc80      	pop	{r7}
  30:	4770      	bx	lr

00000032 <pass_float>:
  32:	b480      	push	{r7}
  34:	b083      	sub	sp, #12
  36:	af00      	add	r7, sp, #0
  38:	6078      	str	r0, [r7, #4]
  3a:	687b      	ldr	r3, [r7, #4]
  3c:	4618      	mov	r0, r3
  3e:	370c      	adds	r7, #12
  40:	46bd      	mov	sp, r7
  42:	bc80      	pop	{r7}
  44:	4770      	bx	lr

00000046 <pass_uint64_t>:
  46:	b490      	push	{r4, r7}
  48:	b082      	sub	sp, #8
  4a:	af00      	add	r7, sp, #0
  4c:	e9c7 0100 	strd	r0, r1, [r7]
  50:	e9d7 3400 	ldrd	r3, r4, [r7]
  54:	4618      	mov	r0, r3
  56:	4621      	mov	r1, r4
  58:	3708      	adds	r7, #8
  5a:	46bd      	mov	sp, r7
  5c:	bc90      	pop	{r4, r7}
  5e:	4770      	bx	lr

00000060 <reset_handler>:
  60:	b590      	push	{r4, r7, lr}
  62:	b087      	sub	sp, #28
  64:	af02      	add	r7, sp, #8
  66:	4b0f      	ldr	r3, [pc, #60]	; (a4 <reset_handler+0x44>)
  68:	6818      	ldr	r0, [r3, #0]
  6a:	4b0f      	ldr	r3, [pc, #60]	; (a8 <reset_handler+0x48>)
  6c:	6819      	ldr	r1, [r3, #0]
  6e:	4b0f      	ldr	r3, [pc, #60]	; (ac <reset_handler+0x4c>)
  70:	681a      	ldr	r2, [r3, #0]
  72:	4b0f      	ldr	r3, [pc, #60]	; (b0 <reset_handler+0x50>)
  74:	681c      	ldr	r4, [r3, #0]
  76:	4b0f      	ldr	r3, [pc, #60]	; (b4 <reset_handler+0x54>)
  78:	681b      	ldr	r3, [r3, #0]
  7a:	9300      	str	r3, [sp, #0]
  7c:	4623      	mov	r3, r4
  7e:	f7ff ffc3 	bl	8 <pass_int_function>
  82:	60f8      	str	r0, [r7, #12]
  84:	4b0c      	ldr	r3, [pc, #48]	; (b8 <reset_handler+0x58>)
  86:	681b      	ldr	r3, [r3, #0]
  88:	4618      	mov	r0, r3
  8a:	f7ff ffd2 	bl	32 <pass_float>
  8e:	60b8      	str	r0, [r7, #8]
  90:	4b0a      	ldr	r3, [pc, #40]	; (bc <reset_handler+0x5c>)
  92:	e9d3 3400 	ldrd	r3, r4, [r3]
  96:	4618      	mov	r0, r3
  98:	4621      	mov	r1, r4
  9a:	f7ff ffd4 	bl	46 <pass_uint64_t>
  9e:	e9c7 0100 	strd	r0, r1, [r7]
  a2:	e7fe      	b.n	a2 <reset_handler+0x42>
  a4:	000000c0 	andeq	r0, r0, r0, asr #1
  a8:	000000c4 	andeq	r0, r0, r4, asr #1
  ac:	000000c8 	andeq	r0, r0, r8, asr #1
  b0:	000000cc 	andeq	r0, r0, ip, asr #1
  b4:	000000d0 	ldrdeq	r0, [r0], -r0	; <UNPREDICTABLE>
  b8:	000000d4 	ldrdeq	r0, [r0], -r4
  bc:	000000d8 	ldrdeq	r0, [r0], -r8

Disassembly of section .data:

000000c0 <int_val1>:
  c0:	00000001 	andeq	r0, r0, r1

000000c4 <int_val2>:
  c4:	00000002 	andeq	r0, r0, r2

000000c8 <int_val3>:
  c8:	00000003 	andeq	r0, r0, r3

000000cc <int_val4>:
  cc:	00000004 	andeq	r0, r0, r4

000000d0 <int_val5>:
  d0:	00000005 	andeq	r0, r0, r5

000000d4 <float_val>:
  d4:	41b1c28f 			; <UNDEFINED> instruction: 0x41b1c28f

000000d8 <t>:
  d8:	00000021 	andeq	r0, r0, r1, lsr #32
  dc:	00000000 	andeq	r0, r0, r0

Disassembly of section .comment:

00000000 <.comment>:
   0:	3a434347 	bcc	10d0d24 <t+0x10d0c4c>
   4:	35312820 	ldrcc	r2, [r1, #-2080]!	; 0xfffff7e0
   8:	332e363a 			; <UNDEFINED> instruction: 0x332e363a
   c:	732b312e 			; <UNDEFINED> instruction: 0x732b312e
  10:	35326e76 	ldrcc	r6, [r2, #-3702]!	; 0xfffff18a
  14:	39333033 	ldmdbcc	r3!, {r0, r1, r4, r5, ip, sp}
  18:	7562312d 	strbvc	r3, [r2, #-301]!	; 0xfffffed3
  1c:	31646c69 	cmncc	r4, r9, ror #24
  20:	2e362029 	cdpcs	0, 3, cr2, cr6, cr9, {1}
  24:	20312e33 	eorscs	r2, r1, r3, lsr lr
  28:	37313032 			; <UNDEFINED> instruction: 0x37313032
  2c:	30323630 	eorscc	r3, r2, r0, lsr r6
	...

Disassembly of section .ARM.attributes:

00000000 <.ARM.attributes>:
   0:	00003041 	andeq	r3, r0, r1, asr #32
   4:	61656100 	cmnvs	r5, r0, lsl #2
   8:	01006962 	tsteq	r0, r2, ror #18
   c:	00000026 	andeq	r0, r0, r6, lsr #32
  10:	726f4305 	rsbvc	r4, pc, #335544320	; 0x14000000
  14:	2d786574 	cfldr64cs	mvdx6, [r8, #-464]!	; 0xfffffe30
  18:	0600344d 	streq	r3, [r0], -sp, asr #8
  1c:	094d070d 	stmdbeq	sp, {r0, r2, r3, r8, r9, sl}^
  20:	14041202 	strne	r1, [r4], #-514	; 0xfffffdfe
  24:	17011501 	strne	r1, [r1, -r1, lsl #10]
  28:	1a011803 	bne	4603c <t+0x45f64>
  2c:	22061e01 	andcs	r1, r6, #1, 28
  30:	Address 0x0000000000000030 is out of bounds.
```

## 3. 結果與討論
From [The ARM-Thumb Procedure Call StandardSpecification](http://infocenter.arm.com/help/topic/com.arm.doc.espc0002/ATPCS.pdf), we can compare the mechanism with the assembly from objdump.
* 5 THE BASE PROCEDURE CALL STANDARD
The base standard defines a machine-level, core-registers-only calling standard common to the ARM and Thumb instruction sets.
* 5.1.1 Core registers
The first four registers r0-r3 (a1-a4) are used to pass argument values into a subroutine and to return a result value from a function.
> Observed in 2.
* 5.1.1.1 Handling values larger than 32 bits
Fundamental types larger than 32 bits may be passed as parameters to, or returned as the result of, function calls.
When these types are in core registers the following rules apply:
 A double-word sized type is passed in two consecutive registers (e.g., r0 and r1, or r2 and r3). The content of the registers is as if the value had been loaded from memory representation with a single LDM instruction.
 A 128-bit containerized vector is passed in four consecutive registers. The content of the registers is as if the value had been loaded from memory with a single LDM instruction.
> Observed in 6.
* 5.5 Parameter Passing
The base standard provides for passing arguments in core registers (r0-r3) and on the stack. For subroutines that take a small number of parameters, only registers are used, greatly reducing the overhead of a call.
> Observed in 3. 、 4.

### reference
* [The ARM-Thumb Procedure Call StandardSpecification](http://infocenter.arm.com/help/topic/com.arm.doc.espc0002/ATPCS.pdf)
* [ARM的規則ATPCS和AAPCS](https://codertw.com/%E7%A8%8B%E5%BC%8F%E8%AA%9E%E8%A8%80/423687/)
