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
global  dd  0
x  dd  0
y  dd  0
valor  dd  0

	section .text
	global main,_start
main:
_start:
	mov dword [i], 5
	mov dword [global], 10
	; Empilha variavel i
	mov eax, dword [i]
	push rax
	; Armazenamento de numero
	mov rax,5
	push rax
	pop rax
	pop rbx
	add rax,rbx
	push rax
	; Atribuicao de valor em variavel
	pop rax
	mov dword [x], eax
	; Empilha variavel global
	mov eax, dword [global]
	push rax
	; Atribuicao de valor em variavel
	pop rax
	mov dword [y], eax
	;chamada de funcao
	call label_func_calculo
	; Empilha variavel i
	mov eax, dword [i]
	push rax
	; Atribuicao de valor em variavel
	pop rax
	mov dword [valor], eax
	;chamada de funcao
	call label_func_mostrar

	;encerra programa
	mov ebx,0
	mov eax,1
	int 0x80
label_func_calculo:
	; Empilha variavel x
	mov eax, dword [x]
	push rax
	; Empilha variavel y
	mov eax, dword [y]
	push rax
	pop rax
	pop rbx
	add rax,rbx
	push rax
	; Atribuicao de valor em variavel
	pop rax
	mov dword [x], eax
	;retorno da funcao
	ret
label_func_mostrar:

	;escreve valor de valor
	mov edx,4
	mov ecx,valor
	mov ebx,1
	mov eax,4
	int 0x80
	;retorno da funcao
	ret
