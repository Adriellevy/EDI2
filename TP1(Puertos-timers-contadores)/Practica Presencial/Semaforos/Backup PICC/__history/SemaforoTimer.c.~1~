#include <Semaforo.h>
#FUSES NOWDT, NOMCLR

typedef enum {
    VERDE,
    AMARILLO,
    ROJO
} EstadoSemaforo;

void INITGPIO();

void main()
{
    INITGPIO();
    EstadoSemaforo estado = VERDE;
    output_toggle(PIN_A2);
    while (TRUE) {
        switch (estado) {
            case VERDE:
                delay_ms(500);  // 5 segundos
                output_toggle(PIN_A2);
                output_toggle(PIN_A3);
                estado = AMARILLO;
                break;

            case AMARILLO:            
                delay_ms(200);   // 2 segundos
                output_toggle(PIN_A3);
                output_toggle(PIN_A4);
                estado = ROJO;
                break;

            case ROJO:
                delay_ms(500);                 // 5 segundos
                output_toggle(PIN_A4);
                output_toggle(PIN_A2);
                estado = VERDE;
                break;
        }
    }
}

void INITGPIO()
{
   set_tris_a(0b00100000);
   output_a(0b00100000);
}
