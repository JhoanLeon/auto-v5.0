#include <16F877A.h>

#FUSES     NOWDT
#FUSES     XT
#FUSES     PUT
#FUSES     NOPROTECT
#FUSES     NODEBUG
#FUSES     NOBROWNOUT
#FUSES     NOLVP
#FUSES     NOCPD
#FUSES     NOWRT

#use standard_io(D)
#use standard_io(C)

#use delay(clock=4000000,crystal)

#include "lib_rf2gh4_10.h"

#byte porta=0x05           // Direcci?n del puerto A.

#int_ext                   // Interrupci?n del m?dulo RF.

int time = 200;            // Tiempo para la secuencia de inicio 
int8 velocity;             // Variable de velocidad del joystick
int8 position;             // Variable del potenciometro para servo

void int_RB0()
{
   int8 ret1;

   ret1 = RF_RECEIVE();
   if ( (ret1 == 0) || (ret1 == 1) ) // Tanto si hay recepci?n simple o m?ltiple, leer datos.
   {
      do
      {
         output_high(PIN_D1);          // Indicador que est?n llegando paquetes del transmisor
      
         porta = RF_DATA[0];           // El puerto A contendr? el valor que le llegue del emisor, a trav?s de RF_DATA[0].
         position = RF_DATA[1];        // Lee el valor mandado por el potenciometro de posici?n
         
         set_pwm2_duty(position);      // pone a la salida ccp2_pwm (RC1) el valor recibido del potenciometro
         delay_us(10);                 // tiempo de espera para la salida analoga         
         
         ret1 = RF_RECEIVE();        // "ret1" nos dir? si hay recepci?n simple, m?ltiple o no hay datos para leer.
      } while ( (ret1 == 0) || (ret1 == 1) ); // Mientras haya datos para leer, seguir leyendo.
   } 
   
   output_low(PIN_D1);
}


void main()
{
   setup_adc_ports(NO_ANALOGS);
   setup_adc(ADC_CLOCK_DIV_2);
   setup_vref(FALSE);
   
   setup_ccp1(CCP_PWM); // configura el ccp1 como salida PWM
   setup_timer_2(T2_DIV_BY_4, 255, 1); // configura el timer2 para el PWM
   setup_ccp2(CCP_PWM); // configura el ccp2 como salida PWM
   setup_timer_2(T2_DIV_BY_4, 255, 1); // configura el timer2 para el PWM
  
   set_tris_a(0b000000);     // Todo el puerto A como salida.
   porta = 0;                // Inicialmente lo ponemos a cero.
  
   RF_INT_EN();              // Habilitar interrupci?n RB0/INT.
   RF_CONFIG_SPI();          // Configurar m?dulos SPI del PIC.
   RF_CONFIG(0x40,0x08);     // Configurar m?dulo RF (canal y direcci?n).
   RF_ON();                  // Activar el m?dulo RF.
   
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
  
   while(true);              // Bucle infinito.            
      
}

