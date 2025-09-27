#include <Ejercicio_2.h>
#fuses INTRC_IO, NOWDT, NOMCLR

#define LCD_ENABLE_PIN  PIN_B0 // Librerias para el LCD
#define LCD_RS_PIN      PIN_B1
#define LCD_RW_PIN      PIN_B2
#define LCD_DATA4       PIN_B4
#define LCD_DATA5       PIN_B5
#define LCD_DATA6       PIN_B6
#define LCD_DATA7       PIN_B7
#include <lcd.c>

void InitGPIO();
void InitTimer1();

volatile int contador = 0;   // cuenta de interrupciones
volatile int flag_timeout = 0;  // bandera de inactividad

#INT_TIMER1
void Timer1_ISR(void) {
   contador++;
   if (contador >= 4) {   // 2 interrupciones de 500ms = 1 segundo
      flag_timeout = 1;   // activa bandera de espera
      contador = 0;
      set_timer1(65536 - 6250); 
   }
}

void main() {
   InitGPIO();
   InitTimer1();
   lcd_putc("\fEsperando Input");

   while(TRUE) 
   {
      if(input(PIN_A0) == 1) { // Boton 1
         lcd_putc("\fPRENDIENDO");
         flag_timeout = 0;     // se reinicia el contador de inactividad
         contador = 0;
      }
      else if(input(PIN_A1) == 1) { // Boton 2
         lcd_putc("\fAPAGANDO");
         flag_timeout = 0;
         contador = 0;
      }
      else if(flag_timeout) { // si paso 1s sin presionar nada
         lcd_putc("\fEsperando Input");
         flag_timeout = 0; // resetea bandera, se seguirá mostrando hasta que aprieten algo
      }
   }
}

void InitGPIO(void){
   set_tris_a(0b11000000);   // A es ENTRADA
   set_tris_b(0b00000000);   // PORTB como SALIDA
   lcd_init(); 
}

void InitTimer1(void){
   setup_timer_1(T1_INTERNAL | T1_DIV_BY_8); 
   set_timer1(65536 - 6250); 
   enable_interrupts(INT_TIMER1);
   enable_interrupts(GLOBAL);
}

