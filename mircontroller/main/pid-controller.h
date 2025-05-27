#ifndef PID-CONTROLLER_H
#define PID-CONTROLLER_H
struct pid {
  double Kp;
  double Ki;
  double Kd;
  double setpoint;
  double lastE;
  double dE;
  double sumE;
  double dt;
  double val;
}

#endif