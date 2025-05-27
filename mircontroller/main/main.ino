#include"pid-controller.h"

#define EN 6
#define M1 7
#define M2 8
#define enA 2
#define enB 3

//motor
#define FORWARD 1
#define STOP 0
#define REVERSE -1

//encoder related
volatile long enCount = 0;
double t0 = 0, t1 = 0;

double filter[3]= {0, 0, 0};
int fillerCount;
void filterX(double* val){
  filter[fillerCount] = *val;
  if(filter[2] ==0 || filter[1] ==0 || filter[0] ==0){
    *val =0;
  }
  else {
    *val = (filter[0] + filter[1] + filter[2])/3;
  }
  fillerCount++;
}

pid controller;
void setup(){
  pidinit(&controller, 1, 0.001, 0, 200, 0.01);

  Serial.begin(9600);
  pinMode(EN, 1);
  pinMode(M1, 1);
  pinMode(M2, 1);
  pinMode(enA, 0);
  pinMode(enB, 0);
  attachInterrupt(digitalPinToInterrupt(enA), encoderISRA, CHANGE);
  attachInterrupt(digitalPinToInterrupt(enB), encoderISRB, CHANGE);
  }
void loop() {
  //
  double u=0;
  t1 = enCount - t0;
  double spd = rpm(t1, (float)controller.dt/1000);
  pidCompute(&controller,controller.setpoint - spd);
  motor(M1, M2, EN, FORWARD, controller.val);
  double t = (float)millis() /1000;
  printCSV(t,spd);
  
  t0 = enCount;
  delay(controller.dt);
}

void motor(int pinA, int pinB, int pinEN, int dir, int power) {
  int PWM = map(power, 0, 100, 0, 255);
  if (dir == 0 || power < 5) {
    digitalWrite(pinA, 0);
    digitalWrite(pinB, 0);
    analogWrite(pinEN, 0);
  } else if (dir > 0) {
    digitalWrite(pinA, 1);
    digitalWrite(pinB, 0);
    analogWrite(pinEN, PWM);
  } else {
    digitalWrite(pinA, 1);
    digitalWrite(pinB, 0);
    analogWrite(pinEN, PWM);
  }
}

void printCSV(float x, float y) {
  Serial.print((float)x, 2);
  Serial.print(",");
  Serial.println((float)y, 2);
}

double rpm(long pulse, double sec){
  double u = ((float)pulse*60)/(4*11*9.6*sec);
  return u;
}

void encoderISRA(){
  enCount++;
}
void encoderISRB(){
  enCount++;
}