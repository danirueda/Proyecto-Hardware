.global sudoku_candidatos_thumb
.arm
sudoku_candidatos_thumb:
						ADR r3, lanzadera+1
						bx r3

.thumb
lanzadera:	   PUSH	{r4-r7,lr}
					sub sp, sp, #20		//dejo espacio en la pila para guardar variables
					str r1, [sp]		//almacenamos fila
					str r2, [sp,#4]		//almacenamos columna
					str r0, [sp,#16]    //guardamos el inicio de tablero
					//Cálculo de la posición del número
					mov r6,#32
					mov r3,#1
					mul r3,r6
					mul r3,r1
					//r1 tenemos la fila
					add r0,r0,r3		//nos ponemos al principio de la fila
					str r0, [sp,#8]		//guardamos la direccion del comienzo de la fila en la pila
					mov r6,#2
					mov r3,#1
					mul r3,r6
					mul r3,r2
					add r0,r0,r3
					//en este momento tenemos r0 apunta a la posicion de nuestra celda

					/***********ANIADIDO*********/
					//PROCEDEMOS A BORRAR LOS BITS DE CANDIDATOS
					ldrh r5,[r0]			//tenemos el valor
					ldr r4,=0xFE00
					and r5,r4			//candidatos puestos a cero
					strh r5,[r0]

					/*********FANIADIDO***********/

					str	r0, [sp,#12]	//guardamos la posicion de nuestra celda en la pila
					mov r5,#1			//numero=1
					mov r4,#0			//i=0 (del bucle for)
					ldr r0,[sp,#8]		//cargamos en r0 el principio de nuestra fila
					mov r6, #0



filas:		cmp r4,#9			//comparacion for de filas
			bge col
			cmp r4,r2			//comparo i con columna (si estamos en nuestra celda, no entro)
			beq finFilas		//si son iguales no entro


			ldrh r7, [r0,r6]	//cargamos el número de dicha posicion
			LSR r7,r7,#12		//equivalente al código de celda_leer_valor
			cmp r7, #0			//comparamos valorCelda con 0
			beq finFilas		//si la celda esta vacía salimos
			sub r7,r7,#1
			LSL r5,r7
			ldr r0, [sp,#12]	//cargamos en r0 la dirección de nuestra celda
			ldrh r3,[r0]		//leemos el valor de nuestra celda
			orr r3,r5			//ponemos el bit de NO candidato a 1
			strh r3, [r0]		//almacenamos en memoria el número con los no candidatos
			mov r5,#1			//pongo numero a 1


finFilas:	add r4,r4,#1		//i++
			add r6, r6, #2		//i de memoria ++
			ldr r0,[sp,#8]		//volvemos a cargar el inicio de fila
			b filas

col:		ldr r0, [sp,#16] 	//cargamos en r0 la posicion del inicio de la cuadricula
			mov r3,#2
			mul r3,r2
			add r0,r0,r3
			str r0, [sp,#8]		//guardamos en r0 la posicion de inicio de la columna
			//en este momentos estamos al inicio de la columna

			mov r6,#0
			mov r5,#1			//numero=1
			mov r4,#0			//i=0 (del bucle for)




columnas:	cmp r4,#9			//comparacion for de filas
			bge cuadri
			cmp r4,r1			//comparo i con fila (si estamos en nuestra celda, no entro)
			beq finColumnas		//si son iguales no entro


			ldrh r7, [r0,r6]	//cargamos el número de dicha posicion
			LSR r7,r7,#12		//equivalente al código de celda_leer_valor
			cmp r7, #0			//comparamos valorCelda con 0
			beq finColumnas		//si la celda esta vacía salimos
			sub r7,r7,#1
			LSL r5,r7
			ldr r0, [sp,#12]	//cargamos en r0 la direccion nuestra celda
			ldrh r3,[r0]		//cargamos en r3 el valor de nuestra celda
			orr r3,r5			//ponemos el bit de NO candidato a 1
			strh r3, [r0]		//almacenamos en memoria el número con los no candidatos
			mov r5,#1			//pongo numero a 1


finColumnas:	add r4,r4,#1		//i++
				add r6, r6, #32		//i de memoria ++
				ldr r0,[sp,#8]		//volvemos a cargar el inicio de columna
				b columnas


cuadri:

			ldr r4,=lookup_table
			ldrb r4,[r4,r1]			//inicio de la fila en la cuadricula (inicioI)
			ldr r1, [sp,#4]			//cargamos la columna en r1
			ldr r5,=lookup_table
			ldrb r5,[r5,r1]			//inicio de la columna de la cuadricula (inicioJ)


			add r6,r4,#2	//hastaFila=posicion de fin de nuestra cuadricula (filas)
			add r7,r5,#2	//hastaColumna=posicion de fin de nuestra cuadricula (columnas)

			//recorremos la cuadrícula poniendo los bits de los NO candidatos a 1
cuadriculaI:
			cmp r4,r6
			bgt termina
			ldr r1,[sp,#4]			//cojo de la pila el numero de columna
			ldr r5,=lookup_table
			ldrb r5,[r5,r1]			//inicio de la columna de la cuadricula (inicioJ)

cuadriculaJ:
			cmp r5,r7
			bgt finCuadriculaI
			ldr r1,[sp]			//cargo en r1 el número de fila
			ldr r2,[sp,#4]		//cargo en r2 el número de columna
			//inicioI != fila || inicioJ != columna
			cmp r4,r1
			bne entra
			cmp r5,r2
			beq finCuadriculaJ
entra:
			ldr r0,	[sp,#16]	//cargamos en r0 el inicio de la cuadricula
			mov r1,#32
			mul r1,r4

			add r0,r0,r1		//nos ponemos en nuestra fila
			mov r1,#2
			mov r2,#1
			mul r2,r1
			mul r2,r5
			add r0,r0,r2		//nos ponemos en la columna
			//ya nos encontramos en la posición deseada
			ldrh r0,[r0]		//cargamos el numero
			LSR r0,r0,#12		//equivalente al código de celda_leer_valor
			cmp r0,#0
			beq finCuadriculaJ
			mov r2, #1
			sub r0,r0,#1
			LSL r2, r0
			ldr r0, [sp,#12]	//posicion de nuestra celda
			ldrh r3,[r0]		//numero de nuestra celda
			orr r3,r2			//ponemos el bit de NO candidato a 1
			strh r3,[r0]		//almacenamos el nuevo número
			mov r2,#1

finCuadriculaJ:
			add r5,r5,#1		//inicioJ++
			ldr r0,[sp,#16]		//volvemos a cargar el inicio de la cuadrícula
			b cuadriculaJ
finCuadriculaI:
			add r4,r4,#1		//inicioI++
			b cuadriculaI
termina:
			ldr r1,[sp,#12]
			ldrh r1,[r1]		//cargo número

			/********CAMBIADO***********/
			mov r3,r1			//muevo a r3 el r1 (el número)
			ldr r2,=0x0800
			and r3,r2			//realizo una máscara para saber si es pista o no lo es
			ldr r5,=0x0800
			cmp r3,r5
			beq pista			//es pista
			ldr r2, =0xF000
			mov r3,r1			//muevo a r3 el r1 (el número)
			and r3,r2			//para coger el valor (aux en C)
			cmp r3,#0
			beq vacia			//celda vacía

			//celda llena que no es pista
			mov r4, #1			//numero = 1
			LSR r3,r3,#12		// aux = aux >> 12
			sub r3,r3,#1		// aux=aux-1
			LSL r4,r3			// numero = numero << aux

			//Miramos si ese número que hemos puesto
			//es no candidato
			mov r2,r1
			and r2,r4
			cmp r2,r4
			beq bitError		//si el número que se ha puesto
			//estaba de no candidato, se produce un error
			mov r0,#1			//celda llena y no hay error
			b retorna

pista:		mov r0,#1
			b retorna

vacia:		mov r0,#0
			b retorna

			//celda llena y con error
bitError:	ldr r2,=0x0400
			orr r1,r2			//pongo bit de error
			ldr r2,[sp,#12]		//cargo dirección de la celda
			strh r1,[r2]		//cuadricula[fila][columna] |= 0x0400;
			ldr r0,=-1			//retornamos

			/********FCAMBIADO**********/
retorna:	add sp, sp,#20
			pop		{r4-r7}
			pop {r1}
			bx r1
