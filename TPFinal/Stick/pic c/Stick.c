#include <16F1827.h>
#device ADC=10
#fuses NOWDT, INTRC_IO, NOPROTECT, NOLVP, NOMCLR
#use delay(clock=4MHz)
#use RS232(BAUD=9600, BITS=8, PARITY=N, XMIT=PIN_B5, RCV=PIN_B1)

// ---------------- Definiciones ----------------
#define PIN_X   PIN_A0    // Eje X del joystick (VRx)
#define PIN_Y   PIN_A1    // Eje Y del joystick (VRy)
#define PIN_BTN PIN_B0    // Botón del joystick (SW)

// ---------------- Programa principal ----------------
void main() {
    // Configuración básica
    setup_oscillator(OSC_4MHZ);
    setup_adc_ports(sAN0 | sAN1, VSS_VDD);
    setup_adc(ADC_CLOCK_INTERNAL);

    set_tris_a(0b00000011); // RA0 y RA1 entradas analógicas
    set_tris_b(0b00000001); // RB0 entrada digital (botón)

    unsigned int16 ejeX, ejeY;           // Originales 0-1023
    unsigned int8  ejeX_8bit, ejeY_8bit; // Convertidos 0-255
    int1 boton;

    printf("\r\n--- Lectura Joystick PIC16F1827 ---\r\n");

    while(TRUE) {
    // Leer eje X
    set_adc_channel(0);
    delay_us(20);
    ejeX = read_adc();

    // Leer eje Y
    set_adc_channel(1);
    delay_us(20);
    ejeY = read_adc();

    // Leer botón (activo en 0)
    boton = input(PIN_BTN);

    // Convertir de 10 bits (0-1023) a 8 bits (0-255)
    ejeX_8bit = ejeX >> 2;
    ejeY_8bit = ejeY >> 2;
    
    
    // ***** INICIO DE LA MODIFICACIÓN (ALTERNATIVA) *****
    
    // Usamos un IF/ELSE para decidir qué línea completa imprimir.
    // Esto evita poner lógica (como el '? :') dentro del printf.

    if (boton == 0) {
        // Si el botón está presionado (lee 0)
        printf("X=%4Lu (%3u) \t Y=%4Lu (%3u) \t BTN=PRESIONADO\r\n",
               ejeX, ejeX_8bit,
               ejeY, ejeY_8bit);
    } else {
        // Si el botón está suelto (lee 1)
        printf("X=%4Lu (%3u) \t Y=%4Lu (%3u) \t BTN=SUELTO\r\n",
               ejeX, ejeX_8bit,
               ejeY, ejeY_8bit);
    }
    
    // ***** FIN DE LA MODIFICACIÓN *****

    delay_ms(1000);
}
}
