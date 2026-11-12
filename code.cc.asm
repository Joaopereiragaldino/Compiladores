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
idade  dd  0
nome  db  "                " 

	section .text
	global main,_start
main:
_start:

;le valor para idade
mov edx,4
mov ecx,idade
mov ebx,0
mov eax,3
int 0x80

;escreve valor de idade
mov edx,4
mov ecx,idade
mov ebx,1
mov eax,4
int 0x80
	; Atribuicao de string para nome: Compiladores

;escreve valor de nome
mov edx,4
mov ecx,nome
mov ebx,1
mov eax,4
int 0x80

;encerra programa
mov ebx,0
mov eax,1
int 0x80
