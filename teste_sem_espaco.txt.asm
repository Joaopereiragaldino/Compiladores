;UFMT-Compiladores
;Prof. Ivairton
;Procedimento para geracao do executavel apos compilacao (em Linux):
;(1) compilacao do Assembly com nasm: $ nasm -f elf64 <nome_do_arquivo>
;(2) likedicao: $ ld -m elf_x86_64 <nome_arquivo_objeto>

extern printf
extern scanf
section .data
fmtstr0  db  "%d",0
fmtstr1  db  "%s",0
i  dd  0
f  dd 0.0
c  db  0
s  db  "                " 
global  dd  0
x  dd  0
y  dd  0
valor  dd  0

	section .text
	global main,_start
main:
_start:
	mov dword [i], 0
