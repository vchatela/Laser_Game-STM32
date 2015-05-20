; ce programme est pour l'assembleur RealView (Keil)
	thumb
	area  moncode, code, readonly
	export son_callback
	include ../etat.inc
	extern etat

TIM3_CCR3	equ	0x4000043C	; adresse registre PWM
;

son_callback	proc			; appelé par principal.c
	; on récupere la valeur (son + [index*2]), index car tableau
	ldr r0, =etat    ;@ début struct
	ldr r1, [r0, #E_SON]  ;r1 = debut son
	ldr r2, [r0, #E_POS]  ;r2=valeur position
	; on verifie que position dépasse pas longeurmax
	ldr r3, [r0, #E_TAI] ; r3 = taille
	subs r3, r2 ; on fait r3 = position - longeur max    ON met le S pour maj des flags !!!!!!
	BEQ onenvoi0   ; si Z est vrai alors on a dépassé donc on va a onenvoi0
	; on a bien du son a recupérer
	ldrsh r1, [r1, r2, LSL #1]   ;récupéré valeur du son : LSL #1 permet de multiplier r2 par 2
; on traite notre valeur entre décalage et tout et tout 
	
	; (val + 2^15)*etatresolution / 2^16    or val = r1
	add r1, #(1<<15) ; val + 2^15
		; on récupère etatresolution
	ldr r3, [r0, #E_RES]
		; on multiplie par etat resolution
	mul r1, r1, r3  ; r1 = r1 * r3
		; decallage a droite de 16 ( div par 2^16)
	lsr r1, r1, #16 ; ici on a la valeur finale (ds r1) a transmettre au [TIM3_CCR3]

	ldr r12, =TIM3_CCR3     ; car accès direct impossible
	str r1, [r12] ; on transmet r1 au registre    
	add r2, #1 ; add de 2octets pour incrementer l'index / offset
	str r2, [r0, #E_POS] ; on ecrit la nouvelle valeur
	B fin
; r1 = etatresolution/2
onenvoi0
	ldr r1, [r0, #E_RES]
	lsr r1, r1, #1  ; r1 = r1/2 = etat_resol / 2 qui correspond a 0 en signal son
	ldr r12, =TIM3_CCR3     ; car accès direct impossible
	str r1, [r12] ; on transmet r1 au registre

fin	bx	lr	; dernière instruction de la fonction
	endp
;
	end