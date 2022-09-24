extern task_switch
global isr_irq0_pit:function
isr_irq0_pit:
push rax
mov al, 0x20
out 0x20, al
pop rax
call task_switch
iretq

extern keyboard_handler
global isr_irq1_keyb:function
isr_irq1_keyb:
push rdi
push rax
mov al, 0x20
out 0x20, al
in al, 0x60
mov rdi, al
pop rax
call keyboard_handler
pop rdi
iretq

extern incur_page_fault
global isr_exc14_pf:function
isr_exc14_pf:
xchg rdi, [rsp]
push rsi
mov rsi, cr2
call incur_page_fault
pop rsi
pop rdi
iretq

global init_pit:function
init_pit:
mov al, 0x36
out 0x43, al
mov al, 0x8C
out 0x40, al
mov al, 0x43
out 0x40, al
ret