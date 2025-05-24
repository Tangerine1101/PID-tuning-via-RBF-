#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>
#include <linux/i2c.h>

// --- General Definitions ---
#define LCD_I2C_ADDRESS         0x27
const char *I2C_BUS_PATH      = "/dev/i2c-2";

// --- LCD Definitions ---
#define LCD_RS_PIN              (1 << 0)
#define LCD_E_PIN               (1 << 2)
#define LCD_BL_PIN              (1 << 3)
#define LCD_D4_PIN              (1 << 4)
#define LCD_D5_PIN              (1 << 5)
#define LCD_D6_PIN              (1 << 6)
#define LCD_D7_PIN              (1 << 7)
#define LCD_COMMAND_MODE        0
#define LCD_DATA_MODE           1

static int i2c_fd = -1;

void delay_ms(long ms) { usleep(ms * 1000); } 
void delay_us(long us) { usleep(us); }       
uint8_t bcd2dec(uint8_t bcd) { return ((bcd >> 4) * 10) + (bcd & 0x0F); } 
uint8_t dec2bcd(uint8_t dec) { return ((dec / 10) << 4) + (dec % 10); } 


int i2c_bus_init(const char *device_path) {
    if (i2c_fd >= 0) close(i2c_fd);
    i2c_fd = open(device_path, O_RDWR);
    return (i2c_fd < 0) ? -1 : 0;
}

void i2c_bus_deinit(void) {
    if (i2c_fd >= 0) {
        close(i2c_fd);
        i2c_fd = -1;
    }
}

int i2c_select_device(int addr) {
    if (i2c_fd < 0) return -1;
    return (ioctl(i2c_fd, I2C_SLAVE, addr) < 0) ? -1 : 0;
}

// --- LCD Functions ---
void lcd_pcf8574_write_byte(uint8_t data) {
    if (i2c_fd < 0) return;
    write(i2c_fd, &data, 1);
}

void lcd_send_nibble(uint8_t nibble, uint8_t mode) {
    uint8_t pcf_data = (mode ? LCD_RS_PIN : 0) | LCD_BL_PIN;
    if (nibble & 0x01) pcf_data |= LCD_D4_PIN;
    if (nibble & 0x02) pcf_data |= LCD_D5_PIN;
    if (nibble & 0x04) pcf_data |= LCD_D6_PIN;
    if (nibble & 0x08) pcf_data |= LCD_D7_PIN;

    lcd_pcf8574_write_byte(pcf_data | LCD_E_PIN);
    delay_us(1);
    lcd_pcf8574_write_byte(pcf_data & ~LCD_E_PIN);
    delay_us(50);
}

void lcd_send_byte(uint8_t byte, uint8_t mode) {
    lcd_send_nibble((byte >> 4) & 0x0F, mode);
    lcd_send_nibble(byte & 0x0F, mode);
}

void lcd_init(void) {
    if (i2c_select_device(LCD_I2C_ADDRESS) != 0) return;
    delay_ms(50);
    lcd_send_nibble(0x03, LCD_COMMAND_MODE); delay_ms(5);
    lcd_send_nibble(0x03, LCD_COMMAND_MODE); delay_us(150);
    lcd_send_nibble(0x03, LCD_COMMAND_MODE);
    lcd_send_nibble(0x02, LCD_COMMAND_MODE);
    delay_us(150);

    lcd_send_byte(0x28, LCD_COMMAND_MODE); // Function set: 4-bit, 2 lines, 5x8 font
    lcd_send_byte(0x01, LCD_COMMAND_MODE); // Clear display
    delay_ms(2);
    lcd_send_byte(0x06, LCD_COMMAND_MODE); // Entry mode set
    lcd_send_byte(0x0C, LCD_COMMAND_MODE); // Display on
}

void lcd_set_cursor(uint8_t row, uint8_t col) {
    if (i2c_select_device(LCD_I2C_ADDRESS) != 0) return;
    uint8_t address = (row == 0) ? 0x00 : 0x40;
    address += col;
    lcd_send_byte(0x80 | address, LCD_COMMAND_MODE);
}

void lcd_print_string(const char *str) {
    if (i2c_select_device(LCD_I2C_ADDRESS) != 0) return;
    while (*str) {
        lcd_send_byte((uint8_t)(*str++), LCD_DATA_MODE);
    }
}

void lcd_clear(void) {
    if (i2c_select_device(LCD_I2C_ADDRESS) != 0) return;
    lcd_send_byte(0x01, LCD_COMMAND_MODE);
    delay_ms(2);
}

//---------------------------------------------------------------------------------------------------------------------------------------------
int main(int argc, char *argv[]) {

    

    return 0;
}

// aarch64-linux-gnu-gcc -static lcd.c -o lcd
