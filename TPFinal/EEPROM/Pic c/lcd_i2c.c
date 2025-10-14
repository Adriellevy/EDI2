// Driver CCS oficial simplificado para LCD por I2C con PCF8574
// Compatible con LCD tipo HD44780 (LM016L)
#include <lcd.c>

#define LCD_I2C_ADDRESS 0x4E   // Dirección del PCF8574
#use i2c(Master, SDA=PIN_B1, SCL=PIN_B4, slow)

void lcd_i2c_send_nibble(BYTE n) {
   BYTE data;
   data = (n << 4);
   i2c_start();
   i2c_write(LCD_I2C_ADDRESS);
   i2c_write(data | 0x04);    // EN=1
   delay_us(50);
   i2c_write(data & ~0x04);   // EN=0
   i2c_stop();
}

void lcd_i2c_send_byte(BYTE address, BYTE n) {
   BYTE hi, lo;
   hi = n >> 4;
   lo = n & 0x0F;
   lcd_i2c_send_nibble(hi);
   lcd_i2c_send_nibble(lo);
   delay_ms(2);
}

void lcd_init() {
   delay_ms(50);
   lcd_i2c_send_nibble(0x03);
   delay_ms(5);
   lcd_i2c_send_nibble(0x03);
   delay_us(150);
   lcd_i2c_send_nibble(0x02); // 4 bits
   lcd_i2c_send_byte(0, 0x28);
   lcd_i2c_send_byte(0, 0x0C);
   lcd_i2c_send_byte(0, 0x06);
   lcd_i2c_send_byte(0, 0x01);
   delay_ms(5);
}

