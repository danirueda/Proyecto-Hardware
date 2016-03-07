/*********************************************************************************************
* Fichero:	lcd.c
* Autor:	
* Descrip:	funciones de visualizacion y control LCD
* Version:	<P6-ARM>
*********************************************************************************************/

/*--- ficheros de cabecera ---*/
#include "def.h"
#include "44b.h"
#include "44blib.h"
#include "lcd.h"
#include "bmp.h"
#include "elementosComunes.h"
#include<inttypes.h>



/*--- definicion de macros ---*/
#define DMA_Byte  (0)
#define DMA_HW    (1)
#define DMA_Word  (2)
#define DW 		  DMA_Byte		//configura  ZDMA0 como media palabras
	
/*--- variables externas ---*/
extern INT8U g_auc_Ascii8x16[];
extern STRU_BITMAP Stru_Bitmap_gbMouse;
extern uint16_t cuadriculaSol[][16]; //cuadricula de la solucion

/*--- lookUp table para imprimir los numeros por pantalla ---*/
const INT8U *num_ascii[10] = {"0","1","2","3","4","5","6","7","8","9"};

/*--- para dibujar el punto de candidato ---*/
const INT8U pixeles[] = {BLACK,BLACK,BLACK,BLACK,BLACK,BLACK,BLACK,BLACK
		,BLACK,BLACK,BLACK,BLACK,BLACK,BLACK,BLACK,BLACK};

const STRU_BITMAP punto = {0x10, 4, 4, 4, TRANSPARENCY, (INT8U *)pixeles};
/*-------------------------------------------*/

/*--- codigo de la funcion ---*/
void Lcd_Init(void)
{       
	rDITHMODE=0x1223a;
	rDP1_2 =0x5a5a;      
	rDP4_7 =0x366cd9b;
	rDP3_5 =0xda5a7;
	rDP2_3 =0xad7;
	rDP5_7 =0xfeda5b7;
	rDP3_4 =0xebd7;
	rDP4_5 =0xebfd7;
	rDP6_7 =0x7efdfbf;

	rLCDCON1=(0)|(1<<5)|(MVAL_USED<<7)|(0x0<<8)|(0x0<<10)|(CLKVAL_GREY16<<12);
	rLCDCON2=(LINEVAL)|(HOZVAL<<10)|(10<<21); 
	rLCDSADDR1= (0x2<<27) | ( ((LCD_ACTIVE_BUFFER>>22)<<21 ) | M5D(LCD_ACTIVE_BUFFER>>1));
 	rLCDSADDR2= M5D(((LCD_ACTIVE_BUFFER+(SCR_XSIZE*LCD_YSIZE/2))>>1)) | (MVAL<<21);
	rLCDSADDR3= (LCD_XSIZE/4) | ( ((SCR_XSIZE-LCD_XSIZE)/4)<<9 );
	// enable,4B_SNGL_SCAN,WDLY=8clk,WLH=8clk,
	rLCDCON1=(1)|(1<<5)|(MVAL_USED<<7)|(0x3<<8)|(0x3<<10)|(CLKVAL_GREY16<<12);
	rBLUELUT=0xfa40;
	//Enable LCD Logic and EL back-light.
	rPDATE=rPDATE&0x0e;
	
	//DMA ISR
	rINTMSK &= ~(BIT_GLOBAL|BIT_ZDMA0);
    pISR_ZDMA0=(int)Zdma0Done;
}

/*********************************************************************************************
* name:		Lcd_Active_Clr()
* func:		clear LCD screen
* para:		none 
* ret:		none
* modify:
* comment:		
*********************************************************************************************/
void Lcd_Active_Clr(void)
{
	INT32U i;
	INT32U *pDisp = (INT32U *)LCD_ACTIVE_BUFFER;
	
	for( i = 0; i < (SCR_XSIZE*SCR_YSIZE/2/4); i++ )
	{
		*pDisp++ = WHITE;
	}
}

/*********************************************************************************************
* name:		Lcd_GetPixel()
* func:		Get appointed point's color value
* para:		usX,usY -- pot's X-Y coordinate 
* ret:		pot's color value
* modify:
* comment:		
*********************************************************************************************/
INT8U LCD_GetPixel(INT16U usX, INT16U usY)
{
	INT8U ucColor;

	ucColor = *((INT8U*)(LCD_VIRTUAL_BUFFER + usY*SCR_XSIZE/2 + usX/8*4 + 3 - (usX%8)/2));
	ucColor = (ucColor >> ((1-(usX%2))*4)) & 0x0f;
	return ucColor;
}

/*********************************************************************************************
* name:		Lcd_Active_Clr()
* func:		clear virtual screen
* para:		none 
* ret:		none
* modify:
* comment:		
*********************************************************************************************/
void Lcd_Clr(void)
{
	INT32U i;
	INT32U *pDisp = (INT32U *)LCD_VIRTUAL_BUFFER;
	
	for( i = 0; i < (SCR_XSIZE*SCR_YSIZE/2/4); i++ )
	{
		*pDisp++ = WHITE;
	}
}

/*********************************************************************************************
* name:		LcdClrRect()
* func:		fill appointed area with appointed color
* para:		usLeft,usTop,usRight,usBottom -- area's rectangle acme coordinate
*			ucColor -- appointed color value
* ret:		none
* modify:
* comment:	also as clear screen function 
*********************************************************************************************/
void LcdClrRect(INT16 usLeft, INT16 usTop, INT16 usRight, INT16 usBottom, INT8U ucColor)
{
	INT16 i,k,l,m;
	
	INT32U ulColor = (ucColor << 28) | (ucColor << 24) | (ucColor << 20) | (ucColor << 16) | 
				     (ucColor << 12) | (ucColor << 8) | (ucColor << 4) | ucColor;

	i = k = l = m = 0;	
	if( (usRight-usLeft) <= 8 )
	{
		for( i=usTop; i<=usBottom; i++)
		{
			for( m=usLeft; m<=usRight; m++)
			{
				LCD_PutPixel(m, i, ucColor);
			}
		}	
		return;
	}

	/* check borderline */
	if( 0 == (usLeft%8) )
		k=usLeft;
	else
	{
		k=(usLeft/8)*8+8;
	}
	if( 0 == (usRight%8) )
		l= usRight;
	else
	{
		l=(usRight/8)*8;
	}

	for( i=usTop; i<=usBottom; i++ )
	{
		for( m=usLeft; m<=(k-1); m++ )
		{
			LCD_PutPixel(m, i, ucColor);
		}
		for( m=k; m<l; m+=8 )
		{
			(*(INT32U*)(LCD_VIRTUAL_BUFFER + i * SCR_XSIZE / 2 + m / 2)) = ulColor;
		}
		for( m=l; m<=usRight; m++ )
		{
			LCD_PutPixel(m, i, ucColor);
		}
	}
}

/*********************************************************************************************
* name:		Lcd_Draw_Box()
* func:		Draw rectangle with appointed color
* para:		usLeft,usTop,usRight,usBottom -- rectangle's acme coordinate
*			ucColor -- appointed color value
* ret:		none
* modify:
* comment:		
*********************************************************************************************/
void Lcd_Draw_Box(INT16 usLeft, INT16 usTop, INT16 usRight, INT16 usBottom, INT8U ucColor, INT16U usWidth)
{
	Lcd_Draw_HLine(usLeft, usRight,  usTop,    ucColor, usWidth);
	Lcd_Draw_HLine(usLeft, usRight,  usBottom, ucColor, usWidth);
	Lcd_Draw_VLine(usTop,  usBottom, usLeft,   ucColor, usWidth);
	Lcd_Draw_VLine(usTop,  usBottom, usRight,  ucColor, usWidth);
}

/*********************************************************************************************
* name:		Lcd_Draw_Line()
* func:		Draw line with appointed color
* para:		usX0,usY0 -- line's start point coordinate
*			usX1,usY1 -- line's end point coordinate
*			ucColor -- appointed color value
*			usWidth -- line's width
* ret:		none
* modify:
* comment:		
*********************************************************************************************/
void Lcd_Draw_Line(INT16 usX0, INT16 usY0, INT16 usX1, INT16 usY1, INT8U ucColor, INT16U usWidth)
{
	INT16 usDx;
	INT16 usDy;
	INT16 y_sign;
	INT16 x_sign;
	INT16 decision;
	INT16 wCurx, wCury, wNextx, wNexty, wpy, wpx;

	if( usY0 == usY1 )
	{
		Lcd_Draw_HLine (usX0, usX1, usY0, ucColor, usWidth);
		return;
	}
	if( usX0 == usX1 )
	{
		Lcd_Draw_VLine (usY0, usY1, usX0, ucColor, usWidth);
		return;
	}
	usDx = abs(usX0 - usX1);
	usDy = abs(usY0 - usY1);
	if( ((usDx >= usDy && (usX0 > usX1)) ||
        ((usDy > usDx) && (usY0 > usY1))) )
    {
        GUISWAP(usX1, usX0);
        GUISWAP(usY1, usY0);
    }
    y_sign = (usY1 - usY0) / usDy;
    x_sign = (usX1 - usX0) / usDx;

    if( usDx >= usDy )
    {
        for( wCurx = usX0, wCury = usY0, wNextx = usX1,
             wNexty = usY1, decision = (usDx >> 1);
             wCurx <= wNextx; wCurx++, wNextx--, decision += usDy )
        {
            if( decision >= usDx )
            {
                decision -= usDx;
                wCury += y_sign;
                wNexty -= y_sign;
            }
            for( wpy = wCury - usWidth / 2;
                 wpy <= wCury + usWidth / 2; wpy++ )
            {
                LCD_PutPixel(wCurx, wpy, ucColor);
            }

            for( wpy = wNexty - usWidth / 2;
                 wpy <= wNexty + usWidth / 2; wpy++ )
            {
                LCD_PutPixel(wNextx, wpy, ucColor);
            }
        }
    }
    else
    {
        for( wCurx = usX0, wCury = usY0, wNextx = usX1,
             wNexty = usY1, decision = (usDy >> 1);
             wCury <= wNexty; wCury++, wNexty--, decision += usDx )
        {
            if( decision >= usDy )
            {
                decision -= usDy;
                wCurx += x_sign;
                wNextx -= x_sign;
            }
            for( wpx = wCurx - usWidth / 2;
                 wpx <= wCurx + usWidth / 2; wpx++ )
            {
                LCD_PutPixel(wpx, wCury, ucColor);
            }

            for( wpx = wNextx - usWidth / 2;
                 wpx <= wNextx + usWidth / 2; wpx++ )
            {
                LCD_PutPixel(wpx, wNexty, ucColor);
            }
        }
    }
}

/*********************************************************************************************
* name:		Lcd_Draw_HLine()
* func:		Draw horizontal line with appointed color
* para:		usX0,usY0 -- line's start point coordinate
*			usX1 -- line's end point X-coordinate
*			ucColor -- appointed color value
*			usWidth -- line's width
* ret:		none
* modify:
* comment:		
*********************************************************************************************/
void Lcd_Draw_HLine(INT16 usX0, INT16 usX1, INT16 usY0, INT8U ucColor, INT16U usWidth)
{
	INT16 usLen;

    if( usX1 < usX0 )
    {
        GUISWAP (usX1, usX0);
    }

    while( (usWidth--) > 0 )
    {
        usLen = usX1 - usX0 + 1;
        while( (usLen--) > 0 )
        {
        	LCD_PutPixel(usX0 + usLen, usY0, ucColor);
        }
        usY0++;
    }
}

/*********************************************************************************************
* name:		Lcd_Draw_VLine()
* func:		Draw vertical line with appointed color
* para:		usX0,usY0 -- line's start point coordinate
*			usY1 -- line's end point Y-coordinate
*			ucColor -- appointed color value
*			usWidth -- line's width
* ret:		none
* modify:
* comment:		
*********************************************************************************************/
void Lcd_Draw_VLine (INT16 usY0, INT16 usY1, INT16 usX0, INT8U ucColor, INT16U usWidth)
{
	INT16 usLen;

    if( usY1 < usY0 )
    {
        GUISWAP (usY1, usY0);
    }

    while( (usWidth--) > 0 )
    {
        usLen = usY1 - usY0 + 1;
        while( (usLen--) > 0 )
        {
        	LCD_PutPixel(usX0, usY0 + usLen, ucColor);
        }
        usX0++;
    }
}

/*********************************************************************************************
* name:		Lcd_DspAscII8x16()
* func:		display 8x16 ASCII character string 
* para:		usX0,usY0 -- ASCII character string's start point coordinate
*			ForeColor -- appointed color value
*			pucChar   -- ASCII character string
* ret:		none
* modify:
* comment:		
*********************************************************************************************/
void Lcd_DspAscII8x16(INT16U x0, INT16U y0, INT8U ForeColor, INT8U * s)
{
	INT16 i,j,k,x,y,xx;
	INT8U qm;
	INT32U ulOffset;
	INT8 ywbuf[16],temp[2];
    
	for( i = 0; i < strlen((const char*)s); i++ )
	{
		if( (INT8U)*(s+i) >= 161 )
		{
			temp[0] = *(s + i);
			temp[1] = '\0';
			return;
		}
		else
		{
			qm = *(s+i);
			ulOffset = (INT32U)(qm) * 16;		//Here to be changed tomorrow
			for( j = 0; j < 16; j ++ )
			{
				ywbuf[j] = g_auc_Ascii8x16[ulOffset + j];
            }

            for( y = 0; y < 16; y++ )
            {
            	for( x = 0; x < 8; x++ ) 
               	{
                	k = x % 8;
			    	if( ywbuf[y]  & (0x80 >> k) )
			       	{
			       		xx = x0 + x + i*8;
			       		LCD_PutPixel(xx, y + y0, (INT8U)ForeColor);
			       	}
			   	}
            }
		}
	}
}

/*********************************************************************************************
* name:		ReverseLine()
* func:		Reverse display some lines 
* para:		ulHeight -- line's height
*			ulY -- line's Y-coordinate
* ret:		none
* modify:
* comment:		
*********************************************************************************************/
void ReverseLine(INT32U ulHeight, INT32U ulY)
{
	INT32U i, j, temp;
	
	for( i = 0; i < ulHeight; i++ )
	{
		for( j = 0; j < (SCR_XSIZE/4/2) ; j++ )
		{
			temp = *(INT32U*)(LCD_VIRTUAL_BUFFER + (ulY+i)*SCR_XSIZE/2 + j*4);
			temp ^= 0xFFFFFFFF;
			*(INT32U*)(LCD_VIRTUAL_BUFFER + (ulY+i)*SCR_XSIZE/2 + j*4) = temp;
		}
	}
}

/*********************************************************************************************
* name:		Zdma0Done()
* func:		LCD dma interrupt handle function
* para:		none
* ret:		none
* modify:
* comment:		
*********************************************************************************************/
static INT8U ucZdma0Done=1;	//When DMA is finish,ucZdma0Done is cleared to Zero
void Zdma0Done(void)
{
	rI_ISPC=BIT_ZDMA0;	    //clear pending
	ucZdma0Done=0;
}

/*********************************************************************************************
* name:		Lcd_Dma_Trans()
* func:		dma transport virtual LCD screen to LCD actual screen
* para:		none
* ret:		none
* modify:
* comment:		
*********************************************************************************************/
void Lcd_Dma_Trans(void)
{
	INT8U err;
	
	ucZdma0Done=1;
	//#define LCD_VIRTUAL_BUFFER	(0xc400000)
	//#define LCD_ACTIVE_BUFFER	(LCD_VIRTUAL_BUFFER+(SCR_XSIZE*SCR_YSIZE/2))	//DMA ON
	//#define LCD_ACTIVE_BUFFER	LCD_VIRTUAL_BUFFER								//DMA OFF
	//#define LCD_BUF_SIZE		(SCR_XSIZE*SCR_YSIZE/2)
	//So the Lcd Buffer Low area is from LCD_VIRTUAL_BUFFER to (LCD_ACTIVE_BUFFER+(SCR_XSIZE*SCR_YSIZE/2))
	rNCACHBE1=(((unsigned)(LCD_ACTIVE_BUFFER)>>12) <<16 )|((unsigned)(LCD_VIRTUAL_BUFFER)>>12);
  	rZDISRC0=(DW<<30)|(1<<28)|LCD_VIRTUAL_BUFFER; // inc
  	rZDIDES0=( 2<<30)  |(1<<28)|LCD_ACTIVE_BUFFER; // inc
        rZDICNT0=( 2<<28)|(1<<26)|(3<<22)|(0<<20)|(LCD_BUF_SIZE);
        //                      |            |            |             |            |---->0 = Disable DMA
        //                      |            |            |             |------------>Int. whenever transferred
        //                      |            |            |-------------------->Write time on the fly
        //                      |            |---------------------------->Block(4-word) transfer mode
        //                      |------------------------------------>whole service
	//reEnable ZDMA transfer
  	rZDICNT0 |= (1<<20);		//after ES3
    rZDCON0=0x1; // start!!!  

	//Delay(500);
	while(ucZdma0Done);		//wait for DMA finish
}

/*
 * Funcion utilizada para dibujar la pantalla inicial del juego
 */
void Lcd_inicio(void)
{
	/* initial LCD controller */
	Lcd_Init();
	/* clear screen */
	Lcd_Clr();
	Lcd_Active_Clr();

	/* draw rectangle pattern */ 
    #ifdef Eng_v // english version
	Lcd_DspAscII8x16(10,0,DARKGRAY,"Embest S3CEV40 ");
	#else
	//Lcd_DspHz16(10,0,DARKGRAY,"英蓓特三星实验评估板");
	#endif
	Lcd_DspAscII8x16(0,0,BLACK,"#########################################");
	Lcd_DspAscII8x16(0,20,BLACK,"##########SUDOKU PH 2015-2016###########");
	Lcd_DspAscII8x16(0,40,BLACK,"########################################");
	Lcd_DspAscII8x16(0,70,BLACK,"Instrucciones del juego: ");
	Lcd_DspAscII8x16(0,90,BLACK,"1. Seleccione la fila");
	Lcd_DspAscII8x16(0,110,BLACK,"2. Seleccione la columna");
	Lcd_DspAscII8x16(0,130,BLACK,"3. Seleccione el valor");
	Lcd_DspAscII8x16(0,150,BLACK,"Boton izquierdo: Aumentar valor");
	Lcd_DspAscII8x16(0,170,BLACK,"Boton derecho: Confirmar");
	Lcd_DspAscII8x16(0,190,BLACK,"Pulse cualquier boton para iniciar");
	Lcd_DspAscII8x16(0,200,BLACK,"el juego.");
	Lcd_DspAscII8x16(0,220,BLACK,"Adrian Susinos y Daniel Rueda");
	Lcd_Dma_Trans();
}

/*
 * Funcion utilizada para pintar el tablero y la  informacion del juego a la derecha
 */
void pintarSudoku(void){
	int incremento = 0; 	//variable utilizada para poner los numeritos de fila y columna
	int i;
	Lcd_Clr();
	Lcd_Active_Clr();
	/* pintamos la cuadricula externa del tablero del sudoku*/
	Lcd_Draw_Box(inicioTablero_izquierda,inicioTablero_arriba,anchura_tablero,altura_tablero,15,1);
	Lcd_Dma_Trans();
	for(i=0; i<9;i++){
		/* pintamos los indices del sudoku */
		Lcd_DspAscII8x16(27+separacion_numeros*i,0,BLACK,num_ascii[i+1]);
		Lcd_DspAscII8x16(4,25+separacion_numeros*i,BLACK,num_ascii[i+1]);
		/* pintamos las lineas de las filas y las columnas del tablero y distinguimos de las lineas que definen
		 * las cuadriculas poniendoles a estas mas grosor*/
		if(i%3==0){ //lineas de cuadricula (linea gruesa)
			Lcd_Draw_VLine(inicioTablero_arriba,  altura_tablero, inicioTablero_izquierda+i*separacion_lineas, 15, 1);
			Lcd_Draw_HLine(inicioTablero_izquierda,  anchura_tablero, inicioTablero_arriba+i*separacion_lineas, 15, 1);
		}
		else { //lineas normales (lineas finas)
			Lcd_Draw_VLine(inicioTablero_arriba,  altura_tablero, inicioTablero_izquierda+i*separacion_lineas, 10, 1);
			Lcd_Draw_HLine(inicioTablero_izquierda,  anchura_tablero, inicioTablero_arriba+i*separacion_lineas, 10, 1);
		}
	}
	/* Informacion e instrucciones de la partida */
	Lcd_DspAscII8x16(225,20,BLACK,"Tiempo de");
	Lcd_DspAscII8x16(225,31,BLACK,"partida (s)");
	Lcd_DspAscII8x16(225,85,BLACK,"Tiempo de");
	Lcd_DspAscII8x16(225,96,BLACK,"calculo (ms)");
	Lcd_DspAscII8x16(228,150,BLACK,"Introduzca");
	Lcd_DspAscII8x16(225,161,BLACK,"fila A para");
	Lcd_DspAscII8x16(232,172,BLACK,"acabar la");
	Lcd_DspAscII8x16(240,183,BLACK,"partida");
	Lcd_Dma_Trans();
}

/* Funcion utilizada para pintar los numeros dentro del tablero */
void pintarNumeros(void){
	uint16_t numeroPintar, aux;
	int i,j,k;
	int izq, arriba;
	for(i=0;i<9;i++){ //bucle para recorrer el sudoku
		for(j=0;j<9;j++){
			if((cuadriculaSol[i][j] & 0xF000) == 0){ //si la celda celda vac韆
				aux = cuadriculaSol[i][j] & 0x01FF;	//obtenemos bits de posibles candidatos
				k=1;
				while(k<=9){ //bucle para recorrer los bits de candidato
					if((aux & 0x0001) == 0 ){ //sacamos el bit menos significativo
						//si es 0 es que es candidato
						/* Calculamos las coordenadas del punto a pintar*/
						izq = inicioTablero_izquierda + 22 * j;
						arriba = inicioTablero_arriba + 22 * i;
						pintarCandidatos(izq,arriba,k); //pintamos el punto
					}
					aux = aux >> 1; //movemos un bit a la derecha (siguiente posible candidato)
					k++;
				}
			}
			else { //si la celda tiene un numero ya
				if((cuadriculaSol[i][j] & 0x0800) == 0x0800){// si es pista
					numeroPintar = cuadriculaSol[i][j] & 0xF000; //obtenemos el valor
					numeroPintar = numeroPintar >> 12; //desplazamos 12 bits para tener el numero de pista
					/* pintamos en la correspondiente posicion (BLACK grosor normal) */
					Lcd_DspAscII8x16(marIzq_num+separacion_numeros*j, marSup_num+separacion_numeros*i, BLACK, num_ascii[numeroPintar]);
				}
				else { //si no es pista (numero introducido por el usuario)
					numeroPintar = cuadriculaSol[i][j] & 0xF000; //obtenemos el valor
					numeroPintar = numeroPintar >> 12; //desplazamos 12 bits para tener el numero de pista
					/* pintamos en la correspondiente posicion (TRANSPARENCY grosor gordo) */
					Lcd_DspAscII8x16(marIzq_num+separacion_numeros*j, marSup_num+separacion_numeros*i, TRANSPARENCY, num_ascii[numeroPintar]);
					if((cuadriculaSol[i][j] & 0x0400) == 0x0400){ //si el numero que hemos pintado es erroneo
						/* Distinguimos la cuadricula pintandola de un grosor mayor*/
						Lcd_Draw_Box(inicioTablero_izquierda+separacion_lineas*j,inicioTablero_arriba+separacion_lineas*i,
								19+separacion_lineas*(j+1),19+separacion_lineas*(i+1),15,2);
					}
				}
			}
		}
	}
	Lcd_Dma_Trans();
}

/* Funcion para pintar los puntos de los candidatos */
void pintarCandidatos(int izq, int arriba, int k){
/* Dependiendo del numero del candidato a pintar pintamos en una zona u otra de la cuadricula*/
	switch(k){
		case 1:
			BitmapView(izq+3,arriba+3,punto);
			break;
		case 2:
			BitmapView(izq+9,arriba+3,punto);
			break;
		case 3:
			BitmapView(izq+15,arriba+3,punto);
			break;
		case 4:
			BitmapView(izq+3,arriba+9,punto);
			break;
		case 5:
			BitmapView(izq+9,arriba+9,punto);
			break;
		case 6:
			BitmapView(izq+15,arriba+9,punto);
			break;
		case 7:
			BitmapView(izq+3,arriba+15,punto);
			break;
		case 8:
			BitmapView(izq+9,arriba+15,punto);
			break;
		case 9:
			BitmapView(izq+15,arriba+15,punto);
			break;
		default:
			break;
	}
	Lcd_Dma_Trans();
}

/* Funcion utilizada en el momento del final de la partida*/
void finPartida(void){
	Lcd_Clr();
	Lcd_Active_Clr();
	int resultado = sudoku_recalcular_c(cuadriculaSol); //nos guardamos el resultado del recalcular sudoku
	if(resultado<0){ //si hay errores
		Lcd_DspAscII8x16(100,0,BLACK,"Tablero con errores");
	}
	else if(resultado==0){ //si el tablero lleno y sin errores
		Lcd_DspAscII8x16(100,100,BLACK,"Tablero completo");
	}
	else{ //si el tablero sin completar y sin errores
		Lcd_DspAscII8x16(100,100,BLACK,"Tablero incompleto");
	}
	Lcd_DspAscII8x16(70,120,BLACK,"Pulse un boton para jugar");
	Lcd_Dma_Trans();
}

