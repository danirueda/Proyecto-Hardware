/*--- ficheros de cabecera ---*/
#include "timer.h"
#include "44blib.h"
#include "44b.h"
#include "stdio.h"
#include "lcd.h"
#include "Bmp.h"
#include "elementosComunes.h"


int empiezo = 0; //indica si he pulsado algun boton para iniciar el programa
int fila, columna = 0; //indican la fila y la columna que se han introducido (al inicio 0)
int valor = -1; //indica el valor que se quiere introducir en la celda (al inicio -1)
static int cuenta, resultado1, resultado2; //variables utilizadas para los tiempos de calculo (timer 2)
static INT8U borrado[1024]; //tabla de chars que servira pra borrar una zona de pantalla
static int i; //variable que se utilizara para rellenar la tabla anterior
static int segundo=0; //contador de segundos
int empezarCuenta = 0;	//flag utilizado para no leer el tiempo hasta que no se introduce el primer valor en alguna celda
extern int contadorSegundo; //variable externa al fichero
int cuentaFinPartida = 0; //flag que se utliza para poner la cuenta a 0 cada vez que finalizamos una partida


/*--- funciones externas ---*/
extern void Eint4567_init();
extern void D8Led_init();
extern void sudoku9x9();
extern void D8Led_symbol(int);
extern void restaurarCuadricula();

extern void timer2_inicializar();
extern int timer2_leer();
extern void poner_valor(int,int,int);

extern void Lcd_inicio(void);
extern void pintarSudoku(void);

/*--- declaracion de funciones ---*/
void Main(void);

/*--- codigo de funciones ---*/
void Main(void)
{
	//se llena la tabla de chars (se utilizara para borrar un trozo de pantalla)
	for(i=0;i<1024;i++){
		borrado[i] = WHITE;
	}
	STRU_BITMAP borrar = {0x10, 4, 32, 32, TRANSPARENCY, (INT8U *)borrado}; //creamos la estructura con la tabla anterior


	estado_timer_boton = inicio; // inicializamos el estado de la maquina de estados que controlara el funcionamiento
	//de los botones

	//Inicializa controladores
	sys_init();        // Inicializacion de la placa, interrupciones y puertos
	Lcd_inicio();        // inicializacion LCD y visualizacion texto y rectangulos
	Eint4567_init();	// inicializamos los pulsadores
	D8Led_init(); // inicializamos el 8led
	timer2_inicializar(); //inicializamos el timer2


	/* EMPIEZA NUESTRO PROGRAMA*/
	while(1){ //bucle infinito
		while (empiezo!=1);	//mientras no pulse un boton no empiezo

		sudoku9x9(); //recalculamos el tablero
		pintarSudoku(); //pintamos el tablero (vacio)
		pintarNumeros(); //pintamos los numeros
		if(empezarCuenta == 1){ //en la primera vez que ejecutamos el juego, no tenemos que calcular la cuenta
			resultado2 = timer2_leer();
			cuenta += resultado2 - resultado1; //sumamos el tiempo de calculo al que ya habia
			if(cuentaFinPartida == 1){ //cuando hemos finalizado el juego, reinicializamos la cuenta
				cuenta = 0;
				cuentaFinPartida = 0;
			}
			BitmapView(245,120,borrar); //borramos la parte de la pantalla en la que se encuentra el tiempo de calculo
			Lcd_DspAscII8x16(245,120,BLACK,itoa(cuenta/1000)); //dibujamos el nuevo tiempo de calculo
			Lcd_Dma_Trans(); //actualizamos los cambios en la pantalla

		}

		while(fila == 0){	//mientras no seleccione fila
			if(contadorSegundo % llegoSegundo == 0){ //cada llegoSegundo interrupciones (489) cuento un segundo
			    contadorSegundo=0; //ponemos el contador a 0
			    segundo++; //incrementamos el numero de segundos
			    BitmapView(245,55,borrar); //borramos el trozo de pantalla que corresponde al tiempo de partida
			    Lcd_DspAscII8x16(245,55,BLACK,itoa(segundo)); //escribimos el nuevo tiempo
			    Lcd_Dma_Trans(); //actualizamos los cambios en pantalla
			}
		}
		if(fila == 10){ //si fila es A, sale del programa
			D8Led_symbol(15); //mostramos una F en el 8Led
			finPartida(); //llamamos a fin partida
			cuentaFinPartida=1; //ponemos cuentaFinPartida a 1 para que reinicie el tiempo de calculo
			empiezo = 0; //ponemos empiezo a 0 para que espere otra vez a que el usuario pulse un boton
			restaurarCuadricula(); //restauramos la cuadricula inicial
			segundo=0; //ponemos el tiempo de partida a 0
		}
		else { //nos han introducido un numero de fila que no es A (seguimos en partida)
			Delay(500); //retardo para que muestre bien la confimacion por el 8Led
			D8Led_symbol(12);	//muestro C al confirmar
			while(columna == 0){ //mientras no seleccione columna
				if(contadorSegundo % llegoSegundo == 0){ //cada llegoSegundo interrupciones (489) cuento un segundo
					contadorSegundo=0; //ponemos el contador a 0
					segundo++; //incrementamos el numero de segundos
					BitmapView(245,55,borrar); //borramos el trozo de pantalla que corresponde al tiempo de partida
					Lcd_DspAscII8x16(245,55,BLACK,itoa(segundo)); //escribimos el nuevo tiempo
					Lcd_Dma_Trans(); //actualizamos los cambios en pantalla
				}
			}
			Delay(500); //retardo para que muestre bien la confimacion por el 8Led
			D8Led_symbol(12);	//muestro C al confirmar
			while(valor == -1){ //mientras no seleccione valor
				if(contadorSegundo % llegoSegundo == 0){ //cada llegoSegundo interrupciones (489) cuento un segundo
					contadorSegundo=0; //ponemos el contador a 0
					segundo++; //incrementamos el numero de segundos
					BitmapView(245,55,borrar); //borramos el trozo de pantalla que corresponde al tiempo de partida
					Lcd_DspAscII8x16(245,55,BLACK,itoa(segundo)); //escribimos el nuevo tiempo
					Lcd_Dma_Trans(); //actualizamos los cambios en pantalla
				}
			}
			Delay(500); //retardo para que muestre bien la confimacion por el 8Led
			D8Led_symbol(12);	//mostramos C al confirmar
			resultado1 = timer2_leer(); //leemos el tiempo del timer2 (lo utilizaremos para saber el tiempo de calculo)
			poner_valor(fila-1,columna-1,valor); //llamamos a la funcion que pone valor en la cuadricula
		}
		//inicializamos fila, columna, valor y empezarCuenta para la siguiente iteracion del usuario
		fila = 0;
		columna = 0;
		valor = -1;
		empezarCuenta = 1;
	}
}
