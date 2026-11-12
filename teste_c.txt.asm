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
g  dd  0
x  dd  0
y  dd  0
valor  dd  0

	section .text
	global main,_start
main:
_start:
	mov dword [i], 0
	mov dword [f], __float32__(1.5)
	mov byte [c], 'A'
	; Atribuicao de string para s: Teste Compilador
	mov dword [g], 10

	;le valor para i
	mov edx,4
	mov ecx,i
	mov ebx,0
	mov eax,3
	int 0x80
	; Empilha variavel i
	mov eax, dword [i]
	push rax
	; Armazenamento de numero
	mov rax,10
	push rax
	; Aplica operador booleano/exp.logica
	pop rbx
	pop rax
	mov rcx, 1
	cmp rax, rbx
	jl label_bool0
	mov rcx, 0
label_bool0:
	mov rax, rcx
	push rax
	;jump condicional
	pop rax
	cmp rax, 0
	jz label0
	; Empilha variavel i
	mov eax, dword [i]
	push rax
	; Armazenamento de numero
	mov rax,1
	push rax
	pop rax
	pop rbx
	add rax,rbx
	push rax
	; Atribuicao de valor em variavel
	pop rax
	mov dword [i], eax
	;jump incondicional
	jmp label1
label0:
	mov dword [i], 0
label1:
label2:
	; Empilha variavel i
	mov eax, dword [i]
	push rax
	; Armazenamento de numero
	mov rax,0
	push rax
	; Aplica operador booleano/exp.logica
	pop rbx
	pop rax
	mov rcx, 1
	cmp rax, rbx
	jg label_bool1
	mov rcx, 0
label_bool1:
	mov rax, rcx
	push rax
	;jump condicional
	pop rax
	cmp rax, 0
	jz label3
	; Empilha variavel i
	mov eax, dword [i]
	push rax
	; Armazenamento de numero
	mov rax,1
	push rax
	pop rbx
	pop rax
	sub rax,rbx
	push rax
	; Atribuicao de valor em variavel
	pop rax
	mov dword [i], eax
	;jump incondicional
	jmp label2
label3:
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
	; Empilha variavel g
	mov eax, dword [g]
	push rax
	; Atribuicao de valor em variavel
	pop rax
	mov dword [y], eax
	;chamada de funcao
	call label_func_calculo
	; Empilha variavel valor
	mov eax, dword [valor]
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
	; Empilha variavel valor
	mov eax, dword [valor]
	push rax
	; Armazenamento de numero
	mov rax,48
	push rax
	pop rax
	pop rbx
	add rax,rbx
	push rax
	; Atribuicao de valor em variavel
	pop rax
	mov dword [valor], eax

	;escreve valor de valor
	mov edx,4
	mov ecx,valor
	mov ebx,1
	mov eax,4
	int 0x80
	;retorno da funcao
	ret
