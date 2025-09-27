#include <Ejercicio_1.h>
#fuses INTRC_IO, NOWDT, NOMCLR
#use delay(clock=4000000)

#define LCD_ENABLE_PIN  PIN_B0 // Librerías para el LCD
#define LCD_RS_PIN      PIN_B1
#define LCD_RW_PIN      PIN_B2
#define LCD_DATA4       PIN_B4
#define LCD_DATA5       PIN_B5
#define LCD_DATA6       PIN_B6
#define LCD_DATA7       PIN_B7
#include <lcd.c>

void InitGPIO();
void InitTimer();

// Variables globales
volatile int flag_1s = 0;
volatile int cuenta50ms = 0;

// Interrupción del Timer1
#INT_TIMER1
void isr_timer1(void) {
   // Recargar Timer1 para 50 ms
   set_timer1(65536 - 6250);  // 6250 ticks * 8us = 50 ms
   cuenta50ms++;
   if(cuenta50ms >= 20) {  // 20 * 50ms = 1s
      cuenta50ms = 0;
      flag_1s = 1;   // Señal para el main
   }
}



void main() {
   InitGPIO();
   
   int contador = 0;
   int estado = 0;
   InitTimer();
   
   while(TRUE) 
   {
      if(flag_1s) {   // Cada 1 segundo exacto
         flag_1s = 0; 

         switch(estado) {
            case 0:
               lcd_putc("\fHello World"); 
               estado = 1;
               break;

            case 1:
               lcd_putc("\fBienvenidos a\nEDI2");
               estado = 2;
               break;

            case 2:
               contador++;
               printf(lcd_putc, "\fContador = %d", contador);
               estado = 0;
               break;
         }
      }
   }
}

void InitGPIO(void){
    set_tris_a(0b01111111);   // A es ENTRADA
    set_tris_b(0b00000000);   // PORTB como SALIDA
    lcd_init(); 
}
void InitTimer(void){
   setup_timer_1(T1_INTERNAL | T1_DIV_BY_8);  
   set_timer1(65536 - 6250);  
   enable_interrupts(INT_TIMER1);
   enable_interrupts(GLOBAL);
}
