#define EN 6
#define M1 7
#define M2 8
#define enA 2
#define enB 3

//motor
#define FORWARD 1
#define STOP 0
#define REVERSE -1

//pid variables
double Kp = 0.05, Ki = 0.001, Kd =0;
double setpoint = 150;                           //in RPM
double dt = 100;                                  //sample time, in milliseconds
double e = 0, last_e = 0, de = 0, sum_e = 0;
double buffer[3] = {0, 0, 0};
int count =0;

int a= digitalRead(enA), b =digitalRead(enB);
//encoder related
volatile long enCount = 0;
double t0 = 0, t1 = 0;

void setup(){
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
  double spd = rpm(t1, (float)dt/1000);
  Serial.print(count);
  double error = setpoint - spd;
  pidCompute(&u, Kp, Ki, Kd, error, &de, &last_e, &sum_e);
  motor(M1, M2, EN, FORWARD, 100);
  double t = (float)millis() /1000;
  printCSV(t,spd);
  
  t0 = enCount;
  delay(dt);
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

void pidCompute(double* val, double Kp, double Ki, double Kd, double E, double* dE, double* lastE, double* sumE){
  //convert rpm into 
  *val = Kp * E + Ki * (*sumE) + Kd * (*dE);
  constrain(*val, 0, 100);
  *sumE += E;
  *dE = (E - (*lastE)) / dt;
  *lastE = E;
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