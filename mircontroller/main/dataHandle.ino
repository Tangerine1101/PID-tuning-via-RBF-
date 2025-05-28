#include "dataHandle.h"
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
void printCSV(float x, float y) {
  Serial.print((float)x, 2);
  Serial.print(",");
  Serial.println((float)y, 2);
}
void print_performance(sys_per sys, sys_criteria criteria, double limit){
  Serial.println("---------------------------------------RESULT:----------------------------------");
  Serial.print("steady state error: "); Serial.println(sys.final_error);
  Serial.print("steady state value: "); Serial.println(sys.final_val);
  Serial.print("overshoot: "); Serial.println(sys.overshoot);
  Serial.print("time rise: ");
  if (sys.time_rise > 0.00001) { 
    Serial.println(sys.time_rise);
  }
  else {Serial.println("nan"); }

  Serial.print("time settle: "); 
  if(sys.time_settle <= limit) {
    Serial.println(sys.time_settle);
  }
  else {Serial.println("inf"); }
}