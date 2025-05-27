// === PHẦN MẠNG RBF ===

// Hàm khởi tạo các trọng số cho mạng RBF
void initializeRBFWeights() {
    // Khởi tạo trọng số mạng RBF với các giá trị khác nhau cho từng neuron
    // Ví dụ:
    // Trọng số cho Kp, Ki, Kd từ neuron 1
    rbf_weights[0][0] = 2.0;  // Kp
    rbf_weights[0][1] = 1.5;  // Ki
    rbf_weights[0][2] = 0.5;  // Kd
  
    // Trọng số cho Kp, Ki, Kd từ neuron 2
    rbf_weights[1][0] = 1.5;  // Kp
    rbf_weights[1][1] = 0.9;  // Ki
    rbf_weights[1][2] = 0.4;  // Kd
  
    // Trọng số cho Kp, Ki, Kd từ neuron 3
    rbf_weights[2][0] = 1.0;  // Kp
    rbf_weights[2][1] = 0.5;  // Ki
    rbf_weights[2][2] = 0.3;  // Kd
  }
  
  // === KẾT THÚC PHẦN MẠNG RBF ===

  // === PHẦN MẠNG RBF ===

// Hàm tính toán giá trị Kp, Ki, Kd từ mạng RBF dựa trên sai số e
void computePIDWithRBF(double error_input) {
    double phi[NUM_NEURONS_RBF];    // Mảng lưu trữ đầu ra của các hàm RBF (hàm kích hoạt)
    double totalPhi = 0;          // Tổng giá trị phi để chuẩn hóa
  
    // 1. Tính giá trị kích hoạt (phi) cho mỗi nơ-ron RBF
    for (int i = 0; i < NUM_NEURONS_RBF; i++) {
      // Sử dụng hàm Gaussian làm hàm cơ sở xuyên tâm
      phi[i] = exp(-pow(error_input - rbf_mu[i], 2) / (2 * pow(rbf_sigma[i], 2)));
      totalPhi += phi[i]; // Cộng dồn để chuẩn hóa sau này
    }
  
    // Khởi tạo lại Kp, Ki, Kd về 0 trước khi tính toán
    Kp_rbf = 0;
    Ki_rbf = 0;
    Kd_rbf = 0;
  
    // 2. Tính toán Kp, Ki, Kd bằng cách tổng hợp có trọng số các đầu ra RBF
    // Chuẩn hóa đầu ra của các nơ-ron (phi) để đảm bảo tổng các trọng số hiệu dụng là 1.
    if (totalPhi > 0) { // Tránh chia cho 0 nếu tất cả phi đều rất nhỏ hoặc bằng 0
      for (int i = 0; i < NUM_NEURONS_RBF; i++) {
        double normalized_phi = phi[i] / totalPhi;
        Kp_rbf += normalized_phi * rbf_weights[i][0]; // Tính Kp
        Ki_rbf += normalized_phi * rbf_weights[i][1]; // Tính Ki
        Kd_rbf += normalized_phi * rbf_weights[i][2]; // Tính Kd
      }
    } else {
      // Trường hợp totalPhi = 0 (có thể xảy ra nếu error_input quá xa tất cả các tâm mu
      // và sigma quá nhỏ). Cần có một chiến lược dự phòng, ví dụ: sử dụng giá trị mặc định.
      // Ở đây, ta có thể giữ Kp, Ki, Kd bằng 0 hoặc gán giá trị mặc định an toàn.
      // Ví dụ: lấy trọng số của nơ-ron đầu tiên nếu không muốn PID bằng 0.
      // Kp_rbf = rbf_weights[0][0];
      // Ki_rbf = rbf_weights[0][1];
      // Kd_rbf = rbf_weights[0][2];
      // Tuy nhiên, trong mã gốc, chúng sẽ giữ giá trị 0 nếu totalPhi = 0.
    }
  }
  
  // === KẾT THÚC PHẦN MẠNG RBF ===