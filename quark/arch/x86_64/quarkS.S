; IDT

global load_idt:function
load_idt:
lidt [rdi]
sti
ret

; GDT

extern tss

global load_gdt:function
load_gdt:
lgdt [rdi]
reloadSegments:
; Reload CS register:
PUSH 0x08                 ; Push code segment to stack, 0x08 is a stand-in for your code segment
LEA RAX, [rel .reload_CS] ; Load address of .reload_CS into RAX
PUSH RAX                  ; Push this value to the stack
RETFQ                     ; Perform a far return, RETFQ or LRETQ depending on syntax
.reload_CS:
; Reload data segment registers
MOV   AX, 0x10 ; 0x10 is a stand-in for your data segment
MOV   DS, AX
MOV   ES, AX
MOV   FS, AX
MOV   GS, AX
MOV   SS, AX
RET

global load_tss:function
load_tss:
mov rax,tss wrt ..sym
mov [rax + 4],rsp
mov ax,0x28
ltr ax
ret

; PIC

global init_pic:function
init_pic:
mov dx, 0x21
in al, dx
xchg bl,al
mov dx, 0xA1
in al, dx
xchg cl,al
mov al, 0x11
mov dx, 0x20
out dx, al
mov dx, 0xA0
out dx, al
mov dx, 0x21
xchg ax,di
out dx, al
mov dx, 0xA1
xchg ax,si
out dx, al
mov al, 4
mov dx, 0x21
out dx, al
mov al, 2
mov dx, 0xA1
out dx, al
mov al, 1
mov dx, 0x21
out dx, al
mov dx, 0xA1
out dx, al
mov dx, 0x21
xchg al,bl
out dx, al
mov dx, 0xA1
xchg al,cl
out dx, al
ret

global pic_clear_mask:function
pic_clear_mask:
cmp di, 8
jb .master
mov dx, 0xA1
sub di, 8
jmp .do
.master:
mov dx, 0x21
.do:
in al, dx
mov bl, 1
xchg cx, di
shl bl, cl
not bl
and al, bl
out dx, al
ret

; MM

global get_pmap:function
get_pmap:
mov rax,cr3
ret

; IDT

global die:function
die:
cli
hlt
jmp die

global idle:function
idle:
hlt
ret

global isr_no_err_stub:function
isr_no_err_stub:
call die

global isr_err_stub:function
isr_err_stub:
call die

global isr_irqm_stub:function
isr_irqm_stub:
push rax
mov al, 0x20
out 0x20, al
pop rax
iretq

global isr_irqs_stub:function
isr_irqs_stub:
push rax
mov al, 0x20
out 0x20, al
out 0xA0, al
pop rax
iretq

global isr_stub_table:data
isr_stub_table:
dq isr_no_err_stub wrt ..sym
dq isr_no_err_stub wrt ..sym
dq isr_no_err_stub wrt ..sym
dq isr_no_err_stub wrt ..sym
dq isr_no_err_stub wrt ..sym
dq isr_no_err_stub wrt ..sym
dq isr_no_err_stub wrt ..sym
dq isr_no_err_stub wrt ..sym
dq isr_err_stub wrt ..sym
dq isr_no_err_stub wrt ..sym
dq isr_err_stub wrt ..sym
dq isr_err_stub wrt ..sym
dq isr_err_stub wrt ..sym
dq isr_err_stub wrt ..sym
dq isr_err_stub wrt ..sym 
dq isr_no_err_stub wrt ..sym
dq isr_no_err_stub wrt ..sym
dq isr_err_stub wrt ..sym
dq isr_no_err_stub wrt ..sym
dq isr_no_err_stub wrt ..sym
dq isr_no_err_stub wrt ..sym
dq isr_no_err_stub wrt ..sym
dq isr_no_err_stub wrt ..sym
dq isr_no_err_stub wrt ..sym
dq isr_no_err_stub wrt ..sym
dq isr_no_err_stub wrt ..sym
dq isr_no_err_stub wrt ..sym
dq isr_no_err_stub wrt ..sym
dq isr_no_err_stub wrt ..sym
dq isr_no_err_stub wrt ..sym
dq isr_err_stub wrt ..sym
dq isr_no_err_stub wrt ..sym
dq isr_irqm_stub wrt ..sym
dq isr_irqm_stub wrt ..sym
dq isr_irqm_stub wrt ..sym
dq isr_irqm_stub wrt ..sym
dq isr_irqm_stub wrt ..sym
dq isr_irqm_stub wrt ..sym
dq isr_irqm_stub wrt ..sym
dq isr_irqm_stub wrt ..sym
dq isr_irqs_stub wrt ..sym
dq isr_irqs_stub wrt ..sym
dq isr_irqs_stub wrt ..sym
dq isr_irqs_stub wrt ..sym
dq isr_irqs_stub wrt ..sym
dq isr_irqs_stub wrt ..sym
dq isr_irqs_stub wrt ..sym
dq isr_irqs_stub wrt ..sym

; TASK

extern context_switch
extern save_register
extern load_register

global task_switch:function
task_switch:
push rax
push rbx
push rcx
push rdx
push rbp
push rsi
push rdi
push r8
push r9
push r10
push r11
push r12
push r13
push r14
push r15
%assign i 0
%rep 5
pop rdi
mov rsi,i
call save_register
%assign i i+1 
%endrep
%assign i i+1
%rep 10
pop rdi
mov rsi,i
call save_register
%assign i i+1 
%endrep
mov rdi, rsp
mov rsi, cr3
call context_switch
mov rsp, rax
mov cr3, rdx
%assign i 0
%rep 5
mov rdi,i
call load_register
push rax
%assign i i+1 
%endrep
%assign i i+1
%rep 10
mov rdi,i
call load_register
push rax
%assign i i+1 
%endrep
pop r15
pop r14
pop r13
pop r12
pop r11
pop r10
pop r9
pop r8
pop rdi
pop rsi
pop rbp
pop rdx
pop rcx
pop rbx
pop rax
ret