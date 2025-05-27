#include"pid-controller.h"
#include"system_performance.h"

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
//speed filter: take average of 3 lastest raw rpm value for better output signal
const int filter_mode = 10;
double filter[filter_mode];
int filterCount =0;
void filterX(double* val){
  filter[filterCount] = *val;
  double sum =0.0;
  unsigned int count =0;
  //skip 0 value(for initial period)
  for(unsigned int i =0; i < filter_mode; i++){
    if (filter[i] >= 0.001) {
      sum += filter[i];
      count++;
    }
    if (i == (filter_mode -1) && count != 0){
     *val = sum/count;
    }
  }
  filterCount = (filterCount +1) % filter_mode; 
}
//
double setpoint =200;
pid controller;
sys_per outputVal;
sys_criteria criteria;
bool flag_run_end =0;
unsigned long wait =0, time_limit = 5000;
unsigned long runtime() {
  if ( ((float)millis() - (float)wait) >= 0){
    return (millis() - wait);
  }
  else {
    return 0;
  }
}

void setup(){
  Serial.begin(9600);
  while(!Serial);
  //CHANGE CONFIGURATION HERE:
  pidinit(&controller, 5, 0.05, 0.0, setpoint, 0.01); //pid struct, Kp, Ki, Kd, setpoint(rpm), sample time(s)
  SysPer_init(&outputVal, &criteria, setpoint, 2, 1, 0.1, 0.1); //sys_per struct, sys_criteria struct, setpoint, Steady-state error, overshoot(%), time rise(s), time settle(s)

  pinMode(EN, 1);
  pinMode(M1, 1);
  pinMode(M2, 1);
  pinMode(enA, 0);
  pinMode(enB, 0);
  attachInterrupt(digitalPinToInterrupt(enA), encoderISRA, CHANGE);
  attachInterrupt(digitalPinToInterrupt(enB), encoderISRB, CHANGE);
  wait = millis();
  }
void loop() {
  unsigned long r = runtime();
  unsigned long m = millis();
  
  while(runtime() < time_limit && flag_run_end == 0){
    //pre-pid & control
    double u=0;
    t1 = enCount - t0;
    double spd = rpm(t1, (float)controller.dt);
    //PID & control
    pidCompute(&controller,controller.setpoint - spd);
    motor(M1, M2, EN, FORWARD, controller.val);

    //after PID & control
    filterX(&spd);
    double t = (float)runtime() /1000;
    printCSV(t,spd);
    bool flag_control = evaluate(&outputVal, criteria, spd, (float)runtime()/1000);
    t0 = enCount;
    delay(controller.dt *1000);
   } 
  if (flag_run_end == 0){
    print_performance(outputVal, criteria, (float)time_limit*0.99/1000);
    flag_run_end = 1;
    motor(M1, M2, EN, STOP, 0);
    Serial.println("set your new pid config");
    Serial.println("type 'setpid' for a new run");
  }
  if (cmd(&controller) == 1) {
    SysPer_init(&outputVal, &criteria, 200, 2, 1, 0.1, 0.1);
    flag_run_end = 0;
    enCount =0;
    filterCount =0;
    for(unsigned int i =0; i < filter_mode; i++){
    filter[i] =0;
    }
    t0 =0;
    t1 =0;
    wait = millis();
  }

}

//motor's hardware related functions
void motor(int pinA, int pinB, int pinEN, int dir, int power) {
  int PWM = map(power, 0, 1000, 0, 255);
  if (dir == 0) {
    digitalWrite(pinA, 0);
    digitalWrite(pinB, 0);
    analogWrite(pinEN, 0);
  } else if (dir > 0) {
    digitalWrite(pinA, 1);
    digitalWrite(pinB, 0);
    analogWrite(pinEN, PWM);
  } else {
    digitalWrite(pinA, 0);
    digitalWrite(pinB, 1);
    analogWrite(pinEN, PWM);
  }
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
//Serial interact
String readSerialLine() {
  String line = "";
  unsigned long startTime = millis(); // Bắt đầu bộ đếm thời gian chờ
  const unsigned long timeoutDuration = 30000; // Thời gian chờ tối đa (ví dụ: 5 giây)

  while (true) {
    if (Serial.available() > 0) {
      line = Serial.readStringUntil('\n');
      line.trim(); // Loại bỏ khoảng trắng thừa
      return line;
    }
    // Kiểm tra timeout để tránh bị treo nếu không có dữ liệu
    if (millis() - startTime > timeoutDuration) {
      Serial.println("Timeout! Khong nhan duoc du lieu.");
      return ""; // Trả về chuỗi rỗng nếu timeout
    }
  }
}

bool cmd(pid* controller) { 
  String command;

  if (Serial.available() > 0) {
    command = Serial.readStringUntil('\n');
    command.trim();
  }

  if (command.equalsIgnoreCase("SETPID")) {
    String kp_str, ki_str, kd_str, setpoint_str, sample_time_str;

    Serial.println("Nhap Kp:");
    kp_str = readSerialLine();

    Serial.println("Nhap Ki:");
    ki_str = readSerialLine();

    Serial.println("Nhap Kd:");
    kd_str = readSerialLine();

    pidinit(controller, kp_str.toDouble(), ki_str.toDouble(), kd_str.toDouble(), setpoint, 0.01);
    Serial.println("PID parameters updated!");

    return true; 
  } else {
    return false;
  }
}