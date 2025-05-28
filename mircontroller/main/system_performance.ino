#include"system_performance.h"
void SysPer_init(sys_per* system, sys_criteria* criteria, double setpoint, double Ess, double POT, double Tr, double Tss){
  criteria->final_error = Ess;
  criteria->overshoot = POT;
  criteria->time_rise = Tr;
  criteria->time_settle = Tss;
  criteria->setpoint = setpoint;

  system->final_val =0;
  system->final_error =0;
  system->overshoot =0;
  system->time_rise =0;
  system->time_settle =0;
  system->counter =0;
  system->highest_val =0;
  system->flag_settle =0;
  system->timer =0;
  for (int i =0; i <10; i++){
    system->cached[i] = 0;
  }
}

bool evaluate(sys_per* system, sys_criteria criteria, double val, double runtime) {
  //steady state = average of 3 last value
  system->cached[system->counter] = val;
  system->counter = (system->counter + 1) % 10;
  double sum =0;
  for(unsigned int i =0; i < 10; i++){
    sum += system->cached[i];
  }
  system->final_val = sum/10;
  system->final_error = criteria.setpoint - system->final_val;
  if(system->final_error < 0){
    system->final_error = -system->final_error;
  }
  //overshoot calculate
  if(system->highest_val < val){
    system->highest_val = val;
  }
  else {
    if(system->final_val != 0){
      system->overshoot = (system->highest_val - system->final_val)*100/system->final_val;
    }
  }
  //time rise calculate
  if(val >= criteria.setpoint*0.1 && system->timer == 0) {
    system->timer = runtime;
  }
  else if(val > criteria.setpoint*0.9 && system->timer != -1){ //change to else if for better performance
    system->time_rise = runtime - system->timer;
    system->timer = -1;
  }
  //time settle calculate
  double rate = 0.05;
  if((val >= criteria.setpoint*(1- rate) && val <= criteria.setpoint*(1+rate)) && system->flag_settle != 1){
    system->time_settle = runtime;
    system->flag_settle = 1;
  }
  else if(val < criteria.setpoint*(1- 2*rate) || val > criteria.setpoint*(1 + 2*rate)){
    system->flag_settle = 0;
  }
  //return value
  if ((system->final_error <= criteria.final_error) && system->overshoot <= criteria.overshoot && system->time_rise <= criteria.time_rise && system->time_settle <= criteria.time_settle){
    return 1;
  }
  else{
    return 0;
  }
}

