#include <main.h>

#FUSES NOMCLR,NOWDT,INTRC_IO

void GPIO(void);


void main()
{
   GPIO();
   while(TRUE)
   {
      output_high(PIN_B0);
      output_high(PIN_B1);
      output_low(PIN_B2);
      output_low(PIN_B3);
   }

}

void GPIO(void){
   set_tris_a(0xFF);
   set_tris_b(0b11110000);
}
