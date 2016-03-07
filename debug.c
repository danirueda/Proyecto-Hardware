/*--- ficheros de cabecera ---*/
#include "44b.h"
#include "44blib.h"

//extern int timer2_leer();
extern int timer0_leer();

#define DebugStack (_ISR_STARTADDRESS-0xf00+256*6); //Macro para establecer en memoria el inicio de la pila

int* cima_pila = DebugStack; // 0xc7ff600

int* final_pila = _ISR_STARTADDRESS-0xf00+256*5;

void push_debug(int ID_evento, int auxData);

void push_debug(int ID_evento, int auxData){
	int tiempo = timer0_leer(); //leemos el valor del timer
	if (cima_pila - 1 != final_pila){ //si no hemos llegado al final de la pila
		*cima_pila = ID_evento; //apilamos ID_evento
		cima_pila = cima_pila - 1; //movemos el stack pointer
		*cima_pila = auxData; //apilamos el auxData
		cima_pila = cima_pila -1; //movemos stack pointer
		*cima_pila = tiempo; //apilamos el tiempo
		cima_pila = cima_pila-1; //movemos stack pointer
	}
	else{ //si hemos llegado al final de la pila
		cima_pila = DebugStack; //volvemos a apilar desde el inicio
		*cima_pila = ID_evento;
		cima_pila = cima_pila - 1;
		*cima_pila = auxData;
		cima_pila = cima_pila -1;
		*cima_pila = tiempo;
		cima_pila = cima_pila-1;
	}

}
