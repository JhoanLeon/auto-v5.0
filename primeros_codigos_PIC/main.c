#include <16f877A.h>

#device adc=10

#use delay(clock=4MHZ,crystal)
#fuses XT,NOWDT,NOPROTECT

#use standard_io(A) // analog input
#use standard_io(B) // output
#use standard_io(C) // input port / i2c,spi

int time = 150;

int16 velocity; // valor de velocidad del joystick
int16 position; // valor del potenciometro para servo

void main()
{
   //set_tris_a(0b00000011); // configura AN0 y AN1 como entrada
   setup_adc_ports(ALL_ANALOG); // configuracion de puertos analogicos
   setup_adc(ADC_CLOCK_INTERNAL); // configura clock del ADC
   
   setup_ccp1(CCP_PWM); // configura el ccp1 como salida PWM
   setup_timer_2(T2_DIV_BY_4, 255, 1); // configura el timer2 para el PWM
   setup_ccp2(CCP_PWM); // configura el ccp2 como salida PWM
   setup_timer_2(T2_DIV_BY_4, 255, 1); // configura el timer2 para el PWM

   output_high(PIN_B0);
   delay_ms(time);
   output_low(PIN_B0);
   delay_ms(time);
   output_high(PIN_B0);
   delay_ms(time);
   output_low(PIN_B0);
   delay_ms(time);
   output_high(PIN_B0);
   delay_ms(time);
   output_low(PIN_B0);
   
   while(true)
   {
//!      set_adc_channel(0); // configura el canal AN0 para lectura analoga
//!      delay_us(10); // tiempo de espera para lectura analogica
//!      velocity = read_adc(); // lee el canal cero AN0 y lo guarda en velocity
//!      set_pwm1_duty(velocity); // pone a la salida ccp1_pwm (RC2) el valor leido de AN0
//!      delay_us(20); // tiempo de espera para la salida analoga
      
      set_adc_channel(2); // configura el canal AN2 para lectura analoga
      position = read_adc(); // lee el canal cero AN2 y lo guarda en velocity
      delay_us(10); // tiempo de espera para lectura analogica
      set_pwm2_duty(position); // pone a la salida ccp2_pwm (RC1) el valor leido de AN2
      delay_us(20); // tiempo de espera para la salida analoga
      
//!      if( input(PIN_B7) == 1 ) // switche para las luces
//!      {
//!         output_high(PIN_B0);
//!      }
//!      output_low(PIN_B0);
//!           
//!      if( input(PIN_B6) == 1 )
//!      {
//!         output_high(PIN_B1);
//!      }
//!      output_low(PIN_B1);
//!      
//!      if( input(PIN_B5) == 1 )
//!      {
//!         output_high(PIN_B2);
//!      }
//!      output_low(PIN_B2);
      
   }
}


