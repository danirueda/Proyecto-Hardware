/*********************************************************************************************
* Fichero:      timer.c
* Autor:
* Descrip:      funciones de control del timer0 del s3c44b0x
* Version:
*********************************************************************************************/

/*--- ficheros de cabecera ---*/
#include "44b.h"
#include "44blib.h"
#include "lcd.h"
#define INT_DIGITS 19		/* enough for 64 bit integer */

/*--- variables globales ---*/
int timer2_num_int = 0;
int contadorSegundo=0; //variable utilizada para saber cuando se llega al segundo (comprobado en el main)

/*--- declaracion de funciones ---*/
void rutina_Timer2(void) __attribute__((interrupt("IRQ")));
void timer2_inicializar(void);

/* Funcion para pasar un entero a puntero a caracter */
char *itoa(i)
     int i;
{
  /* Room for INT_DIGITS digits, - and '\0' */
  static char buf[INT_DIGITS + 2];
  char *p = buf + INT_DIGITS + 1;	/* points to terminating '\0' */
  if (i >= 0) {
    do {
      *--p = '0' + (i % 10);
      i /= 10;
    } while (i != 0);
    return p;
  }
  else {			/* i < 0 */
    do {
      *--p = '0' - (i % 10);
      i /= 10;
    } while (i != 0);
    *--p = '-';
  }
  return p;
}


/*--- codigo de las funciones ---*/
void rutina_Timer2(void)
{
    timer2_num_int++; //incrementamos el numero de interrupciones
    contadorSegundo++; //incrementamos la variable
    /* borrar bit en I_ISPC para desactivar la solicitud de interrupción*/
    rI_ISPC |= BIT_TIMER2; // BIT_TIMER2 está definido en 44b.h y pone un uno en el bit que correponde al Timer2
}

void timer2_inicializar(void)
{
    /* Configuraion controlador de interrupciones */
    rINTMOD = 0x0; // Configura las linas como de tipo IRQ
    rINTMSK &= ~(BIT_TIMER2); // Emascara todas las lineas excepto Timer2

    /* Establece la rutina de servicio para TIMER2 */
    pISR_TIMER2 = (unsigned) rutina_Timer2;

    /* Configura el Timer2 */
    rTCFG0 &= 0xFFFF00FF; // ajusta el preescalado
    rTCFG1 = 0x0; // selecciona la entrada del mux que proporciona el reloj. La 00 corresponde a un divisor de 1/2.
    rTCNTB2 = 65535;// valor inicial de cuenta (la cuenta es descendente)
    rTCMPB2 = 0;// valor de comparación
    /* establecer update=manual (bit 14) */
    rTCON = 0x2000;
    /* iniciar timer (bit 13) con auto-reload (bit 16)*/
    rTCON = 0x9000;
}

int timer2_leer(void){
	return ((rTCNTB2-rTCMPB2)*timer2_num_int+(rTCNTB2-rTCNTO2))/32;	//dividimos por (32) frecuencia del reloj, resultado obtenido en micro segundos
}

void timer2_empezar(void){
	timer2_num_int=0;
	rTCNTB2 = 65535;// valor inicial de cuenta (la cuenta es descendente)
	rTCMPB2 = 0;// valor de comparación
	/* establecer update=manual (bit 14) */
	rTCON = 0x2000;
	/* iniciar timer (bit 13) con auto-reload (bit 16)*/
	rTCON = 0x9000;
}
