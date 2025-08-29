#include <16F1827.h>
#device ADC=10
#use delay(internal=4MHz)


#fuses XT, NOWDT
#use delay(clock=4000000)  // Frecuencia del cristal (4 MHz en este ejemplo)

void main() {
   int8 contador = 0;      // Variable para contar de 0 a 255

   set_tris_b(0x00);       // Puerto B como salida (0 = salida, 1 = entrada)
   output_b(0x00);         // Inicializa el puerto B en 0

   while(TRUE) {
   if(contador<10){
      output_b(contador);  // Muestra el valor en los LEDs
   }else{
      contador=0; 
      output_b(contador); 
   }
   delay_ms(200);       // Espera 200 ms
      contador++;          // Incrementa el contador (se reinicia automáticamente en 255 -> 0)
   }
}
