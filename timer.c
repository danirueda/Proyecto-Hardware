/*--- ficheros de cabecera ---*/
#include "timer.h"
#include "44b.h"
#include "44blib.h"
#include "elementosComunes.h"
#include <inttypes.h>


unsigned int int_count = 0;	//cuenta para el 8-LED al pulsar un boton

static int valorLed; //variable utilizada para mostrar el numero en el 8Led
static int cuenta50 = 0; //variable utilizada para esperar medio segundo
static int cuenta30 = 30; //variable utilizada para esperar 1/3 de segundo
static int mantenido = 0; //variable para que al solar el boton, si se estaba manteniendo, no sume el valorLed
static int timer0_num_int = 0; //variable para contar el numero de interrupciones
static uint32_t valor_a_pintar = 0; //variable para guardar el valor del registro del boton que hemos pulsado
static uint32_t valor_actual; //variable que guarda el boton que ha pulsado el usuario
extern int empiezo; //variable externa definida en el main
extern int fila,columna,valor; //variables externas definidas en el main

/* Declaracion de funciones */
extern int D8Led_symbol(int);
void timer_ISR(void) __attribute__((interrupt("IRQ")));

/* Rutina de interrupcion del timer */
void timer_ISR(void)
{
	timer0_num_int++; //sumamos una interrupcion
	switch(estado_timer_boton) { //miramos en que estado nos encontramos
		case rebotes_subida: { //estamos en rebotes_subida
			if(timer0_num_int >= rebotes){ //esperamos 20ms para los rebotes de subida
				estado_timer_boton = leer_boton; //declaramos como siguiente estado leer_boton
				valor_a_pintar = 0; //ponemos el valor a pintar a 0
				timer0_num_int=0; //ponemos el numero de interrupciones a 0 para poder pasar al
			}
			break;
		}
		case leer_boton: { //estamos en leer_boton
			if(timer0_num_int >= monitorizarPulsado){ //esperamos 10ms
				cuenta50 ++;	//incrementamos cuenta50 en una unidad
				valor_actual = rPDATG & 0xc0; //registramos si hay algun boton pulsado y lo guardamos en valor_actual

				if (valor_a_pintar == 0) { //la primera vez que pulsamos el boton nos guardamos que boton hemos pulsado
					valor_a_pintar = valor_actual;	//para registrar el boton que hemos pulsado
				}

				if ((valor_actual != 0x80) && (valor_actual != 0x40)) {	//si no estamos pulsando ningun boton
					estado_timer_boton = rebotes_bajada; //declaramos como siguiente estado rebotes_bajada
				}

				else if(valor_actual == boton_izquierdo){	//si estamos pulsando el boton izq.
					if(cuenta50 >= 50){ //cuenta pulsacion
						cuenta30++;
						if(cuenta30 >= 30){ //cuenta para sumar valor led
							mantenido = 1;	//he mantenido el boton
							int_count++;	//sumo valor para mostrar en el led
							if(fila == 0){	//si aun no he seleccionado fila
								valorLed = int_count % 11; //muestro de 1 a A
								if(valorLed == 0){	//si valorLed es 0, para que muestre 1
									valorLed = 1;
									int_count = 1;
								}
							}
							else { //ya he elegido fila
								valorLed = int_count % 10;	//modulo 10 (de 0-9)
								if(valorLed == 0 && columna == 0){	//si es la seleccion de columna el 0 no aparece
									valorLed = 1;
									int_count = 1;
								}
							}
							D8Led_symbol(valorLed);	//muestro valor en el led
							cuenta30=0;
						}
					}
				}
				timer0_num_int = 0; //ponemos el numero de interrupciones a 0
			}
			break;
		}
		case rebotes_bajada: { //estado que representa el soltar el boton
			if(timer0_num_int >= rebotes){ //esperar 20 ms
				cuenta50=0;
				if(valor_a_pintar == boton_izquierdo) {	//si he pulsado el boton izq.
					if(mantenido==0){	//no lo he mantenido
						int_count++;	//aumento valor a mostrar en el led
					}
					mantenido = 0;
				} else if (valor_a_pintar == boton_derecho) {	//si pulso boton derecho
					if(empiezo!=0){		//pulso boton derecho con el programa ya empezado
						if(fila == 0){ //si aun no hemos seleccionado la fila
							valorLed = int_count % 11; //valores de fila de 0 a A
							if(valorLed == 0){
								valorLed = 1;
								int_count = 1;
							}
						}
						else { //para columna y valor
							valorLed = int_count % 10;	//modulo 10 (de 0-9)
							if(valorLed == 0 && columna == 0){	//si es la seleccion de columna el 0 no aparece
								valorLed = 1;
								int_count = 1;
							}
						}
						if (fila==0){	//si no esta la fila elegida la elegimos
							fila=valorLed;
						}
						else if(columna==0){ //si no esta la columna elegida la elegimos
							columna=valorLed;
						}
						else if(valor==-1){ //si no esta el valor elegido lo elegimos
							valor = valorLed;	//elijo valor
						}
					}

				}
				empiezo = 1; //empiezo = 1
				if(fila == 0){	//para elegir la fila mostramos los valores de 0 a A
					valorLed = int_count % 11;
					if(valorLed == 0){ //fila no puede ser 0
						valorLed = 1;
						int_count = 1;
					}
				}
				else { //si la fila ya esta elegida
					valorLed = int_count % 10;	//valorLed va de 0 a 9
					if(valorLed == 0 && columna == 0){	//si es la seleccion de columna el 0 no aparece
						valorLed = 1;
						int_count = 1;
					}
				}
				D8Led_symbol(valorLed);		//muestro valor en el led
				estado_timer_boton = habilitar_int; //siguiente estado habilitar_int
			}
			break;
		}
		case habilitar_int:{ //estado final para habilitar las interrupciones
			rINTMSK |= BIT_TIMER0;	// enmascaramos interrupción timer0
			rTCON &= 0xFFFFFFFE;    //parar el timer0
			rI_ISPC |= BIT_EINT4567; //bajamos el flag de peticion de la interrupcion
			rINTMSK &= ~(BIT_EINT4567);	//volvemos a activar las interrupciones de boton
			/* borrar bit en I_ISPC para desactivar la solicitud de interrupción*/
			rI_ISPC |= BIT_TIMER0; // BIT_TIMER0 está definido en 44b.h y pone un uno en el bit 13 que correponde al Timer0
			break;
		}
		default: { break;}
	}
}

void timer_init(void)
{
	/* Configuraion controlador de interrupciones */
	rINTMOD = 0x0; // Configura las linas como de tipo IRQ
	rINTMSK &= ~(BIT_TIMER0); // Emascara todas las lineas excepto Timer0 y el bit global (bits 26 y 13, BIT_GLOBAL y BIT_TIMER0 están definidos en 44b.h)

	/* Establece la rutina de servicio para TIMER0 */
	pISR_TIMER0 = (unsigned) timer_ISR;

	/* Configura el Timer0 */
	rTCFG0 &= 0xFFFFFF00 ; // ajusta el preescalado
	rTCFG1 = 0x0; // selecciona la entrada del mux que proporciona el reloj. La 00 corresponde a un divisor de 1/2.
	rTCNTB0 = 2000;// interrupcion cada 20 ms
	rTCMPB0 = 0;// valor de comparación
	/* establecer update=manual (bit 1) + inverter=on (¿? será inverter off un cero en el bit 2 pone el inverter en off)*/
	rTCON = 0x2;
	/* iniciar timer (bit 0) con auto-reload (bit 3)*/
	rTCON = 0x9009;		//respetamos el bit del timer 2 y ponemos a 0 el bit 1 (update=manual)

	// actualizacion estado
	estado_timer_boton = rebotes_subida;
}

int timer0_leer(void){
	return ((rTCNTB0-rTCMPB0)*timer0_num_int+(rTCNTB0-rTCNTO0))/32;	//dividimos por (32) frecuencia del reloj, resultado obtenido en micro segundos
}

