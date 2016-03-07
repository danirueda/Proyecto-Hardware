.global sudoku_recalcular_arm_thumb


sudoku_recalcular_arm_thumb:

						/*Bloque de activación de la subrutina*/
			mov ip, sp
			stmdb sp!, {r4-r10, fp, ip, lr, pc}
			sub fp, ip, #4

			mov r7, r0 //guardo inicio de cuadricula en r7
			mov r4,#0  //contadorVacias , cuenta el número de celdas vacías
			mov r8,#0	//contadorErrores, cuenta el numero de errores
			mov r5,#0	//i=0
for1:		cmp r5,#9
			bge final
			mov r6,#0	//j=0
for2:		cmp r6,#9
			bge finFor1
			mov r0, r7
			mov r1, r5
			mov r2, r6
			/*En r0 tengo cuadricula, en r1 i y en r2 j. Llamo a función */
			bl sudoku_candidatos_thumb //Salta a la subrutina y devuelve un valor en r0
			//Devuelve 0 si está vacía, 1 si tiene un valor
			cmp r0,#0
			bne mirarError
			add r4,r4,#1
			b finFor2
mirarError: cmp r0,#-1
			bne finFor2
			sub r8,r8,#1
finFor2:	add r6,r6,#1
			b for2
finFor1:	add r5,r5,#1
			b for1

final:		cmp r8,#0
			bne hayErrores	//hay errores
			mov r0,r4 //r0=contadorVacias (no hay errores)
			b devuelve
hayErrores:	mov r0,r8

devuelve:	ldmdb fp, {r4-r10, fp, sp, pc}		//retorno de la subrutina
