; ce programme est pour l'assembleur RealView (Keil)
	thumb
	area  moncode, code, readonly
	export Sum_square
	extern TabCos
	extern TabSin

Sum_square	proc			; appel� par principal.c
		; on r�cup�re l'origine de tabCos 
		ldr r1, =TabCos
		; on r�cup�re la valeur de tabcos + offset (r0)
		ldrsh r2, [r1, r0, LSL #1]
		; on la met au carre dans le registre r�sultat
		mul r3, r2, r2
		; Origine tabSin
		ldr r1, =TabSin
		; val tabsin + offset
		ldrsh r2, [r1, r0, LSL #1]
		; on met au carre dans registre tempo
		mul r0, r2, r2
		; on ajoute tempo au resultat pr�c�dent
		add r0, r3
		; on retourne ca ds r0
fin	bx	lr	; derni�re instruction de la fonction
	endp
;
	END