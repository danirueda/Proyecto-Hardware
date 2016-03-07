.global sudoku_recalcular_arm
.extern ini_cuadricula

sudoku_recalcular_arm:
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
			bl sudoku_candidatos_arm //Salta a la subrutina y devuelve un valor en r0
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
devuelve:   ldmdb fp, {r4-r10, fp, sp, pc}		//retorno de la subrutina



.global sudoku_candidatos_arm
sudoku_candidatos_arm:
			/*Bloque de activación de la subrutina*/
			mov ip, sp
			stmdb sp!, {r4-r10, fp, ip, lr, pc}
			sub fp, ip, #4

			sub sp, sp, #8		//dejo espacio en la pila para guardar dos variables

			str r0, [sp]     //guardamos el inicio de tablero en la cima de la pila
			//Cálculo de la posición del número
			mov r3, r1, LSL#5
			add r10,r0,r3		//nos ponemos al principio de la fila
			mov r6, r10
			mov r3, r2, LSL#1

			add r10,r10,r3		//Estamos en la posición de nuestra celda
			str	r10, [sp,#4]			//guardamos la dirección de nuestra celda en la pila

			//PROCEDEMOS A BORRAR LOS BITS DE CANDIDATOS
			ldrh r5,[r10]			//tenemos el valor
			and r5,r5,#0xFE00		//candidatos puestos a cero
			strh r5,[r10]


			mov r5,#1			//numero=1
			mov r4,#0			//i=0 (del bucle for)

filas:		cmp r4,#9			//comparacion for de filas
			bge col
			cmp r4,r2			//comparo i con columna (si estamos en nuestra celda, no entro)
			beq finFilas		//si son iguales no entro


			ldrh r7, [r6]		//cargamos el número de dicha posicion
			mov r7, r7, LSR #12 //Leemos el valor, equivalente a celda_leer_valor
			cmp r7, #0			//comparamos valorCelda con 0
			beq finFilas		//si la celda esta vacía salimos
			sub r7,r7,#1
			mov r5,r5,LSL r7	//Desplazo bits para poner el no candidato a 1
			ldrh r3,[r10]		//leemos el valor de nuestra celda
			orr r3,r3,r5		//ponemos el bit de NO candidato a 1
			strh r3, [r10]		//almacenamos en memoria el número con los no candidatos
			mov r5,#1			//pongo numero a 1


finFilas:	add r4,r4,#1		//i++
			add r6, r6, #2		//i++ de memoria
			b filas

col:		mov r6,#0
			mov r3, r2, LSL#1
			add r6,r0,r3		//nos ponemos al principio de la columna
			mov r5,#1			//numero=1
			mov r4,#0			//i=0 (del bucle for)


columnas:	cmp r4,#9			//comparacion for de filas
			bge cuadri
			cmp r4,r1			//comparo i con fila (si estamos en nuestra celda, no entro)
			beq finColumnas		//si son iguales no entro


			ldrh r7, [r6]		//cargamos el número de dicha posición
			mov r7, r7, LSR #12	//equivalente a celda_leer_valor
			cmp r7, #0			//comparamos valorCelda con 0
			beq finColumnas		//si la celda esta vacía salimos
			sub r7,r7,#1
			mov r5,r5,LSL r7	//Desplazo bits para poner el no candidato a 1
			ldrh r3,[r10]		//cargamos en r3 el valor de nuestra celda
			orr r3,r3,r5		//ponemos el bit de NO candidato a 1
			strh r3, [r10]		//almacenamos en memoria el número con los no candidatos
			mov r5,#1			//pongo numero a 1


finColumnas:	add r4,r4,#1		//i++
				add r6, r6, #32		//i++ de memoria
				b columnas


cuadri:		mov r9,#1		//r9=1 (para la variable numero)

			ldr r4,=lookup_table	//comprobamos el inicio de la cuadricula (filas)
			ldrb r4,[r4,r1]			//inicio de la fila en la cuadricula (inicioI)
			ldr r5,=lookup_table	//comprobamos el inicio de la cuadricula (columnas)
			ldrb r5,[r5,r2]			//inicio de la columna de la cuadricula (inicioJ)

			add r6,r4,#2	//hastaFila=posicion de fin de nuestra cuadricula (filas)
			add r7,r5,#2	//hastaColumna=posicion de fin de nuestra cuadricula (columnas)


//recorremos la cuadrícula poniendo los bits de los NO candidatos en nuestra celda a 1
cuadriculaI:	cmp r4,r6
				bgt termina
				ldr r5,=lookup_table	//comprobamos el inicio de la cuadricula (columnas)
				ldrb r5,[r5,r2]			//inicio de la columna de la cuadricula (inicioJ)

cuadriculaJ:	cmp r5,r7
				bgt finCuadriculaI
				//inicioI != fila || inicioJ != columna
				cmp r4,r1
				bne entra
				cmp r5,r2
				beq finCuadriculaJ

entra:		ldr r0,	[sp]	//cargamos en r0 el inicio de la cuadricula
			mov r8, r4, LSL#5
			add r0,r0,r8		//nos ponemos en nuestra fila
			mov r10,r5, LSL#1
			add r0,r0,r10		//nos ponemos en la columna
			ldrh r0,[r0]		//cargamos el numero
			mov r0, r0, LSR #12	//equivalente a celda_leer_valor
			cmp r0,#0
			beq finCuadriculaJ
			sub r0,r0,#1
			mov r9,r9,LSL r0	//ponemos el bit de NO candidato a 1
			ldr r0, [sp,#4]		//cargamos la posicion de nuestra celda
			ldrh r3,[r0]		//cargamos numero de nuestra celda
			orr r3,r3,r9		//ponemos el bit de NO candidato a 1
			strh r3,[r0]		//almacenamos el nuevo número
			mov r9,#1

finCuadriculaJ:	add r5,r5,#1		//inicioJ++
				b cuadriculaJ


finCuadriculaI:	add r4,r4,#1		//inicioI++
				b cuadriculaI

termina:	ldr r0, [sp,#4]		//cargo la direccion de nuestra celda
			ldrh r0, [r0]		//cargo el dato de nuestra celda


			//CAMBIADO
			mov r2, #0x0800
			and r1,r0,r2		//realizo una máscara para saber si es pista o no lo es
			cmp r1,#0x0800
			beq pista			//es pista
			mov r3, #0xF000
			and r1,r0,r3		//para coger el valor (aux en C)
			cmp r1,#0
			beq vacia			//celda vacía

			//celda llena que no es pista
			mov r4, #1				//numero = 1
			mov r1, r1, LSR #12		//aux = aux >> 12
			sub r1,r1,#1			// aux=aux-1
			mov r4,r4, LSL r1		// numero = numero << aux

			//Miramos si ese número que hemos puesto
			//es no candidato
			and r1,r0,r4
			cmp r1,r4
			beq bitError
			//si el número que se ha puesto
			//estaba de no candidato, se produce un error
			mov r0,#1				//celda llena y no hay error
			b retorna

pista:		mov r0,#1
			b retorna

vacia:      mov r0,#0
			b retorna

			//celda llena y con error
bitError: 	orr r0,r0,#0x0400
			ldr r1,[sp,#4]
			strh r0, [r1]	//cuadricula[fila][columna] |= 0x0400;
			mov r0, #-1		//retornamos

retorna:	add sp, sp,#4
			ldmdb fp, {r4-r10, fp, sp, pc}








