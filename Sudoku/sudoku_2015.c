#include<inttypes.h>

// Tamaños de la cuadricula
// Se utilizan 16 columnas para facilitar la visualización
enum {NUM_FILAS = 9, NUM_COLUMNAS = 16};

enum {FALSE = 0, TRUE = 1};



typedef uint16_t CELDA; 
// La información de cada celda está agrupada en 16 bits con el siguiente formato (empezando en el bit más significativo):
// 4 MSB VALOR
// 1 bit PISTA
// 1 bit ERROR
// 1 bit no usado
// 9 LSB CANDIDATOS

//Tabla utilizada para posicionarnos al inicio de la cuadricula
uint8_t lookup_table[9]={0,0,0,3,3,3,6,6,6};

uint16_t cuadriculaSol[9][16] = {
		{0x9800,0x6800,0x0000,0x0000,0x0000,0x0000,0x7800,0x0000,0x8800,0,0,0,0,0,0,0},
	    {0x8800,0x0000,0x0000,0x0000,0x0000,0x4800,0x3800,0x0000,0x0000,0,0,0,0,0,0,0},
	    {0x1800,0x0000,0x0000,0x5800,0x0000,0x0000,0x0000,0x0000,0x0000,0,0,0,0,0,0,0},
	    {0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x1800,0x7800,0x6800,0,0,0,0,0,0,0},
	    {0x2800,0x0000,0x0000,0x0000,0x9800,0x3800,0x0000,0x0000,0x5800,0,0,0,0,0,0,0},
	    {0x7800,0x0000,0x8800,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0,0,0,0,0,0,0},
	    {0x0000,0x0000,0x7800,0x0000,0x3800,0x2800,0x0000,0x4800,0x0000,0,0,0,0,0,0,0},
	    {0x3800,0x8800,0x2800,0x1800,0x0000,0x5800,0x6800,0x0000,0x0000,0,0,0,0,0,0,0},
	    {0x0000,0x4800,0x1800,0x0000,0x0000,0x9800,0x5800,0x2800,0x0000,0,0,0,0,0,0,0}};

uint16_t cuadriculaIni[9][16] = {
		{0x9800,0x6800,0x0000,0x0000,0x0000,0x0000,0x7800,0x0000,0x8800,0,0,0,0,0,0,0},
	    {0x8800,0x0000,0x0000,0x0000,0x0000,0x4800,0x3800,0x0000,0x0000,0,0,0,0,0,0,0},
	    {0x1800,0x0000,0x0000,0x5800,0x0000,0x0000,0x0000,0x0000,0x0000,0,0,0,0,0,0,0},
	    {0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x1800,0x7800,0x6800,0,0,0,0,0,0,0},
	    {0x2800,0x0000,0x0000,0x0000,0x9800,0x3800,0x0000,0x0000,0x5800,0,0,0,0,0,0,0},
	    {0x7800,0x0000,0x8800,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0,0,0,0,0,0,0},
	    {0x0000,0x0000,0x7800,0x0000,0x3800,0x2800,0x0000,0x4800,0x0000,0,0,0,0,0,0,0},
	    {0x3800,0x8800,0x2800,0x1800,0x0000,0x5800,0x6800,0x0000,0x0000,0,0,0,0,0,0,0},
	    {0x0000,0x4800,0x1800,0x0000,0x0000,0x9800,0x5800,0x2800,0x0000,0,0,0,0,0,0,0}};

uint16_t cuadricula1[9][16] = {
		{0x9800,0x6800,0x0000,0x0000,0x0000,0x0000,0x7800,0x0000,0x8800,0,0,0,0,0,0,0},
		{0x8800,0x0000,0x0000,0x0000,0x0000,0x4800,0x3800,0x0000,0x0000,0,0,0,0,0,0,0},
		{0x1800,0x0000,0x0000,0x5800,0x0000,0x0000,0x0000,0x0000,0x0000,0,0,0,0,0,0,0},
		{0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x1800,0x7800,0x6800,0,0,0,0,0,0,0},
		{0x2800,0x0000,0x0000,0x0000,0x9800,0x3800,0x0000,0x0000,0x5800,0,0,0,0,0,0,0},
		{0x7800,0x0000,0x8800,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0,0,0,0,0,0,0},
		{0x0000,0x0000,0x7800,0x0000,0x3800,0x2800,0x0000,0x4800,0x0000,0,0,0,0,0,0,0},
		{0x3800,0x8800,0x2800,0x1800,0x0000,0x5800,0x6800,0x0000,0x0000,0,0,0,0,0,0,0},
		{0x0000,0x4800,0x1800,0x0000,0x0000,0x9800,0x5800,0x2800,0x0000,0,0,0,0,0,0,0}};

uint16_t cuadricula2[9][16] = {
		{0x9800,0x6800,0x0000,0x0000,0x0000,0x0000,0x7800,0x0000,0x8800,0,0,0,0,0,0,0},
		{0x8800,0x0000,0x0000,0x0000,0x0000,0x4800,0x3800,0x0000,0x0000,0,0,0,0,0,0,0},
		{0x1800,0x0000,0x0000,0x5800,0x0000,0x0000,0x0000,0x0000,0x0000,0,0,0,0,0,0,0},
		{0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x1800,0x7800,0x6800,0,0,0,0,0,0,0},
		{0x2800,0x0000,0x0000,0x0000,0x9800,0x3800,0x0000,0x0000,0x5800,0,0,0,0,0,0,0},
		{0x7800,0x0000,0x8800,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0,0,0,0,0,0,0},
		{0x0000,0x0000,0x7800,0x0000,0x3800,0x2800,0x0000,0x4800,0x0000,0,0,0,0,0,0,0},
		{0x3800,0x8800,0x2800,0x1800,0x0000,0x5800,0x6800,0x0000,0x0000,0,0,0,0,0,0,0},
		{0x0000,0x4800,0x1800,0x0000,0x0000,0x9800,0x5800,0x2800,0x0000,0,0,0,0,0,0,0}};

uint16_t cuadricula3[9][16] = {
		{0x9800,0x6800,0x0000,0x0000,0x0000,0x0000,0x7800,0x0000,0x8800,0,0,0,0,0,0,0},
		{0x8800,0x0000,0x0000,0x0000,0x0000,0x4800,0x3800,0x0000,0x0000,0,0,0,0,0,0,0},
		{0x1800,0x0000,0x0000,0x5800,0x0000,0x0000,0x0000,0x0000,0x0000,0,0,0,0,0,0,0},
		{0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x1800,0x7800,0x6800,0,0,0,0,0,0,0},
		{0x2800,0x0000,0x0000,0x0000,0x9800,0x3800,0x0000,0x0000,0x5800,0,0,0,0,0,0,0},
		{0x7800,0x0000,0x8800,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0,0,0,0,0,0,0},
		{0x0000,0x0000,0x7800,0x0000,0x3800,0x2800,0x0000,0x4800,0x0000,0,0,0,0,0,0,0},
		{0x3800,0x8800,0x2800,0x1800,0x0000,0x5800,0x6800,0x0000,0x0000,0,0,0,0,0,0,0},
		{0x0000,0x4800,0x1800,0x0000,0x0000,0x9800,0x5800,0x2800,0x0000,0,0,0,0,0,0,0}};

uint16_t cuadricula4[9][16] = {
		{0x9800,0x6800,0x0000,0x0000,0x0000,0x0000,0x7800,0x0000,0x8800,0,0,0,0,0,0,0},
		{0x8800,0x0000,0x0000,0x0000,0x0000,0x4800,0x3800,0x0000,0x0000,0,0,0,0,0,0,0},
		{0x1800,0x0000,0x0000,0x5800,0x0000,0x0000,0x0000,0x0000,0x0000,0,0,0,0,0,0,0},
		{0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x1800,0x7800,0x6800,0,0,0,0,0,0,0},
		{0x2800,0x0000,0x0000,0x0000,0x9800,0x3800,0x0000,0x0000,0x5800,0,0,0,0,0,0,0},
		{0x7800,0x0000,0x8800,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0,0,0,0,0,0,0},
		{0x0000,0x0000,0x7800,0x0000,0x3800,0x2800,0x0000,0x4800,0x0000,0,0,0,0,0,0,0},
		{0x3800,0x8800,0x2800,0x1800,0x0000,0x5800,0x6800,0x0000,0x0000,0,0,0,0,0,0,0},
		{0x0000,0x4800,0x1800,0x0000,0x0000,0x9800,0x5800,0x2800,0x0000,0,0,0,0,0,0,0}};

uint16_t cuadricula5[9][16] = {
		{0x9800,0x6800,0x0000,0x0000,0x0000,0x0000,0x7800,0x0000,0x8800,0,0,0,0,0,0,0},
		{0x8800,0x0000,0x0000,0x0000,0x0000,0x4800,0x3800,0x0000,0x0000,0,0,0,0,0,0,0},
		{0x1800,0x0000,0x0000,0x5800,0x0000,0x0000,0x0000,0x0000,0x0000,0,0,0,0,0,0,0},
		{0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x1800,0x7800,0x6800,0,0,0,0,0,0,0},
		{0x2800,0x0000,0x0000,0x0000,0x9800,0x3800,0x0000,0x0000,0x5800,0,0,0,0,0,0,0},
		{0x7800,0x0000,0x8800,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0,0,0,0,0,0,0},
		{0x0000,0x0000,0x7800,0x0000,0x3800,0x2800,0x0000,0x4800,0x0000,0,0,0,0,0,0,0},
		{0x3800,0x8800,0x2800,0x1800,0x0000,0x5800,0x6800,0x0000,0x0000,0,0,0,0,0,0,0},
		{0x0000,0x4800,0x1800,0x0000,0x0000,0x9800,0x5800,0x2800,0x0000,0,0,0,0,0,0,0}};

inline void celda_poner_valor(CELDA *celdaptr, uint8_t val){
	*celdaptr = (*celdaptr & 0x0FFF) | ((val & 0x000F) << 12);
}
inline uint8_t celda_leer_valor(CELDA celda){
	return celda >> 12;
}



extern int sudoku_recalcular_arm(CELDA cuadricula[NUM_FILAS][NUM_COLUMNAS]);     //funcion a implementar en ARM
extern int sudoku_candidatos_arm(CELDA cuadricula[NUM_FILAS][NUM_COLUMNAS],int fila, int columna);	//funcion a  implementar en ARM
extern int sudoku_candidatos_thumb(CELDA cuadricula[NUM_FILAS][NUM_COLUMNAS],int fila, int columna);
extern int sudoku_recalcular_arm_c(CELDA cuadricula[NUM_FILAS][NUM_COLUMNAS]);
extern int sudoku_recalcular_arm_thumb(CELDA cuadricula[NUM_FILAS][NUM_COLUMNAS]);

extern void timer2_inicializar();
extern int timer2_leer();
extern void timer2_empezar();



////////////////////////////////////////////////////////////////////////////////
// dada una determinada celda encuentra los posibles valores candidatos y guarda el mapa de bits en la celda
// retorna false si la celda esta vacia, true si contiene un valor
int sudoku_candidatos_c(CELDA cuadricula[NUM_FILAS][NUM_COLUMNAS], uint8_t fila, uint8_t columna) {
	int i;
	uint16_t numero= 1;
	int valorCelda; //Valor de la celda
	uint8_t inicioI, inicioJ; //Valores desde los cuales tenemos que empezar a recorrer la cuadricula
	int hastaFila, hastaColumna; //Valores hasta los cuales tenemos que recorrer la cuadrícula

	cuadricula[fila][columna] &= 0xFE00;	//ponemos los candidatos a 0
	//recorrer fila recalculando candidatos
	for(i=0;i<9;i++){	//9 es el límite del sudoku (ya que NUM_COLUMNAS está definido incluído el padding
		if(columna != i){ //si estamos en nuestra misma celda, ignoramos nuestro número
			valorCelda=celda_leer_valor(cuadricula[fila][i]);
			if(valorCelda != 0 ){	//si la celda esta vacía no es candidato
				//Si la celda no está vacía ponemos el bit correspondiente del NO candidato a 1
				numero = numero << (valorCelda-1);
				cuadricula[fila][columna] = cuadricula[fila][columna] | numero;
				numero = 1;
			}
		}
	}
	//recorer columna recalculando candidatos
	for(i=0;i<NUM_FILAS;i++){
			if(fila != i){	//si estamos en nuestra misma celda, ignoramos nuestro número
				valorCelda=celda_leer_valor(cuadricula[i][columna]);
				if(valorCelda != 0 ){	//si la celda esta vacía no es candidato
					//Si la celda no está vacía ponemos el bit correspondiente del NO candidato a 1
					numero = numero << (valorCelda-1);
					cuadricula[fila][columna] = cuadricula[fila][columna] | numero;
					numero = 1;
				}
			}
		}
	//recorrer region recalculando candidatos
	//Calculamos los valores de inicio de la cuadricula
	inicioI= lookup_table[fila];	//posicion de inicio de cuadricula (filas)
	inicioJ= lookup_table[columna];	//posicion de inicio de cuadricula (columnas)
	hastaFila= inicioI+2;	//posicion de fin de nuestra cuadricula (filas)
	hastaColumna= inicioJ+2;	//posicion de fin de nuestra cuadricula (columnas)

	//recorremos la cuadrícula poniendo los bits de los NO candidatos a 1
	for (;inicioI<=(hastaFila);inicioI++){
		for(inicioJ= lookup_table[columna];inicioJ<=(hastaColumna);inicioJ++){
			if(inicioI != fila || inicioJ != columna){
				valorCelda=celda_leer_valor(cuadricula[inicioI][inicioJ]);
							if(valorCelda != 0 ){
								numero = numero << (valorCelda-1);
								cuadricula[fila][columna] = cuadricula[fila][columna] | numero;
								numero = 1;
							}
			}
		}
	}
	uint16_t aux = cuadricula[fila][columna] & 0xF000; //tenemos el valor de la celda añadida
	//miramos si es pista (si es pista no es error)
	if (cuadricula[fila][columna] & 0x0800){
		return TRUE;
	}
	else if (aux==0){	//celda vacía
		return FALSE;
	}
	else{ //Celda llena que no es pista (celda que he llenado)
		//Marcamos el bit de error a 1 en el caso de que el numero introducido
		//es no candidato
		numero = 1;


		aux = aux >> 12;		//desplazo el numero de bits que corresponden al valor de la celda
		numero = numero << (aux-1);
		if((cuadricula[fila][columna] & numero) == numero){	//miro si he añadido un número que no era candidato
			cuadricula[fila][columna] |= 0x0400;	//en ese caso pongo el bit de error a 1
			return -1;
		}
		else{
			return TRUE;
		}
	}
}

////////////////////////////////////////////////////////////////////////////////
// recalcula todo el tablero (9x9)
// retorna el numero de celdas vacias
int sudoku_recalcular_c(CELDA cuadricula[NUM_FILAS][NUM_COLUMNAS]){
	int res;
	int i,j;
	int contadorVacias = 0;	//cuenta el numero de celdas vacias
	int contadorErrores = 0; //cuenta el numero de errores
	//para cada fila
	for(i=0;i<NUM_FILAS;i++){
		//para cada columna
		for(j=0;j<9;j++){
			//determinar candidatos
			res = sudoku_candidatos_c(cuadricula,i,j);
			if (res == 0){
				contadorVacias++;  	//actualizar contador de celdas vacias
			}
			else if(res == -1){
				contadorErrores--;	//actualizar errores
			}
		}
	}
	//si hay errores devolvemos contadorErrores
	if(contadorErrores<0){
		return contadorErrores;
	}
	else{
		//retornar el numero de celdas vacias
		return contadorVacias;
	}
}

//C-ARM, funcion recalcular en codigo c, funcion candidatos en ARM
int sudoku_recalcular_c_arm(CELDA cuadricula[NUM_FILAS][NUM_COLUMNAS]) {
	int res;
	int i,j;
	int contadorVacias = 0;	//cuenta el numero de celdas vacias
	int contadorErrores = 0;	//cuenta el numero de errores
	//para cada fila
	for(i=0;i<NUM_FILAS;i++){
		//para cada columna
		for(j=0;j<9;j++){
			//determinar candidatos
			res = sudoku_candidatos_c(cuadricula,i,j);
			if (res == 0){
				contadorVacias++;  	//actualizar contador de celdas vacias
			}
			else if(res == -1){
				contadorErrores--;	//actualizar errores
			}
		}
	}
	//si hay errores devolvemos contadorErrores
	if(contadorErrores<0){
		return contadorErrores;
	}
	else{
		//retornar el numero de celdas vacias
		return contadorVacias;
	}
}

//C-THUMB, funcion recalcular en codigo c, funcion candidatos en THUMB
int sudoku_recalcular_c_thumb(CELDA cuadricula[NUM_FILAS][NUM_COLUMNAS]) {
	int res;
	int i,j;
	int contadorVacias = 0;	//cuenta el numero de celdas vacias
	int contadorErrores = 0;	//cuenta el numero de errores
	//para cada fila
	for(i=0;i<NUM_FILAS;i++){
		//para cada columna
		for(j=0;j<9;j++){
			//determinar candidatos
			res = sudoku_candidatos_c(cuadricula,i,j);
			if (res == 0){
				contadorVacias++;  	//actualizar contador de celdas vacias
			}
			else if(res == -1){
				contadorErrores--;
			}
		}
	}
	if(contadorErrores<0){
		return contadorErrores;
	}
	else{
		//retornar el numero de celdas vacias
		return contadorVacias;
	}
}

//Compara dos cuadriculas pasadas por parámetro, devolviendo 1 si no son identicas y 0 si lo son
int compararCuadriculas (CELDA cuadricula[NUM_FILAS][NUM_COLUMNAS],CELDA cuadricula1[NUM_FILAS][NUM_COLUMNAS]){
	int i,j;
		int error = FALSE;	//si hay error, error=1, por lo tanto inicializamos a 0
		i=0;
		//para cada fila
		while(i<NUM_FILAS && !error){
			j=0;
			while(j<9 && !error){
				if(cuadricula[i][j] != cuadricula1[i][j]){
					error = TRUE;
				}
				j++;
			}
			i++;
		}
		return error;
}

void poner_valor(int fila, int columna, int valor){
	int celdas_vacias;
	if ((valor == 0) && (cuadriculaSol[fila][columna] & 0x0800) != 0x0800) {	//ponemos el valor a 0 y el bit de error a 0
		cuadriculaSol[fila][columna] &= 0x0BFF;
		cuadricula1[fila][columna] &= 0x0BFF;
		cuadricula2[fila][columna] &= 0x0BFF;
		cuadricula3[fila][columna] &= 0x0BFF;
		cuadricula4[fila][columna] &= 0x0BFF;
		cuadricula5[fila][columna] &= 0x0BFF;


	}
 	else if ((cuadriculaSol[fila][columna] & 0x0800) != 0x0800) {	//si no es pista añado valor
			valor = valor << 12;
			cuadriculaSol[fila][columna] |= valor;
			cuadricula1[fila][columna] |= valor;
			cuadricula2[fila][columna] |= valor;
			cuadricula3[fila][columna] |= valor;
			cuadricula4[fila][columna] |= valor;
			cuadricula5[fila][columna] |= valor;
	}
	//cuando pongo el valor recalculo
	/*celdas_vacias = sudoku_recalcular_c(cuadriculaSol);
	celdas_vacias = sudoku_recalcular_arm(cuadricula1);
	celdas_vacias = sudoku_recalcular_c_arm(cuadricula2);
	celdas_vacias = sudoku_recalcular_arm_c(cuadricula3);
	celdas_vacias = sudoku_recalcular_c_thumb(cuadricula4);
	celdas_vacias = sudoku_recalcular_arm_thumb(cuadricula5);*/
}

void restaurarCuadricula(){
	int i,j;
	for(i=0;i<9;i++){
		for(j=0;j<9;j++){
			cuadriculaSol[i][j] = cuadriculaIni[i][j];
		}
	}
}


////////////////////////////////////////////////////////////////////////////////
// proceso principal del juego que recibe el tablero,
// y la señal de ready que indica que se han actualizado fila y columna
void sudoku9x9() {
	int celdas_vacias;	 //numero de celdas aun vacias
	int cuenta,resultado1,resultado2;

	//variables indicadas para ver que los tableros resultantes de las diferentes funciones son identicas
	uint8_t arm,armC,Carm,Cthumb,armTHUMB;

	//timer2_inicializar();
	//resultado1=timer2_leer();
	celdas_vacias = sudoku_recalcular_c(cuadriculaSol);	//C-C
	//resultado2=timer2_leer();
	//cuenta = resultado2 - resultado1;

	//completar tiempos

	//timer2_empezar();
	//resultado1=timer2_leer();
	celdas_vacias = sudoku_recalcular_arm(cuadricula1);					//ARM-ARM
	//resultado2=timer2_leer();
	//cuenta = resultado2 - resultado1;

	//timer2_empezar();
	//resultado1=timer2_leer();
	celdas_vacias = sudoku_recalcular_c_arm(cuadricula2);				//C-ARM
	//resultado2=timer2_leer();
	//cuenta = resultado2 - resultado1;

	//timer2_empezar();
	//resultado1=timer2_leer();
	celdas_vacias = sudoku_recalcular_arm_c(cuadricula3);				//ARM-C
	//resultado2=timer2_leer();
	//cuenta = resultado2 - resultado1;

	//timer2_empezar();
	//resultado1=timer2_leer();
	celdas_vacias = sudoku_recalcular_c_thumb(cuadricula4);				//C-THUMB
	//resultado2=timer2_leer();
	//cuenta = resultado2 - resultado1;

	//timer2_empezar();
	//resultado1=timer2_leer();
	celdas_vacias = sudoku_recalcular_arm_thumb(cuadricula5);			//ARM-THUMB
	//resultado2=timer2_leer();
	//cuenta = resultado2 - resultado1;

	/*if(!compararCuadriculas(cuadriculaSol,cuadricula1)){
		arm=1;	//tableros identicos
	}
	if(!compararCuadriculas(cuadriculaSol,cuadricula2)){
		armC=1;	//tableros identicos
	}
	if(!compararCuadriculas(cuadriculaSol,cuadricula3)){
		Carm=1;	//tableros identicos
	}
	if(!compararCuadriculas(cuadriculaSol,cuadricula4)){
		Cthumb=1;	//tableros identicos
	}
	if(!compararCuadriculas(cuadriculaSol,cuadricula5)){
		armTHUMB=1;	//tableros identicos
	}*/
}

