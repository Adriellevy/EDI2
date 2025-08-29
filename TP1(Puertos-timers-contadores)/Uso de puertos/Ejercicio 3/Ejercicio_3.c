#include <Ejercicio_3.h>
#FUSES INTRC_IO
#FUSES NOMCLR
#FUSES NOWDT

void GPIO(void);

void main()
{
   GPIO();
   while(TRUE)
   {
      int1 s1 = input(PIN_B1);   // Lee interruptor en RB1
      int1 s2 = input(PIN_B2);   // Lee interruptor en RB2

      if (s1 == s2) { // Mismo estado: hace intermitente el LED
         
        output_high(PIN_B4); //Prende el led
        delay_ms(500);  //Espera 500ms y lo apaga
        output_low(PIN_B4);   //Apaga el led
        delay_ms(500);  //Espera 500ms y lo prende
         
      } else { // Distinto estado: LED apagado (sin parpadeo)
         
         output_low(PIN_B4);
      }
   }      
}

void GPIO(void){
   set_tris_a(0xFF);
   set_tris_b(0b11101111);
}

