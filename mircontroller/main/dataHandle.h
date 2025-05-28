#ifndef DATAHANDLE_H
#define DATAHANDLE_H

#include "pid-controller.h"
#include "system_performance.h"
#include "rbf-.h"

String readSerialLine();
bool cmd(pid* controller);
void printCSV(float x, float y);
void print_performance(sys_per sys, sys_criteria criteria, double limit);
#endif