#include <Señal cuadrada por Timer0.h>


#FUSES NOWDT , NOMCLR
void INIT_GPIO();
void INIT_TIMER0();
void TIMER0();

void main()
{
   INIT_GPIO();
   INIT_TIMER0();
   while(TRUE)
   {

   
      //TODO: User Code
   }

}


Void INIT_TIMER0(){
   set_timer0(100);                    // arrancar TMR0 en 0
   clear_interrupt(INT_TIMER0); 
   setup_timer_0(RTCC_INTERNAL | RTCC_DIV_8);  
   // Fuente interna (Fosc/4) y prescaler 1:256
   enable_interrupts(INT_TIMER0);  // Habilitar interrupción Timer0
   enable_interrupts(GLOBAL); 
}

// --- Interrupción de TIMER0 ---
#INT_TIMER0 //Habilito interrupciones por timer0
void timer0_isr(void) {   // 8 desbordes ˜ 500 ms
      output_toggle(PIN_A0);       // Ejemplo: toggle LED en RB0
      set_timer0(100);
}

void INIT_GPIO(){
   set_tris_a(0b00011000);
   set_tris_b(0b00000001);
   output_a(0b00100001); 
   output_b(0b00000001); 
}
