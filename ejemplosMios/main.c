/*
 * File:   main.c
 * Author: Grande
 *
 * Created on 6 de mayo de 2019, 9:24
 */

//*********************** INCLUDES/DEFINES  ***************************
#define TRISC0  SQWIn //Pin de entrada de la señal SQWOut del DS1307

#include <xc.h>

#include "blink.h"
#include "flex_lcd.h"
#include "configuracionBytes.h"
#include "pantallaCWS.h"
#include "funcionesDS1307.h"




//******************* DECLARACION DE VARIABLES *********************

__bit flagFin;

//******************* PROGRAMA PRINCIPAL **************************


void main(void) {
    ANSELA = 0;
    ANSELD = 0;
    TRISC0 = 1;
    
    pantallaCWS ("  TEST PROG   ", 2); //Pantalla Inicio CWS
    blink();
    
    puestaHora(9,33,5,18,5,77);  //Prueba puesta en hora
    __delay_ms(20);
    Lcd_Cmd(LCD_CURSOR_OFF);
    sqwOn();
    Lcd_Cmd(LCD_CLEAR);
    while(1){
        flagFin = muestraLcdHora();
        __delay_ms(700);
     }
}
