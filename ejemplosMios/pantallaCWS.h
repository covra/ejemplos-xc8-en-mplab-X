
#ifndef XC_PANTALLACWS_H
#define	XC_PANTALLACWS_H

//*************************************************************
//  Libreria de presentacion de proyecto PANTALLACWS          *
//                                                            * 
//  CovraWorkS-- CWS---                                       *
//  v1.00  2019                                               *
//  para XC8 v2.05                                            *
// Incluir en el main: #include"pantallaCWS.h"                *
// NOTAS: - Invocar la funcion: pantallaCWS(nombreProyecto)   *
//        - Establecer frecuencia oscilador en _XTAL_FREQ     *         
//        - Establecer el tiempo tiempoPres  en LCD           *
//        - Incluir  "flex_lcd.h"                             *
//          si no esta en el mismo directorio                 *
//*************************************************************

#include <xc.h> 
#include "flex_lcd.h"

#define tiempoPres 500  //Tiempo de permanencia en pantalla en ms

void pantallaCWS (const char *stringProyecto){
    Lcd_Init();
    __delay_ms(20);
    Lcd_Out(1, 0, "---CovraWorkS---");	//escribimos en la linea 2 espacio 1 la palabra mkmekatronika
    __delay_ms(tiempoPres);
    Lcd_Out(2, 0, stringProyecto);
    __delay_ms(tiempoPres);
}

#ifdef	__cplusplus
extern "C" {
#endif /* __cplusplus */

    // TODO If C++ is being used, regular C code needs function names to have C 
    // linkage so the functions can be used by the c code. 

#ifdef	__cplusplus
}
#endif /* __cplusplus */

#endif	/* XC_HEADER_TEMPLATE_H */

