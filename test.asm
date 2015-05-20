	AREA DonneeSon, DATA, READONLY
	export LongueurSon
	export PeriodeSonMicroSec
	export Son
LongueurSon		DCD	6
PeriodeSonMicroSec	DCD	91
Son 	dcw	0
	dcw	1	; plus petite valeur positive
	dcw	-1	; plus petite valeur négative
	dcw	32767	; plus grande valeur positive
	dcw	-32768	; plus grande valeur négative
	dcw	0