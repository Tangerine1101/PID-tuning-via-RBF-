#include"pid-controller.h"

void pidinit(pid* controller,double kp, double ki, double kd, double setpoint, double sample_time){
  if (controller == 0){
    return;
  }
  controller->Kp = kp;
  controller->Ki = ki;
  controller->Kd = kd;
  controller->setpoint = setpoint;
  controller->dt = sample_time;
  controller->dE = 0;
  controller->lastE = 0;
  controller->sumE = 0;
  controller->val = 0;
}

void pidCompute(double* val, double Kp, double Ki, double Kd, double E, double* dE, double* lastE, double* sumE){
    //convert rpm into 
    *val = Kp * E + Ki * (*sumE) + Kd * (*dE);
    constrain(*val, 0, 100);
    *sumE += E;
    *dE = (E - (*lastE)) / dt;
    *lastE = E;
  }

void pidCompute1(pid* controller, double error){
  double P = controller->Kp * error;
  double I = controller->Ki * controller->sumE;
  double D = controller->Kd * controller->dE;
  controller->val = P + I + D;
  controller->sumE += error;
  controller->dE = (error- controller->lastE )/controller->dt;
  controller->lastE = error;
}