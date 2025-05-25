#include <string.h>
#include <errno.h>
#include <wiringPi.h>

#include "print-lcd.c"

#define DIR_1_PIN   23
#define DIR_2_PIN   25
#define PWM_PIN     21
#define ENC_A_PIN   5
#define ENC_B_PIN   7
#define RESET_PIN   27

volatile long encoder_pulse_count = 0;
unsigned long last_speed_check_time = 0;
long last_encoder_pulse_count_for_speed = 0;

void encoder_isr(void) {
    encoder_pulse_count++;
}

void setup_gpio(void) {
    if (wiringPiSetup() == -1) {
        fprintf(stderr, "Khong the khoi tao wiringPi: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    pinMode(DIR_1_PIN, OUTPUT);
    pinMode(DIR_2_PIN, OUTPUT);
    pinMode(PWM_PIN, PWM_OUTPUT);

    pinMode(ENC_A_PIN, INPUT);
    pinMode(ENC_B_PIN, INPUT);
    pullUpDnControl(ENC_A_PIN, PUD_UP);

    if (wiringPiISR(ENC_A_PIN, INT_EDGE_RISING, &encoder_isr) < 0) {
        fprintf(stderr, "Khong the thiet lap ISR cho encoder: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    digitalWrite(DIR_1_PIN, LOW);
    digitalWrite(DIR_2_PIN, LOW);
    pwmWrite(PWM_PIN, 0);

    last_speed_check_time = millis();
    last_encoder_pulse_count_for_speed = encoder_pulse_count;
}

void motor(int speed){
    if (speed == 0){
    digitalWrite(DIR_1_PIN, 0);
    digitalWrite(DIR_2_PIN, 0);
    }    
    else if (speed >= 1){
    digitalWrite(DIR_1_PIN, 0);
    digitalWrite(DIR_2_PIN, 1);
    pwmWrite(PWM_PIN, speed);    
    }
    else if (speed <= -1){
    digitalWrite(DIR_1_PIN, 1);
    digitalWrite(DIR_2_PIN, 0);
    pwmWrite(PWM_PIN, speed);    
    }
}

int main(int argc, char *argv[]) {

    if (i2c_bus_init(I2C_BUS_PATH) != 0) {
        return 1;
    }

    lcd_init();
    lcd_clear();
    lcd_set_cursor(0, 3);
    lcd_clear();

    if (argc >= 2){
        lcd_print_string(argv[1]);
    }

    setup_gpio();
    int pwm_val =0;
    int val =10;
    while(digitalRead(RESET_PIN) ==0){
        pwm_val += val;
        int u = pwm_val;
        if (pwm_val >= 0);
        else {u = -pwm_val;}
        if (pwm_val ==255){
            val = -val;
        }
        else if (pwm_val == -255) {
            val = -val;
        }
        char lcd_str[16]; 
        motor(pwm_val);
        sprintf(lcd_str, "%03d", pwm_val);
        lcd_set_cursor(0,0);
        lcd_print_string(lcd_str);
        delay(200);
    }

    /*while (1) {
        unsigned long current_time = millis();
        long current_pulses = encoder_pulse_count;

        if (current_time - last_speed_check_time >= 1000) {
            double pulses_diff = (double)(current_pulses - last_encoder_pulse_count_for_speed);
            double time_diff_seconds = (double)(current_time - last_speed_check_time) / 1000.0;
            double speed_pps = 0.0;
            if (time_diff_seconds > 0) {
                 speed_pps = pulses_diff / time_diff_seconds;
            }

            printf("Tong so xung: %ld, Toc do: %.2f xung/giay\n", current_pulses, speed_pps);

            last_encoder_pulse_count_for_speed = current_pulses;
            last_speed_check_time = current_time;
        }
        delay(50);
    } */

    return 0;
}
