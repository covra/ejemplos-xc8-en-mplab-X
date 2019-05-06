#ifndef XC_FUNCIONESDS1307_H
#define	XC_FUNCIONESDS1307_H

//*************************************************************
//  Libreria de funciones del RTC DS1307                      *
//  ------------------------------------                      * 
//                                                            *
//  CovraWorkS-- CWS---                                       *
//  v1.00  2019                                               *
//  para XC8 v2.05 C99                                        *
// Incluir en el main: #include "funcionesDS1307.h"           *
// NOTAS: - Invocar la funcion:                               *
//          funcionesDS1307 ()                                *
//        - DEFINIR:(opcional) en "main.c" las siguientes     *
//          variables de tipo:                                *
//           char sec, min, hora, diaSem, dia, mes, year      *
//          DS1307_ADDRESS, por defecto es 0xD0               *  
//        - DEPENDENCIAS:                                     *
//              "flex_lcd.h"                                  *
//               <xc.h>                                       *
//         - NOTAS:                                           *
//                  La salida en el LCD del dia de la semana  *
//          hay que cambiarla manualmente (o eliminarla) desde*
//          el bloque switch-case de la funcion               *
//          "muestraLcdHora ()"                               *
//                 Para poner un led desde SQWOut hay que     *
//                 definir el pin como digital INPUT          *
//                                                            *
//           - EJEMPLO:                                       *
//                                                            *
//             ANSELD = 0; //pines del lcd digital            *
//             TRISCbits.RC0 = 1; //sqwout pin                *
//             puestaHora(9,33,5,18,5,77);//09:33 18/05/1977  *
//             __delay_ms(20);                                *
//             sqwOn(); //SQWOut ON a 1Hz                     *
//             while(1){                                      *
//                flagFin = muestraLcdHora();//Muestra la hora*
//                __delay_ms(700);                            *
//             }                                              *
//                                                            *
//*************************************************************

#include <xc.h> 

#define DS1307_ADDRESS 0xD0  //Direccion del DS1307 por defecto

char sec, min, hora, diaSem, dia, mes, year; //Opcional definirlas en "main.c"

//**************** FUNCIONES DS1307 ***************************************************
enum WeekDays {
DOMINGO=0x01,
LUNES=0x02,
MARTES=0x03,
MIERCOLES=0x04,
JUEVES=0x05,
VIERNES=0x06,
SABADO=0x07
};

enum TimeMode{Mode12Hour=1,Mode24Hour=2};     // 1 = 12H , 2 = 24H

void updateTimeDS1307();
void puestaHora(char Hour, char Minute, char WeekDay, char Date, char Month, char Year);
char* GetLocalTime(enum TimeMode Mode);
char* GetLocalDate();


struct TimeStruct {
  char FormatedTime[9];
  char FormatedDate[10];
  void (*SetTime)(char Hour, char Minute, char WeekDay, char Date, char Month, char Year);
  char* (*GetTime)(enum TimeMode Mode);
  char* (*GetDate)();
  void (*Update)();
  void (*Init)();
 }Time={{'\0'},{'\0'},&puestaHora,&GetLocalTime,&GetLocalDate,&updateTimeDS1307};
  
char* GetLocalDate(){
  return (char*)Time.FormatedDate;
}
  
char* GetLocalTime(enum TimeMode Mode) {       //GetLocalTime (1) ...1 para 12h y 2 para 24h
  signed short x;
  if(Mode==Mode12Hour)
  {
    x=(Time.FormatedTime[1]-'0')+(Time.FormatedTime[0]-'0')*10;
    if(x==0)x+=12;
    else if(x>=13)x-=12;
    Time.FormatedTime[1]=(x%10)+'0';
    Time.FormatedTime[0]=((x/10)%10)+'0';
  }
  return (char*)Time.FormatedTime;
 }

//********************** FUNCIONES I2C1-DS1307 ***************************************

void rtcWrite ( char dataSend)  { //(S - write - P)
    SSP1CON2bits.SEN1 = 1; //Inicia Start condicion
    while (SEN1);         //Espera a que se complete
    PIR1bits.SSP1IF = 0;  //Entonces clear IF
           
    SSP1BUF = dataSend;    // Escribe  el dato seleccionado en el buffer
    while (!SSP1IF)          // Espera por ACK: SSP1F = 1 cada 9º bit
        ;
        PIR1bits.SSP1IF = 0;  //Entonces clear IF
        if (SSP1CON2bits.ACKSTAT1) {
            SSP1CON2bits.PEN1 = 1;  //Inicia la condicion Stop
            while (PEN1);      //Espera que termine la condicion Start
            return;
        }        
}

void initI2C1_driver (void) { //Iniciacion del driver por MCC
    TRISCbits.RC3 = 1; //Lineas SDA SCL como inputs
    TRISCbits.RC4 = 1;
    PORTB;
    __delay_ms(500);
    if(!SSP1CON1bits.SSPEN) //parametros generados por MCC
    {
        SSP1STAT = 0x00; 
        SSP1CON1 = 0x28;
        SSP1CON2 = 0x00;
        SSP1ADD = 3;
    }
}

inline void i2c1_close(void)  //Cierre del i2c1
{
    SSP1CON1bits.SSPEN = 0;
}
inline void i2c1_driver_restart(void)
{
    SSP1CON2bits.RSEN = 1;
}
inline void i2c1_driver_stop(void)
{
    SSP1CON2bits.PEN = 1;
}

void i2c1_restart (void){
      SSP1CON2bits.RSEN1 = 1; //Inicia condicion Restart
      while (RSEN1);    //espera a que termine el restart
      PIR1bits.SSP1IF = 0;  //Entonces clear IF
      __delay_ms(20);
  }


 char i2c1_RXByte (void){ //Recibe 1 byte (requiere S y RS condition)
    char tempBuffer;
    RCEN1 = 1;          //Empieza a recibir
    while (!BF1);       //Salta interrupcion fin rx
    tempBuffer = SSP1BUF;      //lee el buffer
    BF1 = 0;            //reset stat del buffer
    ACKDT1 = 0;         //ACKDT1 = 0;
    ACKEN1 = 1;         //Envia ACK al slave
    PIR1bits.SSP1IF = 0;  
    __delay_ms (20);
    PIR1bits.SSP1IF = 0;//Clear IF
    return tempBuffer;
 }
 

 void puestaHora (char Hour, char Minute, char WeekDay, char Date, char Month, char Year){
    initI2C1_driver(); //Start
    rtcWrite (DS1307_ADDRESS); //DS1307
    rtcWrite (0x00); //
    //rtcWrite (0x80); //
    rtcWrite (0x00); //
    rtcWrite ((((0x00|((Minute/10)%10))<<4)|(Minute%10))); //Minutos
    rtcWrite ((((0x00|((Hour/10)%10))<<4)|(Hour%10))); //Hora
    rtcWrite (WeekDay); //Dia semana, empieza en domingo
    rtcWrite ((((0x00|((Date/10)%10))<<4)|(Date%10))); //Dia del mes
    rtcWrite ((((0x00|((Month/10)%10))<<4)|(Month%10))); //Mes
    rtcWrite ((((0x00|((Year/10)%10))<<4)|(Year%10))); //Año
    i2c1_driver_stop(); //Stop 
    
  }
  

 __bit muestraLcdHora (void){ //Muestra en el LCD la hora 
    updateTimeDS1307(); //Carga las variables al leer todo el RTC
    __delay_ms(20);
    Lcd_Out(2,0,""); //Posiciona el cursor
    Lcd_Chr_CP(Time.FormatedTime[0]); //Hora
    Lcd_Chr_CP(Time.FormatedTime[1]);
    Lcd_Chr_CP(':');
    Lcd_Chr_CP(Time.FormatedTime [3]); //Minutos
    Lcd_Chr_CP(Time.FormatedTime [4]);
    Lcd_Chr_CP(':');
    Lcd_Chr_CP(Time.FormatedTime [6]); //Segundos
    Lcd_Chr_CP(Time.FormatedTime [7]);
    Lcd_Out(1,0,""); //Posiciona el cursor
    Lcd_Chr_CP(Time.FormatedDate[0]); //dia
    Lcd_Chr_CP(Time.FormatedDate[1]);
    Lcd_Chr_CP('/');
    Lcd_Chr_CP(Time.FormatedDate [3]); //mes
    Lcd_Chr_CP(Time.FormatedDate [4]);
    Lcd_Chr_CP('/');
    Lcd_Chr_CP(Time.FormatedDate [6]); //año
    Lcd_Chr_CP(Time.FormatedDate [7]);
    char diaEnviado = Time.FormatedDate [9];
    switch (diaEnviado){
        case 0x02: Lcd_Out(1,9, "Lunes");break;
        case 0x03 : Lcd_Out(1,9, "Martes");break;
        case 0x04 : Lcd_Out(1,9, "Miercoles");break;
        case 0x05 : Lcd_Out(1,9, "Jueves");break;
        case 0x06 : Lcd_Out(1,9, "Viernes");break;
        case 0x07 : Lcd_Out(1,9, "Sabado");break;
        case 0x01 : Lcd_Out(1,9, "Domingo");break;
        }
    
    return 1;
  }
  void sqwOn (void){  //ON 1Hz
      
    initI2C1_driver();//Start
    rtcWrite (0xD0);  //DS1307
    rtcWrite (0x07);  //registro SQW/Out
    rtcWrite (0x90);  //salida SQW/Out 1Hz ON
    i2c1_driver_stop();//Stop  
  }
  
  void sqwOff (void){  //OFF
      
    initI2C1_driver();//Start
    rtcWrite (0xD0); //DS1307
    rtcWrite (0x07); //registro SQW/Out
    rtcWrite (0x80); //salida SQW/Out 1Hz OFF
    i2c1_driver_stop();//Stop  
  }
  

 void updateTimeDS1307 (void){  //Actualiza los registros asociados al rtc
    SSP1CON2bits.SEN = 1;  //Inicia Start condicion
    while (SEN1);          //Espera a que se complete
    PIR1bits.SSP1IF = 0;   //Entonces clear IF
    rtcWrite (0xD0);       //Llama a la direccion del rtc
    rtcWrite (0x00);       // lo activa (datasheet)
    i2c1_driver_restart(); //Rs condition
    while (RSEN1);
    PIR1bits.SSP1IF = 0;   //Clear IF
    
    rtcWrite (0xD1);    //Llama a la direccion en modo escritura del rtc (D0 + 1)
    //Segundos
    RCEN1 = 1;          //Empieza a recibir
    while (!BF1);       //Salta interrupcion fin rx
    sec = SSP1BUF;      //lee el buffer
    BF1 = 0;            //reset stat del buffer
    ACKDT1 = 0;         //ACKDT1 = 0;
    ACKEN1 = 1;         //Envia ACK al slave
    PIR1bits.SSP1IF = 0;  
    __delay_ms (20);
    PIR1bits.SSP1IF = 0;//Clear IF
    //Minutos
    RCEN1 = 1;          //Empieza a recibir
    while (!BF1);       //Salta interrupcion fin rx
    min = SSP1BUF;      //lee el buffer
    BF1 = 0;            //reset stat del buffer
    ACKDT1 = 0;         //ACKDT1 = 0;
    ACKEN1 = 1;         //Envia ACK al slave
    PIR1bits.SSP1IF = 0;  
    __delay_ms (20);
    PIR1bits.SSP1IF = 0;//Clear IF
    //Horas
    RCEN1 = 1;          //Empieza a recibir
    while (!BF1);       //Salta interrupcion fin rx
    hora = SSP1BUF;     //lee el buffer
    BF1 = 0;            //reset stat del buffer
    ACKDT1 = 0;         //ACKDT1 = 0;
    ACKEN1 = 1;         //Envia ACK al slave
    PIR1bits.SSP1IF = 0;  
    __delay_ms (20);
    PIR1bits.SSP1IF = 0;//Clear IF
    //Dia de la semana
    RCEN1 = 1;          //Empieza a recibir
    while (!BF1);       //Salta interrupcion fin rx
    diaSem = SSP1BUF;   //lee el buffer
    BF1 = 0;            //reset stat del buffer
    ACKDT1 = 0;         //ACKDT1 = 0;
    ACKEN1 = 1;         //Envia ACK al slave
    PIR1bits.SSP1IF = 0;  
    __delay_ms (20);
    PIR1bits.SSP1IF = 0;//Clear IF
    //Dia del mes
    RCEN1 = 1;          //Empieza a recibir
    while (!BF1);       //Salta interrupcion fin rx
    dia = SSP1BUF;      //lee el buffer
    BF1 = 0;            //reset stat del buffer
    ACKDT1 = 0;         //ACKDT1 = 0;
    ACKEN1 = 1;         //Envia ACK al slave
    PIR1bits.SSP1IF = 0;  
    __delay_ms (20);
    PIR1bits.SSP1IF = 0;//Clear IF
    //Mes
    RCEN1 = 1;          //Empieza a recibir
    while (!BF1);       //Salta interrupcion fin rx
    mes = SSP1BUF;      //lee el buffer
    BF1 = 0;            //reset stat del buffer
    ACKDT1 = 0;         //ACKDT1 = 0;
    ACKEN1 = 1;         //Envia ACK al slave
    PIR1bits.SSP1IF = 0;  
    __delay_ms (20);
    PIR1bits.SSP1IF = 0;//Clear IF
    //Año
    RCEN1 = 1;          //Empieza a recibir
    while (!BF1);       //Salta interrupcion fin rx
    year = SSP1BUF;     //lee el buffer
    BF1 = 0;            //reset stat del buffer
    ACKDT1 = 0;         //ACKDT1 = 0;
    ACKEN1 = 1;         //Envia ACK al slave
    PIR1bits.SSP1IF = 0;  
    __delay_ms (20);
    PIR1bits.SSP1IF = 0;//Clear IF
    
    SSP1CON2bits.PEN = 1; //P Stop Condition
    
   Time.FormatedTime[0]=((hora&0x30)>>4)+48;   //hora         (hh:mm:ss)
   Time.FormatedTime[1]=(hora&0x0f)+48;        //hora
   Time.FormatedTime[2]=':';                 //:
   Time.FormatedTime[3]=((min&0x70)>>4)+48;   //minuto
   Time.FormatedTime[4]=(min&0x0f)+48;       //minuto
   Time.FormatedTime[5]=':';                //:
   Time.FormatedTime[6]=((sec&0x70)>>4)+48;   //sec
   Time.FormatedTime[7]=(sec&0x0f)+48;        //sec
   Time.FormatedTime[8]='\0';

   Time.FormatedDate[0]=((dia&0x30)>>4)+48;      //dia     (dd/mm/aa)
   Time.FormatedDate[1]=(dia&0x0f)+48;           //dia
   Time.FormatedDate[2]='/';                       //  /
   Time.FormatedDate[3]=((mes&0x10)>>4)+48;        //mes
   Time.FormatedDate[4]=(mes&0x0f)+48;              //mes
   Time.FormatedDate[5]='/';                     //  /
   Time.FormatedDate[6]=((year&0xf0)>>4)+48;      // año
   Time.FormatedDate[7]=(year&0x0f)+48;           // año
   Time.FormatedDate[8]='\0';
   Time.FormatedDate[9]=  diaSem;           //dia de la semana en formato HEX
}
 

#ifdef	__cplusplus
extern "C" {
#endif /* __cplusplus */



#ifdef	__cplusplus
}
#endif /* __cplusplus */

#endif	/* XC_FUNCIONESDS1307_H */

