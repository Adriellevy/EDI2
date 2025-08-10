#include <16F1827.h>        // Librería del micro
#fuses INTRC_IO, NOWDT, NOPROTECT, NOLVP   // Configuración de fuses
#use delay(clock=4000000)  // Oscilador interno de 4 MHz

void main(){
   set_tris_a(0b11111100); // RA0 y RA1 salidas, el resto entradas
   output_high(PIN_A0);// RA0 como salida
   output_high(PIN_A1);// RA1 como salida
   while(TRUE){
        
   
   }
}
