#include <main.h> 

#use rs232(UART1, baud=9600, parity=N, bits=8, stop=1, stream=SER)

// I2C
#use i2c(MASTER, I2C1, FAST=100000)


#define ADDR_W(a7)  (((a7)<<1) | 0)
#define ADDR_R(a7)  (((a7)<<1) | 1)

// Dirección 7-bit del MAX128 
#define MAX128_ADDR7  0x50     
#define MAX128_W      ((MAX128_ADDR7<<1) | 0)  // 0x90
#define MAX128_R      ((MAX128_ADDR7<<1) | 1)  // 0x91



int1 flag_tickH = 0;
unsigned int16 acum_ms = 0;
unsigned int16 H_ms = 500;       // C0=100, C1=500, C2=10000

#int_timer1
void isr_t1(void){
   // Para 1ms necesitamos 2000 ticks Calculo 65536-2000 = 63536
   set_timer1(63536);
   if (++acum_ms >= H_ms){
      acum_ms = 0;
      flag_tickH = 1;
   }
}


typedef enum {
   ST_ARRANQUE,
   ST_ESPERA,
   ST_PAUSADO,
   ST_CICLO_LECTURA,
   ST_MOSTRAR,
   ST_ERROR_I2C
} estado_app_t;

typedef enum {
   I2C_ELEGIR_CANAL,
   I2C_COMIENZO_ESCRIBIR,
   I2C_MANDAR_CONTROL,
   I2C_REINICIO_LEER,
   I2C_LEER_ALTO,
   I2C_LEER_BAJO,
   I2C_GUARDAR_Y_SEGUIR
} estado_i2c_t;

estado_app_t estado_app = ST_ARRANQUE;
estado_i2c_t estado_i2c = I2C_ELEGIR_CANAL;

int1 muestreo_activo = 1;   // S1/S0
int8 canal = 0;             // 0:CH0 (Dx), 1:CH1 (Dy), 2:CH2 (Dz)
unsigned int16 valores[3] = {0,0,0};

// 
static void hw_init(void){
   // Desactivar analógico en todos los pines 
   setup_adc_ports(NO_ANALOGS);

   // Timer1 1ms
   setup_timer_1(T1_INTERNAL | T1_DIV_BY_1);
   set_timer1(63536);
   clear_interrupt(INT_TIMER1);
   enable_interrupts(INT_TIMER1);
   enable_interrupts(GLOBAL);

   // Mensaje de arranque
   printf(SER, "\r\n[BOOT] 16F1827 @8MHz | RS232 9600 | I2C 100kHz\r\n");
}


int1 i2c_escribir(byte b){
   return (i2c_write(b) == 0);  // true si ACK
}

// Byte de control según canal 
byte ctrl_por_canal(int8 ch){
   byte c = 0b10000000;
   c |= (ch & 0x07);        
   return c;
}

// Preparar lectura
int1 max128_preparar(int8 ch){
   i2c_start();
   if(!i2c_escribir(MAX128_W)){ i2c_stop(); return 0; }          
   if(!i2c_escribir(ctrl_por_canal(ch))){ i2c_stop(); return 0; } 
   i2c_start();
   if(!i2c_escribir(MAX128_R)){ i2c_stop(); return 0; }          
   return 1;
}

// Lee 12 bits
int1 max128_leer12(int8 ch, unsigned int16 *out){
   if(!max128_preparar(ch)) return 0;

   byte hi = i2c_read(1);     // ACK para pedir siguiente
   byte lo = i2c_read(0);     // NACK último byte
   i2c_stop();

   unsigned int16 raw16 = make16(hi, lo);

   unsigned int16 v12 = (raw16 >> 4);       // limpia 4 LSB "basura"
   *out = v12;
   return 1;
}

// Lectura de 3 canales (CH0, CH1, CH2)
int1 max128_leer_triple(unsigned int16 *Dx, unsigned int16 *Dy, unsigned int16 *Dz){
   if(!max128_leer12(0, Dx)) return 0;
   if(!max128_leer12(1, Dy)) return 0;
   if(!max128_leer12(2, Dz)) return 0;
   return 1;
}

// Scanner 
void i2c_scan(void){
   printf(SER,"[SCAN] I2C:\r\n");
   for (int8 a=0; a<128; a++){
      i2c_start();
      int1 nack = i2c_write(ADDR_W(a));
      i2c_stop();
      if(!nack) printf(SER,"  ACK @0x%02X (W=0x%02X R=0x%02X)\r\n", a, ADDR_W(a), ADDR_R(a));
      delay_ms(3);
   }
}

static int1 have_cmd0 = 0;
static char cmd0;

void procesar_rx(void){
   while (kbhit(SER)){
      char c = getc(SER);
      if(!have_cmd0){
         if (c=='S' || c=='C' || c=='M'){ have_cmd0=1; cmd0=c; }
      }else{
         have_cmd0=0;
         if(cmd0=='S'){ muestreo_activo = (c=='1'); printf(SER,"[S] %u\r\n", muestreo_activo); }
         else if(cmd0=='C'){
            if(c=='0') H_ms=100; else if(c=='1') H_ms=500; else if(c=='2') H_ms=10000;
            printf(SER,"[C] H=%lu ms\r\n", (unsigned long)H_ms);
         }else if(cmd0=='M'){
            if(c>='0' && c<='2'){ canal = c-'0'; estado_app = ST_CICLO_LECTURA; }
         }
      }
      if(c=='\r' || c=='\n'){ have_cmd0=0; }
   }
}

// -------------------- MAIN -----------------------
void main(){
   hw_init();


   while(TRUE){
      procesar_rx();

      switch(estado_app){

         case ST_ARRANQUE:
            printf(SER,"[ARRANQUE] OK\r\n");
            estado_app = ST_ESPERA;
            break;

         case ST_ESPERA:
            if(!muestreo_activo){ estado_app = ST_PAUSADO; break; }
            if(flag_tickH){
               flag_tickH=0;
               canal = 0;
               estado_i2c = I2C_ELEGIR_CANAL;
               estado_app = ST_CICLO_LECTURA;
            }
            break;

         case ST_PAUSADO:
            if(muestreo_activo) estado_app = ST_ESPERA;
            break;

         case ST_CICLO_LECTURA:
            switch(estado_i2c){
               case I2C_ELEGIR_CANAL:
                  estado_i2c = I2C_COMIENZO_ESCRIBIR;
                  break;

               case I2C_COMIENZO_ESCRIBIR:
                  i2c_start();
                  if(!i2c_escribir(MAX128_W)){ i2c_stop(); estado_app=ST_ERROR_I2C; }
                  else estado_i2c = I2C_MANDAR_CONTROL;
                  break;

               case I2C_MANDAR_CONTROL:
                  if(!i2c_escribir(ctrl_por_canal(canal))){ i2c_stop(); estado_app=ST_ERROR_I2C; }
                  else { i2c_start(); if(!i2c_escribir(MAX128_R)){ i2c_stop(); estado_app=ST_ERROR_I2C; } else estado_i2c = I2C_LEER_ALTO; }
                  break;

               case I2C_LEER_ALTO: {
                     byte hi = i2c_read(1);               // ACK
                     valores[canal] = ((unsigned int16)hi)<<8;
                     estado_i2c = I2C_LEER_BAJO;
                  } break;

               case I2C_LEER_BAJO: {
                     byte lo = i2c_read(0);               // NACK
                     i2c_stop();
                     unsigned int16 raw16 = make16((valores[canal]>>8)&0xFF, lo);
                     // Limpieza de 4 bits basura:
                     valores[canal] = (raw16 >> 4);       // usar &0x0FFF si hiciera falta
                     estado_i2c = I2C_GUARDAR_Y_SEGUIR;
                  } break;

               case I2C_GUARDAR_Y_SEGUIR:
                  if(canal < 2){ canal++; estado_i2c = I2C_ELEGIR_CANAL; }
                  else { estado_app = ST_MOSTRAR; }
                  break;
            }
            break;

         case ST_MOSTRAR:
            // Enviá por consola (podés reemplazar por LCD si lo tenés)
            printf(SER,"D:%lu,%lu,%lu\r\n",
                   (unsigned long)valores[0],
                   (unsigned long)valores[1],
                   (unsigned long)valores[2]);
            estado_app = ST_ESPERA;
            break;

         case ST_ERROR_I2C:
            printf(SER,"[ERR] I2C NACK\r\n");
            estado_app = ST_ESPERA;   // reintenta en el próximo tick
            break;
      }
   }
}

