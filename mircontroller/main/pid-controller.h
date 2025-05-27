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
  long dt;
  double val;
};

void pidinit(pid* controller,double kp, double ki, double kd, double setpoint, double sample_time);
void pidCompute(pid* controller, double error);

#endif