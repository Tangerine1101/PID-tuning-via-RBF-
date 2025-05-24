#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <wiringPi.h>

#define DIR_1_PIN   23
#define DIR_2_PIN   25
#define PWM_PIN     21
#define ENC_A_PIN   5
#define ENC_B_PIN   3

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

void motor_set_direction_and_speed(int dir1_val, int dir2_val, int speed_pwm) {
    digitalWrite(DIR_1_PIN, dir1_val);
    digitalWrite(DIR_2_PIN, dir2_val);
    pwmWrite(PWM_PIN, speed_pwm);
}

void motor_forward(int speed_pwm) {
    motor_set_direction_and_speed(HIGH, LOW, speed_pwm);
}

void motor_backward(int speed_pwm) {
    motor_set_direction_and_speed(LOW, HIGH, speed_pwm);
}

void motor_stop(void) {
    motor_set_direction_and_speed(LOW, LOW, 0);
}

int main(void) {
    setup_gpio();

    int pwm_drive_value = 700;

    printf("Dong co chay tien...\n");
    motor_forward(pwm_drive_value);
    delay(3000);

    printf("Dong co dung.\n");
    motor_stop();
    delay(1000);
    printf("So xung encoder hien tai: %ld\n", encoder_pulse_count);


    long pulses_before_reverse = encoder_pulse_count;
    printf("Dong co chay lui...\n");
    motor_backward(pwm_drive_value);
    delay(3000);

    printf("Dong co dung.\n");
    motor_stop();
    delay(1000);
    printf("So xung encoder hien tai: %ld\n", encoder_pulse_count);
    printf("So xung trong luc chay lui: %ld\n", encoder_pulse_count - pulses_before_reverse);


    printf("\nTheo doi toc do (Ctrl+C de thoat):\n");
    while (1) {
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
    }

    return 0;
}
