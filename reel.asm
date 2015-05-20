	thumb
	area  moncode, code, readonly
	export part
	export m2
	extern TabSin
	extern TabCos

	; données passées en paramètre : @ base signal , valeur k, @ tabcos ou tabsin
part	proc
	; r0 = @ base signal
	; r1 =  valeur de k
	; r2 = @ tab			
	;r3 = somme
	; r12 = valeur de i
	; Registre r3 contenant la somme de :
	; x(i) valeur du signal au temps i
	; récupérer la valeur du cos/sin (i k 2pi / N)
	; les multipliers et les metrte dans la somme
	mov r3, #0
	;stocker i dans r12
	mov r12, #0
	; on calcul ik (mod 2^p)
	push {r4}
	push {r5}
loop	mov r4, r1 ; on met k dans r4
	mul r4, r12 ; i * k : pas de débordement possible
	; masquage des bits >=  6 (car 64 = 2^6)
	and r4, #0x3F ; 0x3F = 0b111111
	;ici r4 est la valeur du rang dans la table
	ldrsh r4, [r2, r4, LSL #1] ;on récupère la valeur de tab + offset (r4)
	; maintenant r4 (cos ik..) * x(i) 
	; récupérer x(i)
	ldrsh r5, [r0, r12, LSL #1] ; LSL car même principe que tab
	mul r4, r5 ;r4 * x(i)
	add r3, r4 ; la somme a bien fini sont tour i
	; on incrémente i
	add r12, #0x1
	; on teste 64 - i = 0
	mov r4, #64
	subs r4, r12
	bne loop ; branch non equal
	
	pop{r5}
	pop{r4}
	mov r0, r3 ;on met r3 comme valeur de retour
	bx	lr	; dernière instruction de la fonction
	endp


m2	proc
	; r0 = @ base signal
	; r1 =  valeur de k
	ldr r2, =TabCos
	
	push {LR}
	push {r0}
	bl part
	; on a le résultat du reel dans r0

	mov r2, r0
	pop{r0} ; on remet r0 = @ signal 
	push {r2} ; on a stocké dans la pile le résultat de part1 = re(k)

	ldr r2, =TabSin
	bl part

	smull r2, r1, r0, r0 ;im^2
	pop{r0} ; on récupère re(k)
	smlal r2, r1, r0, r0 ; re^2 + im^2 dans r1 (high) et r2(low)
	; on a 64 bits signés
	; on veut 32 bits de pdids forts signés
	mov r0, r1
	pop{PC} ; POP {LR} + bx lr
	endp

;
	END