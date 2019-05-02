
#ifndef XC_BLINK_H
#define	XC_BLINK_H
//*************************************************************
//  Libreria para parpadeo de led  " BLINK "                  *
//                                                            * 
//  CovraWorkS-- CWS---                                       *
//  v1.00  2019                                               *
//  para XC8 v2.05                                            *
// Incluir en el main: #include"blink.h"                      *
// NOTAS: - Cambiar el NOMBRE (x2) del led                    *
//        - Establecer frecuencia oscilador en _XTAL_FREQ     *         
//        - Establecer el tiempo delayTime de parpadeo        *
//        - Definir los bits TRIS y LAT  de ubicacion del led *
//                                                            *
//*************************************************************


#include <xc.h> // include processor files - each processor file is guarded. 

#define ledblink NOMBRE   //definir nombre
#define NOMBRE LATAbits.LA1 //definir LAT port
#define TRISled TRISAbits.RA1 // definir TRIS port
#define delayTime 400 // definir tiempo de delay en ms
#define _XTAL_FREQ 1000000

void blink (void){
    TRISled = 0; // Pin del led como SALIDA
    ledblink = 1;
    __delay_ms (delayTime);
    ledblink = 0;
    __delay_ms (delayTime);
}
#ifdef	__cplusplus
extern "C" {
#endif /* __cplusplus */

#ifdef	__cplusplus
}
#endif /* __cplusplus */

#endif	/* XC_BLINK_H */
