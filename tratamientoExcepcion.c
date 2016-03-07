/*--- ficheros de cabecera ---*/
#include "44b.h"
#include "44blib.h"

extern void D8Led_symbol();
unsigned int tipo;

/* Funcion que se ejecuta cuando salta una excepcion */
void ISR_queHacer(void){
	asm("ldr r0, =tipo\n\t"
			"mrs r2, CPSR\n\t"
			"str r2,[r0]"); //copiamos el valor del registro de estado en la variable tipo
	tipo &= 0x001F; //obtenemos el tipo de excepcion
	D8Led_symbol(14); //mostramos en el led una E de error
}

/* Funcion para establecer la rutina de interrupcion para las distintas interrupciones */
void establecerRutinas(void){
	pISR_UNDEF = (unsigned) ISR_queHacer;
	pISR_PABORT = (unsigned) ISR_queHacer;
	pISR_DABORT	= (unsigned) ISR_queHacer;
}
