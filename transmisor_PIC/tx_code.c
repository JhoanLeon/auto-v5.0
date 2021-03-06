#include <16F877A.h>

#fuses    NOWDT
#fuses    XT
#fuses    PUT
#fuses    NOPROTECT
#fuses    NODEBUG
#fuses    NOBROWNOUT
#fuses    NOLVP
#fuses    NOCPD
#fuses    NOWRT

#device adc=8

#use standard_io(D)
#use standard_io(E)

#use delay(clock=4000000,crystal)

#include "lib_rf2gh4_10.h"

#byte porta=0x05
#byte portb=0x06

#int_ext                     // Esta rutina est? para un futuro si haces comunicaciones bidireccionales.

int time = 200;              // Tiempo para la secuencia de inicio
int8 ret2;                   // Variable para la respuesta de emisi?n


void int_RB0()               // No tiene efecto en el programa principal, ya que s?lo emite.
{                            // Se encargar?a de la recepci?n de datos.
   int8 ret1;
  
   ret1 = RF_RECEIVE();
   if ( (ret1 == 0) || (ret1 == 1) )
   {
      do
      { 
         ret1 = RF_RECEIVE();
      }  while ( (ret1 == 0) || (ret1 == 1) );
   } 
}


void main()
{  
   setup_adc_ports(PIN_E0);     // configuracion de puertos analogicos
   setup_adc_ports(PIN_E1);
   setup_adc(ADC_CLOCK_INTERNAL);   // configura clock del ADC

   RF_INT_EN();              // Habilitar interrupci?n RB0/INT.
   RF_CONFIG_SPI();          // Configurar m?dulo SPI del PIC.
   RF_CONFIG(0x40,0x06);     // Configurar m?dulo RF canal y direcci?n de recepci?n. Para recibir datos en el emisor tiene la direcci?n 0x06.
   RF_ON();                  // Activar el m?dulo RF.
  
   delay_ms(5);              // Le damos un m?nimo de 2.5 milisegundos para que se ponga en marcha.
  
   set_tris_a(0b111111);     // Todo el puerto A como entradas.
   
   output_high(PIN_D1);      // Secuencia de inicio
   delay_ms(time);
   output_low(PIN_D1);
   delay_ms(time);
   output_high(PIN_D1);
   delay_ms(time);
   output_low(PIN_D1);
   delay_ms(time);
   output_high(PIN_D1);
   delay_ms(time);
   output_low(PIN_D1);
   
   while(true)
   {
      RF_DATA[0] = porta;      // Cargamos el dato que se quiere enviar en RF_DATA[0].
      
      set_adc_channel(5);      // configura el canal AN5 para lectura analoga de posici?n
      RF_DATA[1] = read_adc(); // Lee el canal AN5 y lo guarda en el arreglo para enviar
      delay_us(5);             // tiempo de espera para lectura analogica
      
      set_adc_channel(6);      // configura el canal AN6 para lectura analoga de velocidad
      RF_DATA[2] = read_adc(); // Lee el canal AN6 y lo guarda en el arreglo para enviar
      delay_us(5);             // tiempo de espera para lectura analogica 
      
      RF_DIR = 0x08;           // Direcci?n del receptor.
      ret2 = RF_SEND();        // Envia el dato de RF_DATA y guarda la respuesta de env?o en ret2.
  
      if(ret2==0)              //Envio realizado y ACK recibido
      {
         //output_high(PIN_D0);
      }
      else if(ret2==1)         //Envio realizado y ACK no recibido
      {
         //output_high(PIN_D1);
      }
      else                     //Envio no realizado
      {
         //output_high(PIN_D2);
      }
      
      //output_low(PIN_D0);
      //output_low(PIN_D1);
      //output_low(PIN_D2);
      //delay_ms(time);
   }
}
